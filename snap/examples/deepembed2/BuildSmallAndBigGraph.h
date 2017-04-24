#ifndef BUILDSMALLANDBIGGRAPH_H
#define BUILDSMALLANDBIGGRAPH_H

void computeWeight(PUNGraph & InNet, int & nodeId, THash<TInt, TInt> & N2C, 
	int & outEdgeCount, int & inEdgeCount, 
	int & clusterId,std::vector<THash<TInt, TInt> > & weightVec,
	PUNGraph & smallNet);

void BuildSmallAndBigGraphToMemory(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PUNGraph> & NetVector, PWNet & SuperNet);

void BuildSmallAndBigGraphToDisk(PWNet & InNet,std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, std::string & GraphFolder);

void writeOutGraph1(std::string & GraphFolder, std::string & name,PUNGraph & Net);
void writeOutGraph2(std::string & GraphFolder, std::string & name,PWNet & Net);

#endif //BUILDSMALLANDBIGGRAPH_H