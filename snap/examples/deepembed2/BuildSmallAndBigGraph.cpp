#include "stdafx.h"
#include <vector>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include "biasedrandomwalk.h"
#include "BuildSmallAndBigGraph.h"

void SuperGraphConsturction(PWNet & SuperNet, std::vector<int> & inEdgeCounts, std::vector<THash<TInt, TInt> > outEdgeMaps){
	printf("start building super graph!\n");
	//super graph construction
	for(int i = 0; i < outEdgeMaps.size(); i++){
		if(!SuperNet->IsNode(i)){
			SuperNet->AddNode(i);
		}
		for(THash<TInt, TInt>::TIter HashI = outEdgeMaps[i].BegI(); HashI < outEdgeMaps[i].EndI(); HashI++){
			int neighbor = (*HashI).Key;
			int interClusterEdgeCount = (*HashI).Dat;
			
			assert(i < neighbor);
			
			if(! SuperNet->IsNode(neighbor)){
				SuperNet->AddNode(neighbor);
			}
			if(! SuperNet->IsEdge(i, neighbor)){
				if((double)interClusterEdgeCount/(double)(std::min(inEdgeCounts[i], inEdgeCounts[neighbor])) > 0){
					SuperNet->AddEdge(i,neighbor, (double)interClusterEdgeCount/(double)(std::min(inEdgeCounts[i], inEdgeCounts[neighbor])));
					SuperNet->AddEdge(neighbor,i, (double)interClusterEdgeCount/(double)(std::min(inEdgeCounts[i], inEdgeCounts[neighbor])));
				}
			}
		}
		if(i % 1 == 0){
			printf("\rBuilding super graph process: %2lf%%",100*(double)(i+1)/(double)outEdgeMaps.size());
    		fflush(stdout);
		}
	}

	printf("\n");
}

void computeWeight(PWNet & InNet, int & nodeId, THash<TInt, TInt> & N2C, 
	int & outEdgeCount, int & inEdgeCount, 
	int & clusterId,std::vector<THash<TInt, TInt> > & weightVec,
	PWNet & smallNet, bool & BuildSmallGraphNow){

	//get current node
	TWNet::TNodeI currentNode = InNet->GetNI(nodeId);
	int weight12, weight21;
	//for all the neighbors of the node
	for(int m = 0; m < currentNode.GetDeg(); m++){
		//get the neighbor
		int targetId = currentNode.GetNbrNId(m);
		int targetClusterId = N2C(targetId);

		//if the neighbor also in current cluster, increment in-cluster-edge count
		if(targetClusterId == clusterId){
			inEdgeCount++;

			if(BuildSmallGraphNow){
				if(!smallNet->IsNode(nodeId)){
					smallNet->AddNode(nodeId);
				}	
				if(! smallNet->IsNode(targetId)){
					smallNet->AddNode(targetId);
				}
				if(! smallNet->IsEdge(nodeId, targetId)){

					weight12 = InNet->GetEDat(nodeId, targetId);
					weight21 = InNet->GetEDat(targetId, nodeId);
					
					smallNet->AddEdge(nodeId, targetId,weight12);
					smallNet->AddEdge(targetId, nodeId, weight21);
				}

				assert(smallNet->IsEdge(nodeId, targetId) == true);
			}
			
		}else{
/* else the neighbor belongs to other cluster, increment out-cluster-edge count, 
this is a bdy node*/
			outEdgeCount++;
//also update the super graph weight between current community and the other community
// we only consider weightVec[i] containing all the edges between i and n where n > i
			if(targetClusterId > clusterId){
				// if in hashtable, increment weight, else set weight to be 1 
				if(weightVec[clusterId].IsKey(targetClusterId)){
					weightVec[clusterId](targetClusterId)++;
				}else{
					weightVec[clusterId](targetClusterId) = 1;
				}
			}else{
//if clusterId > targetClusterId, this edge should be handled before we come to this step
				assert(weightVec[targetClusterId].IsKey(clusterId) == true);
			}
		}
		assert(outEdgeCount+inEdgeCount == currentNode.GetOutDeg());
	}
}

