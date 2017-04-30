#ifndef BUILDSMALLANDBIGGRAPH_H
#define BUILDSMALLANDBIGGRAPH_H

void computeWeight(PWNet & InNet, int & nodeId, THash<TInt, TInt> & N2C, 
	int & outEdgeCount, int & inEdgeCount, 
	int & clusterId,std::vector<THash<TInt, TInt> > & weightVec,
	PWNet & smallNet, bool & BuildSmallGraphNow);

void SuperGraphConsturction(PWNet & SuperNet, std::vector<int> & inEdgeCounts, std::vector<THash<TInt, TInt> > outEdgeMaps);

void BuildSmallAndBigGraphToMemory(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PWNet> & NetVector, PWNet & SuperNet, bool & BuildSmallGraphNow);

void BuildSmallAndBigGraphToDisk(PWNet & InNet,std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, std::string & GraphFolder);

void writeOutGraph(std::string & GraphFolder, std::string & name,PUNGraph & Net);


void MergeSmallSuperNodes(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PWNet> & NetVector, PWNet & SuperNet, int & threshold, int & option);

void communityDetect(int & option, TCnComV & CmtyV, PUNGraph & Graph, double & Q);
// void BuildSuperGraphToMemory(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	// THash<TInt, TInt> & N2C, PWNet & SuperNet, std::vector<int> & inEdgeCounts, std::vector<THash<TInt, TInt> > outEdgeMaps);

#endif //BUILDSMALLANDBIGGRAPH_H