#ifndef BUILDSMALLANDBIGGRAPH_H
#define BUILDSMALLANDBIGGRAPH_H

void computeWeight(PWNet & InNet, int & nodeId, THash<TInt, TInt> & N2C, 
	int & outEdgeCount, int & inEdgeCount, int & clusterId);

void BuildSmallAndBigGraphToMemory(PWNet & InNet, std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, TVec<PWNet> & NetVector, PWNet & SuperNet);

void BuildSmallAndBigGraphToDisk(PWNet & InNet,std::vector< std::vector<int> > & C2N, 
	THash<TInt, TInt> & N2C, std::string & GraphFolder);

void writeOutGraph(std::string & GraphFolder, std::string & name, bool weighted, PWNet & Net);

#endif //BUILDSMALLANDBIGGRAPH_H