void BuildSmallAndBigGraphToMemory(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PWNet> & NetVector, PWNet & SuperNet, bool & BuildSmallGraphNow){
	
	std::vector<THash<TInt, TInt> > weightVec;
	if(BuildSmallGraphNow){
		printf("start building small graphs\n");
	}else{
		printf("start counting edges for super graph\n");
	}
	
	std::vector<int> inEdgeCounts;
	//for each commuinity
	// #pragma omp parallel for schedule(dynamic)
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
			computeWeight(InNet, C2N[i][j], N2C, outEdgeCount, inEdgeCount, i, weightVec, smallNet, BuildSmallGraphNow);
		}
		
		inEdgeCounts.push_back(inEdgeCount);
		
		if(BuildSmallGraphNow){
			NetVector.Add(smallNet);
			if(!TSnap::IsWeaklyConn<PWNet>(smallNet)){
				printf("not connected ! %d size %d \n", i, smallNet->GetNodes());
				printf("cluster size should be %d \n", C2N[i].size());
			}
			printf("\rBuilding small graph process: %2lf%%",100*(double)(i+1)/(double)C2N.size());
	    	fflush(stdout);
		}
	}
	printf("\n");
	
	if(BuildSmallGraphNow){
		printf("finish building small graphs, there are %d in total \n", NetVector.Len());
	}else{
		assert(NetVector.Len() == 0);
	}
	
	SuperGraphConsturction(SuperNet, inEdgeCounts, weightVec);
}

void writeOutGraph(std::string & GraphFolder, std::string & name,PWNet & Net){
	std::string filePath = GraphFolder+"/"+name;
	TStr NewStr(filePath.c_str());
	TFOut FOut(NewStr);

	for(TWNet::TEdgeI EI = Net->BegEI(); EI < Net -> EndEI(); EI++){
		FOut.PutInt(EI.GetSrcNId());
		FOut.PutCh(' ');
		FOut.PutInt(EI.GetDstNId());
		FOut.PutCh(' ');
		FOut.PutFlt(EI.GetDat());
		FOut.PutLn();
	}
}

void BuildSmallAndBigGraphToDisk(PWNet & InNet,std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, std::string & GraphFolder){

	// PUNGraph OriginNet = PUNGraph::New();
	// OriginNet = TSnap::ConvertGraph<PUNGraph, PWNet> (InNet);

	std::vector<THash<TInt, TInt> > weightVec;
	std::cout<<"start building small graphs"<<std::endl;
	PWNet SuperNet = PWNet::New();
	std::vector<int> inEdgeCounts;
	bool BuildSmallGraphNow = true;
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
			computeWeight(InNet, C2N[i][j], N2C, outEdgeCount, inEdgeCount, i, weightVec, smallNet, BuildSmallGraphNow);
		}

		inEdgeCounts.push_back(inEdgeCount);
		std::string name;
		std::ostringstream convert;
		convert << i;
		name = convert.str();
		writeOutGraph(GraphFolder, name, smallNet);

		printf("\rBuilding process: %2lf%%",100*(double)(i+1)/(double)C2N.size());
    	fflush(stdout);
	}
	printf("\n");
	std::cout<<"start building super graph! \n"<<std::endl;
	//super graph construction
	for(int i = 0; i < weightVec.size(); i++){
		if(!SuperNet->IsNode(i)){
			SuperNet->AddNode(i);
		}
		for(THash<TInt, TInt>::TIter HashI = weightVec[i].BegI(); HashI < weightVec[i].EndI(); HashI++){
			int neighbor = (*HashI).Key;
			int weight = (*HashI).Dat;
			assert(i < neighbor);
			if(! SuperNet->IsNode(neighbor)){
				SuperNet->AddNode(neighbor);
			}
			if(! SuperNet->IsEdge(i,neighbor)){
				SuperNet->AddEdge(i,neighbor,(double)weight/(double)(std::min(inEdgeCounts[i], inEdgeCounts[neighbor])));
				SuperNet->AddEdge(neighbor,i,(double)weight/(double)(std::min(inEdgeCounts[i], inEdgeCounts[neighbor])));
			}
		}
		if(i % 1 == 0){
			printf("\rBuilding process: %2lf%%",100*(double)(i+1)/(double)weightVec.size());
    		fflush(stdout);
		}
		std::string name = "super_graph";
		writeOutGraph(GraphFolder,name , SuperNet);
	}
}

bool CheckAdd(TCnCom & com, int & alreadyIn, int & wantCheck, PWNet & SuperNet, bool & shouldAdd){
	for(int j = 0; j < com.Len(); j++){
		if(! SuperNet->IsEdge(wantCheck, com[j]) || 
			SuperNet->GetEDat(wantCheck, com[j]) < 0.5*(SuperNet->GetEDat(wantCheck, alreadyIn))){
			shouldAdd = false;
			break;
		}
	}
}

