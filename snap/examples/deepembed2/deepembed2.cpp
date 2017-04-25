#include "stdafx.h"

#include "n2v.h"
#include <vector>

#include <iostream>
#include <utility>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>

#include "deeputils.h"
#include "GetRawCommunities.h"
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

  ParseArgs(argc, argv, InFile, OutFile, GraphFolder, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, NumCommunities, ShrinkFactor, Verbose, ParamP, ParamQ, UpdateRateThreshold, Directed, Weighted);

  //need a std::string as function for write to disk
  std::string NewGraphFolder = GraphFolder.GetCStr();

  PWNet InNet = PWNet::New();
  printf("Begin loading\n");
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  printf("End loading\n");

  printf("Num of Nodes: %d\n", InNet->GetNodes());
  
  printf("Begin finding raw communities using BFS\n");
  std::vector<std::vector<int> > C2N;
  THash<TInt, TInt> N2C;

  GetRawCommunities(InNet, C2N, N2C, UpdateRateThreshold, NumCommunities);
  printf("Get %d raw communities.\n", C2N.size());
  std::vector<int> s;
  int ss = 0;
  for(int i = 0; i < C2N.size(); i++){
    ss += C2N[i].size();
    s.push_back(C2N[i].size());
  }
  printf("Total covered Nodes: %d\n", ss);
  printf("The sizes of every communities: \n");
  std::sort(s.begin(), s.end());
  for(int i = s.size() - 1; i >= 0; i--){
    printf("%d ", s[i]);
  }
  printf("\n");


  InNet.Clr();
  InNet = PWNet::New();
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  std::vector<std::vector<int> > NewC2N;
  // Update C2N and N2C, such that number of communities == NumCommunities.
  int testNum = 5;
  GetCommunitiesByMerge(InNet, C2N, NewC2N, N2C, testNum);

  printf("Number of communities in Original %d\n", C2N.size());
  printf("Number of communities in New %d\n", NewC2N.size());


  PWNet SuperNet = PWNet::New();
  TVec<PWNet> NetVector;
  // BuildSmallAndBigGraphToMemory(InNet, NewC2N, N2C, NetVector, SuperNet);
  // Or

  std::cout<<NewGraphFolder<<std::endl;
  BuildSmallAndBigGraphToDisk(InNet, NewC2N, N2C, NewGraphFolder);
  
  return 0;
}

