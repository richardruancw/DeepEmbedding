#ifndef DEEPUTILIS_H
#define DEEPUTILIS_H


/* Parse the input from command line
 */
void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile, int& Option,
 int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, double& ShrinkFactor,
 bool& Verbose, double& ParamP, double& ParamQ, bool& Directed, bool& Weighted);

/* Read the graph from edgelist file
 * @pre InNet = InNet.clear()
 *
 * @parama InFile path of the input edgelist file
 * @parama Directed whether the input edgelist is of a directed or undirected graph
 * @parama Weighted whether the edgelist is of a weighted graph
 * @parama Verbose control the verbose level
 * @parama InNet a directed graph with data in edges and nodes
 * 
 */
void ReadGraph(TStr& InFile, bool& Directed, bool& Weighted, bool& Verbose, PWNet& InNet);

/* Generate the random walk matrixs
 * @pre Has ran PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose) for @a InNet
 * @parama
 * @parama
 * @parama
 * @parama
 * @parama
 */
void GetRandomWalks(PWNet& InNet, TVVec<TInt, int64>& WalksVV, TIntV& NIdsV,double& ParamP, double& ParamQ, 
  int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);


void WriteOutput(TStr& OutFile, TIntFltVH& EmbeddingsHV);



void ComputeMetricsForNodes(const PWNet& InNet, const TVVec<TInt, int64>& WalksVV, TIntFltH& MetricCounter);

void SelectRepresentativeNodes(PWNet& InNet, THashSet<TInt>& RepresentativeNodes, TInt NodeNum, 
  double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);

#endif //DEEPUTILIS_H