void ConductanceMerge(PWNet & SuperNet, TCnComV & CmtyV, double & percent, int & maxRound){
	printf("use algo 4: ConductanceMerge !\n");
	std::vector<TWNet::TEdgeI> EdgeVec;
	double value = 0;
	printf("edge weights: \n");
	for(TWNet::TEdgeI EI = SuperNet->BegEI(); EI < SuperNet->EndEI(); EI++){
		EdgeVec.push_back(EI);
		printf("%f,",(double) SuperNet->GetEDat(EI.GetSrcNId(),EI.GetDstNId()));
		if(SuperNet->GetEDat(EI.GetSrcNId(),EI.GetDstNId()) > value){
			value = SuperNet->GetEDat(EI.GetSrcNId(),EI.GetDstNId());
		}
	}
	//map original community id to new community id
	THash<TInt, TInt> assigner;
	//map new community id to the connected component
	THash<TInt, TCnCom> organizer;
	int community = 0;
	//loop all the edges
	for(int i = 0; i < EdgeVec.size(); i++){
		int node1 = EdgeVec[i].GetSrcNId();
		int node2 = EdgeVec[i].GetDstNId();
		// if the conductance on current edge is large enough
		if(SuperNet->GetEDat(node1, node2) > (percent * value)){
			//if we did not encounter these two super nodes before
			if(!assigner.IsKey(node1) && !assigner.IsKey(node2)){
				// map two original communities to new community
				assigner.AddDat(node1, community);
				assigner.AddDat(node2, community);
				assert(organizer.IsKey(community) != true);
				//construct new connected component and add the original two super nodes to the new connnected component
				TCnCom com;
				com.Add(node1);
				com.Add(node2);
				// map new community id to the connected component
				organizer(community) = com;
				community++;
			}//if we saw node1 before but not node2
			else if(assigner.IsKey(node1) && !assigner.IsKey(node2)){
				bool shouldAdd = true;
				// get the connected component that contains node1
				TCnCom com = organizer(assigner(node1));
				//check if we should add super node2 to the connected component
				CheckAdd(com, node1, node2, SuperNet, shouldAdd);
				if(shouldAdd){
					assigner.AddDat(node2, assigner(node1));
					organizer(assigner(node1)).Add(node2);
				}

			}//if we saw node2 before but not node1
			else if(assigner.IsKey(node2) && !assigner.IsKey(node1)){
				bool shouldAdd = true;
				// get the connected component that contains node2
				TCnCom com = organizer(assigner(node2));
				//check if we should add super node1 to the connected component
				CheckAdd(com, node2, node1, SuperNet, shouldAdd);
				if(shouldAdd){
					assigner.AddDat(node1, assigner(node2));
					organizer(assigner(node2)).Add(node1);
				}
			}
		}
	}
	//put the newly formed connected component into CmtyV
	for(THash<TInt, TCnCom>::TIter HashI = organizer.BegI(); HashI < organizer.EndI(); HashI++){
		TCnCom com = (*HashI).Dat;
		CmtyV.Add(com);
	}
	//put other super nodes that have not been assigned to the CmtyV
	for(TWNet::TNodeI NI = SuperNet->BegNI(); NI < SuperNet->EndNI(); NI++){
		int id = NI.GetId();
		if(!assigner.IsKey(id)){
			TCnCom com;
			com.Add(id);
			CmtyV.Add(com);
		}
	}
	printf("original number of communities: %d\n", SuperNet->GetNodes());
	printf("number of communities: %d\n", CmtyV.Len());
}

void communityDetect(int & option, TCnComV & CmtyV, PUNGraph & Graph, double & Q){
	if(option == 1){
		printf("use algo 1: Girvan-Newman !\n");
		Q = TSnap::CommunityGirvanNewman(Graph, CmtyV);
	}else if(option == 2){
		printf("use algo 2:  Clauset-Newman-Moore!\n");
		Q = TSnap::CommunityCNM(Graph, CmtyV);
	}else{
		printf("use algo 3: Infomap !\n");
		Q = TSnap::Infomap(Graph, CmtyV);
	}
	printf("original number of communities: %d\n", Graph->GetNodes());
	printf("number of communities: %d\n", CmtyV.Len());
}

