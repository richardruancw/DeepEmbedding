#include "stdafx.h"

#include "n2v.h"

#include "deeputils.h"
#include "samplegraph.h"
#include "recoveredges.h"
#include "linearinterpolation.h"

#ifdef USE_OPENMP
#include <omp.h>
#endif

int main(int argc, char* argv[]) {
  // File names
  TStr InFile,OutFile;
  int Dimensions, WalkLen, NumWalks, WinSize, Iter;
  double ParamP, ParamQ;
  bool Directed, Weighted, Verbose;
  ParseArgs(argc, argv, InFile, OutFile, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, Verbose, ParamP, ParamQ, Directed, Weighted);


  PWNet InNet = PWNet::New();
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  // Prepare the graph for random walk
  PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);
  PWNet SampleNet = PWNet::New();
  BuildSampleGraph(InNet, SampleNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, 
   Verbose); // TO DO


  TIntFltVH EmbeddingsHVForSample;
  TIntFltVH EmbeddingsHVForAll;

  /* TO DO: 

  Bowen:
  RecoverEdges(InNet, SampleNet, ...);

  node2vec(InNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHVForSample);


  Yue:
  LinearInterpolation(InNet, EmbeddingsHVForAll, EmbeddingsHVForSample, ...;

  WriteOutput(OutFile, EmbeddingsHVForAll);
  */


  TestSAMPLEGRAPH();
  TestLINEARINTERPOLATION();
  TestRECOVEREDGES();

  return 0;
}

