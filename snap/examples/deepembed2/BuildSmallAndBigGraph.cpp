#include "stdafx.h"
#include <vector>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include "biasedrandomwalk.h"
#include "BuildSmallAndBigGraph.h"

void computeWeight(PUNGraph & InNet, int & nodeId, THash<TInt, TInt> & N2C, 
	int & outEdgeCount, int & inEdgeCount, 
	int & clusterId,std::vector<THash<TInt, TInt> > & weightVec,
	PWNet & smallNet){
	
	bool res = false;
	//get current node
	TUNGraph::TNodeI currentNode = InNet->GetNI(nodeId);

	//for all the neighbors of the node
	for(int m = 0; m < currentNode.GetDeg(); m++){
		//get the neighbor
		int targetId = currentNode.GetNbrNId(m);
		int targetClusterId = N2C(targetId);
		//if the neighbor also in current cluster, increment in-cluster-edge count
		if(targetClusterId == clusterId){
			inEdgeCount++;
			
			if(! smallNet->IsNode(targetId))
				smallNet->AddNode(targetId);
			if(! smallNet->IsNode(nodeId))
				smallNet->AddNode(nodeId);
			if(! smallNet->IsEdge(nodeId, targetId))
				smallNet->AddEdge(nodeId, targetId);

		}else{
/* else the neighbor belongs to other cluster, increment out-cluster-edge count, 
this is a bdy node*/
			outEdgeCount++;
			res = true;
//also update the super graph weight between current community and the other community
// we only consider weightVec[i] containing all the edges between i and n where n > i
			if(targetClusterId > clusterId){
				// if in hashtable, increment weight, else set weight to be 1 
				if(weightVec[clusterId].IsKey(targetClusterId)){
					weightVec[clusterId][targetClusterId]++;
				}else{
					weightVec[clusterId][targetClusterId] = 1;
				}
			}else{
//if clusterId > targetClusterId, this edge should be handled before we come to this step
				assert(weightVec[targetClusterId].IsKey(clusterId) == true);
			}
		}
	}
}

void BuildSmallAndBigGraphToMemory(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PWNet> & NetVector, PWNet & SuperNet){
	
	PUNGraph OriginNet = PUNGraph::New();
	OriginNet = TSnap::ConvertGraph<PUNGraph, PWNet> (InNet);
	
	std::vector<THash<TInt, TInt> > weightVec;
	std::vector<int> conductances;
	std::cout<<"start building small graphs"<<std::endl;
	//for each commuinity
	#pragma omp parallel for schedule(dynamic)
	for(int i = 0; i < C2N.size(); i++){
/*create a new hashtable, then weightVec[i] is a hashtable mapping the other community
to the weight between them and community i*/
		THash<TInt, TInt>  OtherCluster2Weight;
		weightVec.push_back(OtherCluster2Weight);
		
		PWNet smallNet = PWNet::New();

		int outEdgeCount = 0;
		int inEdgeCount = 0;
		//for each node in current comminity
		for(int j = 0; j < C2N[i].size(); j++){
			computeWeight(OriginNet, j, N2C, outEdgeCount, inEdgeCount, i, weightVec, smallNet);
		}
		conductances.push_back((double)outEdgeCount/(double)inEdgeCount);
		NetVector.Add(smallNet);

		printf("\rBuilding process: %2lf%%",100*(double)i/(double)C2N.size());
    	fflush(stdout);
	}
	std::cout<<"finish building small graphs, there are "<<NetVector.Len()<<"in total"<<std::endl;
	std::cout<<"start building super graph!"<<std::endl;
	//super graph construction
	for(int i = 0; i < weightVec.size(); i++){
		if(!SuperNet->IsNode(i)){
			SuperNet->AddNode(i);
		}
		for(THash<TInt, TInt>::TIter HashI = weightVec[i].BegI(); HashI < weightVec[i].EndI(); HashI++){
			int neighbor = (*HashI).Key;
			int weight = (*HashI).Dat;
			if(! SuperNet->IsNode(neighbor)){
				SuperNet->AddNode(neighbor);
			}
			if(! SuperNet->IsEdge(i, neighbor)){
				SuperNet->AddEdge(i, neighbor);
				SuperNet->SetEDat(i, neighbor, weight);
			}
		}
		if(i % 100 == 0){
			printf("\rBuilding process: %2lf%%",100*(double)i/(double)weightVec.size());
    		fflush(stdout);
		}
	}
}

void writeOutGraph(std::string & GraphFolder, std::string & name, bool weighted, PWNet & Net){
	std::string filePath = GraphFolder+"/"+name;
	TStr NewStr(filePath.c_str());
	TFOut FOut(NewStr);
	for(TWNet::TEdgeI EI = Net->BegEI(); EI < Net -> EndEI(); EI++){
		FOut.PutInt(EI.GetSrcNId());
		FOut.PutCh(' ');
		FOut.PutInt(EI.GetDstNId());
		FOut.PutCh(' ');
		if(weighted){
			FOut.PutInt(EI.GetDat());
			FOut.PutLn();
		}
	}
}

void BuildSmallAndBigGraphToDisk(PWNet & InNet,std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, std::string & GraphFolder){

	PUNGraph OriginNet = PUNGraph::New();
	OriginNet = TSnap::ConvertGraph<PUNGraph, PWNet> (InNet);

	std::vector<THash<TInt, TInt> > weightVec;
	std::vector<int> conductances;
	std::cout<<"start building small graphs"<<std::endl;
	PWNet SuperNet = PWNet::New();
	//for each commuinity
	for(int i = 0; i < C2N.size(); i++){
		// create a new hashtable, then weightVec[i] is a hashtable mapping the other community
		// to the weight between them and community i
		THash<TInt, TInt>  OtherCluster2Weight;
		weightVec.push_back(OtherCluster2Weight);
		
		PWNet smallNet = PWNet::New();

		int outEdgeCount = 0;
		int inEdgeCount = 0;
		//for each node in current comminity
		for(int j = 0; j < C2N[i].size(); j++){
			computeWeight(OriginNet, j, N2C, outEdgeCount, inEdgeCount, i, weightVec, smallNet);
		}
		conductances.push_back((double)outEdgeCount/(double)inEdgeCount);

		std::string name;
		std::ostringstream convert;
		convert << i;
		name = convert.str();
		writeOutGraph(GraphFolder, name, false, smallNet);

		printf("\rBuilding process: %2lf%%",100*(double)i/(double)C2N.size());
    	fflush(stdout);
	}
	// std::cout<<"finish building small graphs, there are "<<NetVector.size()<<"in total"<<std::endl;
	std::cout<<"start building super graph!"<<std::endl;
	//super graph construction
	for(int i = 0; i < weightVec.size(); i++){
		if(!SuperNet->IsNode(i)){
			SuperNet->AddNode(i);
		}
		for(THash<TInt, TInt>::TIter HashI = weightVec[i].BegI(); HashI < weightVec[i].EndI(); HashI++){
			int neighbor = (*HashI).Key;
			int weight = (*HashI).Dat;
			if(! SuperNet->IsNode(neighbor)){
				SuperNet->AddNode(neighbor);
			}
			if(! SuperNet->IsEdge(i, neighbor)){
				SuperNet->AddEdge(i, neighbor);
				SuperNet->SetEDat(i, neighbor, weight);
			}
		}
		if(i % 100 == 0){
			printf("\rBuilding process: %2lf%%",100*(double)i/(double)weightVec.size());
    		fflush(stdout);
		}
		std::string name = "super_graph";
		writeOutGraph(GraphFolder,name , true, SuperNet);
	}
}