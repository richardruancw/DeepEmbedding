#include "stdafx.h"

#include "n2v.h"

#include "deeputils.h"
#include "recoveredges.h"
#include "linearinterpolation.h"

#ifdef USE_OPENMP
#include <omp.h>
#endif

int main(int argc, char* argv[]) {
  // File names
  TStr InFile,OutFile;
  int Dimensions, WalkLen, NumWalks, WinSize, Iter, ShrinkFactor;
  double ParamP, ParamQ;
  bool Directed, Weighted, Verbose;
  ParseArgs(argc, argv, InFile, OutFile, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, ShrinkFactor, Verbose, ParamP, ParamQ, Directed, Weighted);


  PWNet InNet = PWNet::New();
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  // Prepare the graph for random walk
  PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);
  PWNet SampleNet = PWNet::New();

  THashSet<TInt> RepresentativeNodes;
  TInt NumRepNodes =  InNet->GetNodes() / ShrinkFactor;
  SelectRepresentativeNodes(InNet, RepresentativeNodes, NumRepNodes, ParamP, ParamQ, 
  Dimensions, WalkLen, NumWalks, Iter, Verbose);

  TIntFltVH EmbeddingsHVForSample;
  TIntFltVH EmbeddingsHVForAll;

  /* TO DO: 

  Bowen:
  RecoverEdges(InNet, SampleNet, ...);

  node2vec(InNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHVForSample);


  Yue:
  LinearInterpolation(SampleNet, EmbeddingsHVForAll, EmbeddingsHVForSample, ...;

  WriteOutput(OutFile, EmbeddingsHVForAll);
  */

  TestLINEARINTERPOLATION();
  TestRECOVEREDGES();

  return 0;
}

