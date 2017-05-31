#include "stdafx.h"

#include "n2v.h"
#include <vector>

#include <sstream>
#include <iostream>
#include <iostream>
#include <utility>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <fstream>

#include "deeputils.h"
#include "GetRawCommunities.h"
#include "BuildSmallAndBigGraph.h"
#include "GetCommunitiesByMerge.h"
#include "linearinterpolation.h"
#include "BuildSmallAndBigGraph.h"

#ifdef USE_OPENMP
#include <omp.h>
#endif

const double DISCARD_QUANTILE = 0.3;

int main(int argc, char* argv[]) {
  
  TStr InFile,OutFile, StatsFile;
  int Dimensions, SuperDimensions, WalkLen, NumWalks, WinSize, Iter, Option, CommunityDetectionOption;
  double ParamP, ParamQ, MergeThreshold;
  bool Directed, Weighted, Verbose, Smart, Debug;

  double UpdateRateThreshold;
  int NumCommunities;
  TStr GraphFolder;

  ParseArgs(argc, argv, InFile, OutFile, StatsFile, GraphFolder, Dimensions, SuperDimensions, WalkLen, NumWalks, WinSize,
   Iter, NumCommunities, Option, Verbose, ParamP, ParamQ, UpdateRateThreshold, Directed, Weighted, CommunityDetectionOption, 
   MergeThreshold, Smart, Debug);


  //need a std::string as function for write to disk
  std::string NewGraphFolder = GraphFolder.GetCStr();

  std::clock_t begin = std::clock();
  PWNet InNet = PWNet::New();
  printf("Begin loading graph...\n");
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  printf("End loading graph\n");

  /**************************/
  if (Option == 0) {
    TIntFltVH EmbeddingsHV;
    node2vec(InNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, Verbose, 
    EmbeddingsHV);
    TStr OutOrigin("./embeddings/Origin");
    WriteOutput(OutOrigin, EmbeddingsHV);
      //End of node2vec, timing
    std::clock_t end = std::clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::ofstream StatsStreamOrigin;
    StatsStreamOrigin.open("./stats/OriginStats.txt");
    StatsStreamOrigin << elapsed_secs << "\n";
    return 0;
  } 
  /**************************/

  printf("Num of Nodes: %d\n", InNet->GetNodes());

  InNet = TSnap::GetMxScc(InNet);
  printf("Num of Nodes in MxScc: %d\n", InNet->GetNodes());
  
  IAssert(TSnap::IsWeaklyConn(InNet));
  
  printf("Begin finding raw communities using BFS\n");
  std::vector<std::vector<int> > C2N;
  THash<TInt, TInt> N2C;

  if(Smart){
    printf("Use Smart!!!!!!!!!!!!!!!!!!!\n");
    GetRawCommunities(InNet, C2N, N2C, UpdateRateThreshold, NumCommunities);
  }else{
    printf("Use Random!!!!!!!!!!!!!!!!!!!\n");
    GetRawCommunitiesByRandom(InNet, C2N, N2C, UpdateRateThreshold, NumCommunities);
  }
  printf("Get %d raw communities.\n", C2N.size());


  InNet.Clr();
  InNet = PWNet::New();
  printf("Begin loading graph...\n");
  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);
  printf("End loading graph\n");

  InNet = TSnap::GetMxScc(InNet);
  printf("Num of Nodes in MxScc: %d\n", InNet->GetNodes());

  std::vector<std::vector<int> > NewC2N;

  // Update C2N and N2C, such that number of communities == NumCommunities.
  GetCommunitiesByMerge(InNet, C2N, NewC2N, N2C, NumCommunities);

  printf("Number of communities in Original %d\n", C2N.size());
  printf("Number of communities in New %d\n", NewC2N.size());

  // Output community size before merge
  OutputNodeDistribution("./stats/size_distribution_before.txt", "Before Merge", C2N);
  // Output community size after merge
  OutputNodeDistribution("./stats/size_distribution_after.txt", "After Merge", NewC2N);

  PWNet SuperNet = PWNet::New();
  TVec<PWNet> NetVector;
  bool BuildSmallGraphNow = true;
  BuildSmallAndBigGraphToMemory(InNet, NewC2N, N2C, NetVector, SuperNet, BuildSmallGraphNow);
  // bool BuildSmallGraphNow = false;
  // std::vector<int> SizeVec;
  // for(int i = 0; i < NewC2N.size(); i++){
  //   SizeVec.push_back(NewC2N[i].size());
  // }
  // BuildSmallAndBigGraphToMemory(InNet, NewC2N, N2C, NetVector, SuperNet, BuildSmallGraphNow);
  
  // printf("there are %d nodes \n", SuperNet->GetNodes());
  //   for(TWNet::TNodeI NI = SuperNet->BegNI(); NI < SuperNet -> EndNI(); NI++){
  //     printf("node degree is : %d \n", NI.GetDeg());
  //   }
  
  // std::nth_element(SizeVec.begin(), SizeVec.begin()+(int)(SizeVec.size()*MergeThreshold), SizeVec.end());

  // int threshold = SizeVec[(int)(SizeVec.size()*MergeThreshold)];
  // printf("merge threshold is %d\n", threshold);
  // MergeSmallSuperNodes(InNet,NewC2N, N2C,NetVector,SuperNet,threshold,CommunityDetectionOption);

  // BuildSmallAndBigGraphToDisk(InNet, NewC2N, N2C, NewGraphFolder);

  //End of partition, timing
  std::clock_t end = std::clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  std::ofstream StatsStream;
  StatsStream.open(StatsFile.CStr());
  StatsStream << elapsed_secs << "\n";
  
  //LearnAndWriteOutputEmbeddingForAll(OutFile, StatsStream, SuperNet, NetVector, ParamP, ParamQ, Dimensions, 
  //  WalkLen, NumWalks, WinSize, Iter, Verbose);

  THashSet<TInt> LearnComMarker;
  THashSet<TInt> InterpNodeMarker;

  printf("______________________________\n");
  printf("Decide which group to learn\n");

  begin = std::clock();
  int SizeThreshold = GetLastSizeForQuantile(NewC2N, DISCARD_QUANTILE);
  printf("The threshold is %d\n", SizeThreshold);
  LearnOrInterp(NewC2N, LearnComMarker, InterpNodeMarker, SizeThreshold);
  end = std::clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  printf("______________________________\n");
  printf("Learn local embeddings for selected groups\n");
  TIntFltVH SelectedEmbedding;
  std::stringstream TimeSStream;

  LearnEmbeddingForSelected(LearnComMarker, SelectedEmbedding, NetVector, TimeSStream,
  ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, Verbose);


  begin = std::clock();
  printf("______________________________\n");
  printf("Do linearinterpolation\n");
  TIntFltVH LocalEmbeddingsHVForAll(SelectedEmbedding);
  int TotalRound = 10;
  LevelWiseSpread(InNet, SelectedEmbedding, LocalEmbeddingsHVForAll, Dimensions);

  printf("______________________________\n");
  printf("Add global embeddings and output all \n");
  TIntFltVH FinalEmbedding;

  ConcatenateDirect(OutFile, LocalEmbeddingsHVForAll, SuperNet,
  NewC2N, ParamP, ParamQ, SuperDimensions, WalkLen, NumWalks, WinSize, Iter, Verbose); 

  end = std::clock();
  elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
  StatsStream << elapsed_secs << "\n";
  StatsStream << TimeSStream.str();

  return 0;
}

