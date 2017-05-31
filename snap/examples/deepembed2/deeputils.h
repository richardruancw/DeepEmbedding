#ifndef DEEPUTILIS_H
#define DEEPUTILIS_H

#include <vector>
#include <string>

/* Parse the input from command line
 */
void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile, TStr& StatsFile, TStr& GraphFolder,
 int& Dimensions, int& SuperDimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, int& NumCommunities, int& Option,
 bool& Verbose, double& ParamP, double& ParamQ, double& UpdateRateThreshold, bool& Directed, bool& Weighted, 
 int & CommunityDetectionOption, double & MergeThreshold, bool& Smart, bool& Debug);



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


int GetLastSizeForQuantile(const std::vector<std::vector<int> >& N2C, double Quantile);


void ComputeMetricsForNodes(const PWNet& InNet, const TVVec<TInt, int64>& WalksVV, TIntFltH& MetricCounter);


void SelectRepresentativeNodes(PWNet& InNet, THashSet<TInt>& RepresentativeNodes, TInt NodeNum, 
  double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);


void LearnAndWriteOutputEmbeddingForAll(TStr& OutFile, std::ofstream& StatsStream, PWNet& SuperNet, TVec<PWNet>& NetVector,
  double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose);


void OutputNodeDistribution(std::string OutFile, std::string Comment, std::vector<std::vector<int> >& N2C);


void LearnEmbeddingForSelected(THashSet<TInt>& SelectedGroup, TIntFltVH& SelectedEmbedding, TVec<PWNet>& NetVector, std::stringstream& StatsStream,
  double& ParamP, double& ParamQ, int& SmallDimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose);


void ConcatenateGlobalEmbedding(TIntFltVH& FinalEmbedding, TIntFltVH& LocalEmbedding, PWNet& SuperNet,
  std::vector<std::vector<int> > NewC2N, double& ParamP, double& ParamQ, int& SuperDimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose); 

void ConcatenateDirect(TStr& OutFile, TIntFltVH& LocalEmbedding, PWNet& SuperNet,
  std::vector<std::vector<int> > NewC2N, double& ParamP, double& ParamQ, int& SuperDimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose);

#endif //DEEPUTILIS_H