void MergeSmallSuperNodes(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PWNet> & NetVector, PWNet & SuperNet, int & threshold, int & option){

	std::vector< std::vector<int> > NewC2N;
	THash<TInt, TInt>  NewN2C;

	int counter = 0;
	//filter out all the super nodes whose community have more than threshold nodes in the original graph, delete them from SuperNet
	for(int i = 0; i < C2N.size(); i++){
		if(C2N[i].size() > threshold){
			std::vector<int> TempVec = C2N[i];			
			NewC2N.push_back(TempVec);
			SuperNet->DelNode(i);
			for(int j = 0; j < C2N[i].size(); j++){
				NewN2C(C2N[i][j]) = counter;
			}
			counter++;
		}
	}

	//run community detection algorithm on remaining graph
	TCnComV CmtyV;
	double Q = 0.0;

	PUNGraph SuperGraph = PUNGraph::New();
	SuperGraph = TSnap::ConvertGraph<PUNGraph, PWNet> (SuperNet);
	if(option == 4){
		int maxRound = 5;
		double percent = 0.5;
		ConductanceMerge(SuperNet, CmtyV, percent, maxRound);
	}else{
		communityDetect(option, CmtyV, SuperGraph, Q);
	}
	//update N2C, C2N, inEdgeCounts outEdgeMaps
	for (int c = 0; c < CmtyV.Len(); c++) {
	    std::vector<int> TempVec;

	    for (int i = 0; i < CmtyV[c].Len(); i++) {
	      int clusterId = CmtyV[c][i].Val;
	      int community = c;

	      printf("old cluster id %d, new cluster id %d \n", clusterId, counter);     
	      //put all the nodes of newly merged community into a vector to be the new community
	      for(int j = 0; j < C2N[clusterId].size(); j++){
	      	TempVec.push_back(C2N[clusterId][j]);
	      	NewN2C(C2N[clusterId][j]) = counter;
	      }
	    }

	    NewC2N.push_back(TempVec);
	    counter++;
  	}

	//reconstruct small graphs
	SuperNet->Clr();
	bool BuildSmallGraphNow = true;
	
	BuildSmallAndBigGraphToMemory(InNet, NewC2N, NewN2C, NetVector, SuperNet, BuildSmallGraphNow);
	printf("original number of partitions %d, new number of partitions %d \n", C2N.size(), NewC2N.size());

	printf("new partition sizes: \n");
	int OldSize = 0;
	int NewSize = 0;
	for(int i = 0; i < C2N.size(); i++){
		OldSize+=C2N[i].size();
	}
	for(int i = 0; i < NewC2N.size(); i++){
		printf("%d ", NewC2N[i].size());
		NewSize+=NewC2N[i].size();
	}
	assert(OldSize == NewSize);
	C2N = NewC2N;
	N2C = NewN2C;
}

// bool EdgeComparator(TWNet::TEdgeI & e1, TWNet::TEdgeI & e2){
// 	return (e1.GetDat() > e2.GetDat()); 
// }
void LearnOrInterp(std::vector< std::vector<int> > & C2N, THashSet<TInt> & LearnComMarker, 
	THashSet<TInt> & InterpNodeMarker, int & SizeThreshold){
	//take C2N, find all communities that are large enough to be run n2v on, 
	//mark all the nodes to be interpolated.
	for(int i = 0; i < C2N.size(); i++){
		
		if(C2N[i].size() >= SizeThreshold){
			LearnComMarker.AddKey(i);
		}else{
			for(int j = 0; j < C2N[i].size(); j++){
				InterpNodeMarker.AddKey(C2N[i][j]);
			}
		}
	}
}

bool PairComparator(std::pair<int, double> p1, std::pair<int, double> p2){
	return (p1.second < p2.second);
}

// delete the edges of some high-degree nodes
void DeleteTroubleMakers(PWNet & SuperNet){
	printf("every been here\n");
	for(TWNet::TNodeI NI = SuperNet->BegNI(); NI < SuperNet->EndNI(); NI++){
		int OriginalDeg = NI.GetOutDeg();
		
		if(OriginalDeg > 1000){
			printf("Begin deleting, original out degree is %d\n", OriginalDeg);
			std::vector<std::pair<int, double> > NbrAndWeightV;
	// get all the outNeighboring nodes and the weight (conductance) 
	// between the neighbor and current node
			for(int i = 0; i < OriginalDeg; i++){
				int NeighborId = NI.GetOutNId(i);
				double Conductance = SuperNet->GetEDat(NI.GetId(), NeighborId);
				NbrAndWeightV.push_back(std::pair<int, double>(NeighborId,Conductance));
			}
			// printf("get all the neibors of current node\n");
			assert(NbrAndWeightV.size() == OriginalDeg);
			//sort the out-neighbors by conductance
			// printf("sort the (neighbor, weight) pair\n");
			std::sort(NbrAndWeightV.begin(), NbrAndWeightV.end(), PairComparator);
			//delete out edges with small conductance
			// printf("delete edges with small weight(conductance)\n");
			for(int i = 0; i < NbrAndWeightV.size(); i++){
				int NeighborId = NbrAndWeightV[i].first;
				SuperNet->DelEdge(NI.GetId(), NeighborId);
				OriginalDeg--;
				if(OriginalDeg <= 1000){
					break;
				}
			}
			printf("finish deleting, new degree is %d\n", NI.GetOutDeg());
			assert(NI.GetOutDeg() <= 1000);
		}
	}
}