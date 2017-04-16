#include "stdafx.h"

#include "n2v.h"

#include "deeputils.h"
#include "recoveredges.h"
#include "linearinterpolation.h"
#include "samplen2v.h"


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

  THashSet<TInt> RepresentativeNodes;
  TInt NumRepNodes =  InNet->GetNodes() / ShrinkFactor;
  SelectRepresentativeNodes(InNet, RepresentativeNodes, NumRepNodes, ParamP, ParamQ, 
  Dimensions, WalkLen, NumWalks, Iter, Verbose);
  TIntFltVH EmbeddingsHVForSample;
  /*
    TO DO:  Learn Embedding for sample node
    Option 1 - Consturct graph of representative nodes and run node2vec on top of it
    Option 2 - Ran random walk on representative nodes and use face node id to replace non-representative nodes
               found in random walk. (potential improvement: modify word2vec to emit pairs which contain the fake id)

  */

  //Option 1: 
  // PWNet SampleNet = PWNet::New();
  // RecoverEdges(InNet, SampleNet, RepresentativeNodes, 
  //   ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);
  // node2vec(SampleNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
  //  Verbose, EmbeddingsHVForSample);

  // //Interpolation
  // TIntFltVH EmbeddingsHVForAll = TIntFltVH(EmbeddingsHVForSample);
  // int TotalRound = 5;
  // LinearInterpolation(InNet, EmbeddingsHVForSample, EmbeddingsHVForAll, TotalRound,
  //   ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);
  // WriteOutput(OutFile, EmbeddingsHVForAll);
  


    
  // Option 2:
  samplenode2vec(InNet, RepresentativeNodes, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
    Verbose, EmbeddingsHVForSample);

  //Interpolation
  TIntFltVH EmbeddingsHVForAll = TIntFltVH(EmbeddingsHVForSample);
  int TotalRound = 5;
  LinearInterpolation(InNet, EmbeddingsHVForSample, EmbeddingsHVForAll, TotalRound,
    ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);
  WriteOutput(OutFile, EmbeddingsHVForAll);

  return 0;
}

