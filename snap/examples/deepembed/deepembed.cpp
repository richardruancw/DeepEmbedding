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
  int Dimensions, WalkLen, NumWalks, WinSize, Iter, Option;
  double ParamP, ParamQ, ShrinkFactor;
  bool Directed, Weighted, Verbose;
  ParseArgs(argc, argv, InFile, OutFile, Option, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, ShrinkFactor, Verbose, ParamP, ParamQ, Directed, Weighted);

  PWNet InNet = PWNet::New();
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);

  if (Option == 1) {
      printf("\n Start running recover edges version !!\n");
      // Prepare the graph for random walk
      PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);
      THashSet<TInt> RepresentativeNodes;
      TInt NumRepNodes =  (int) (InNet->GetNodes() * 1.0) / ShrinkFactor;
      SelectRepresentativeNodes(InNet, RepresentativeNodes, NumRepNodes, ParamP, ParamQ, 
      Dimensions, WalkLen, NumWalks, Iter, Verbose);
      TIntFltVH EmbeddingsHVForSample;

      PWNet SampleNet = PWNet::New();
      RecoverEdges(InNet, SampleNet, RepresentativeNodes, 
      ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);
      node2vec(SampleNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
      Verbose, EmbeddingsHVForSample);

      //Interpolation
      TIntFltVH EmbeddingsHVForAll = TIntFltVH(EmbeddingsHVForSample);
      int TotalRound = 5;
      LinearInterpolation(InNet, EmbeddingsHVForSample, EmbeddingsHVForAll, TotalRound,
      ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, WinSize, Verbose);
      WriteOutput(OutFile, EmbeddingsHVForAll);
    } else if(Option == 2) {
      printf("\n Start running direct sampling version !!\n");
      // Prepare the graph for random walk
      PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);
      THashSet<TInt> RepresentativeNodes;
      TInt NumRepNodes =  (int) (InNet->GetNodes() * 1.0) / ShrinkFactor;
      SelectRepresentativeNodes(InNet, RepresentativeNodes, NumRepNodes, ParamP, ParamQ, 
      Dimensions, WalkLen, NumWalks, Iter, Verbose);
      TIntFltVH EmbeddingsHVForSample;

      samplenode2vec(InNet, RepresentativeNodes, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
      Verbose, EmbeddingsHVForSample);
      //Interpolation
      TIntFltVH EmbeddingsHVForAll = TIntFltVH(EmbeddingsHVForSample);
      int TotalRound = 5;
      LinearInterpolation(InNet, EmbeddingsHVForSample, EmbeddingsHVForAll, TotalRound,
      ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, WinSize, Verbose);
      WriteOutput(OutFile, EmbeddingsHVForAll);
    } else {
      printf("\n Start running original node2vec !!\n");
      TIntFltVH EmbeddingsHV;
      node2vec(InNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
        Verbose, EmbeddingsHV);
      TStr OriginName("Origin");
      WriteOutput(OriginName, EmbeddingsHV);
    }
  return 0;
}

