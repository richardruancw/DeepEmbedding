#ifndef DEEPUTILIS_H
#define DEEPUTILIS_H


void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile, int& Dimensions, int& WalkLen, 
	int& NumWalks, int& WinSize, int& Iter, bool& Verbose, double& ParamP, double& ParamQ, bool& Directed, 
	bool& Weighted);

void ReadGraph(TStr& InFile, bool& Directed, bool& Weighted, bool& Verbose, PWNet& InNet);


void GetRandomWalks(PWNet& InNet, TVVec<TInt, int64>& WalksVV, TIntV& NIdsV,double& ParamP, double& ParamQ, 
  int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);


void ComputeMetricsForNodes(const PWNet& InNet, const TVVec<TInt, int64>& WalksVV, TIntFltH MetricCounter);

#endif //DEEPUTILIS_H