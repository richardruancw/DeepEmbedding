#include "stdafx.h"

#include "n2v.h"
#include <vector>

#include "deeputils.h"
#include "BuildSmallAndBigGraph.h"
#include "GetCommunitiesByMerge.h"
#include "BuildSmallAndBigGraph.h"

#ifdef USE_OPENMP
#include <omp.h>
#endif



int main(int argc, char* argv[]) {
  // File names
  TStr InFile,OutFile;
  int Dimensions, WalkLen, NumWalks, WinSize, Iter, Option;
  double ParamP, ParamQ, ShrinkFactor;
  bool Directed, Weighted, Verbose;

  double UpdateRateThreshold;
  int NumCommunities;
  TStr GraphFolder;

  ParseArgs(argc, argv, InFile, OutFile, GraphFolder, Option, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, NumCommunities, ShrinkFactor, Verbose, ParamP, ParamQ, UpdateRateThreshold, Directed, Weighted);


  PWNet InNet = PWNet::New();
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);

  std::vector<std::vector<int> > C2N;
  THash<TInt, TInt> N2C;

  GetRowCommunities(InNet, C2N, N2C, UpdateRateThreshold, NumCommunities);


  InNet.Clr();
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);

  // Update C2N and N2C, such that number of communities == NumCommunities.
  GetCommunitiesByMerge(InNet, C2N, N2C)


  PWNet SuperNet = PWNet::New();
  TVec<PWNet> NetVector;
  BuildSmallAndBigGraphToMemory(InNet, C2N, N2C, NetVector, SuperNet);
  // Or
  BuildSmallAndBigGraphToDisk(InNet, C2N, N2C, GraphFolder);

  return 0;
}

