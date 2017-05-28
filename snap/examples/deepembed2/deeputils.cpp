#include "stdafx.h"
#include "n2v.h"
#include "deeputils.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <utility>

#include <vector>
#include <algorithm>


void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile, TStr& StatsFile, TStr& GraphFolder,
 int& Dimensions, int& SuperDimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, int& NumCommunities, int& Option,
 bool& Verbose, double& ParamP, double& ParamQ, double& UpdateRateThreshold, bool& Directed, bool& Weighted, 
 int & CommunityDetectionOption, double & MergeThreshold, bool& Smart, bool& Debug) {


  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nAn algorithmic framework for representational learning on graphs."));
  InFile = Env.GetIfArgPrefixStr("-i:", "graph/karate.edgelist",
   "Input graph path");
  OutFile = Env.GetIfArgPrefixStr("-o:", "emb/karate.emb",
   "Output graph path");
  StatsFile = Env.GetIfArgPrefixStr("-stats:", "stats/stats.txt",
   "Time statistics path");

  GraphFolder = Env.GetIfArgPrefixStr("-out:", "",
   "Output community graph folder");
  Dimensions = Env.GetIfArgPrefixInt("-d:", 128,
   "Number of dimensions for small community. Default is 128");
  SuperDimensions = Env.GetIfArgPrefixInt("-sd:", Dimensions,
   "Number of dimensions for supper graph. Default is same with small community");
  WalkLen = Env.GetIfArgPrefixInt("-l:", 80,
   "Length of walk per source. Default is 80");
  NumWalks = Env.GetIfArgPrefixInt("-r:", 10,
   "Number of walks per source. Default is 10");
  WinSize = Env.GetIfArgPrefixInt("-k:", 10,
   "Context size for optimization. Default is 10");
  Option = Env.GetIfArgPrefixFlt("-ours:", 1,
   "Learning method. Default is 1, i.e, our's method.");
  Iter = Env.GetIfArgPrefixInt("-e:", 1,
   "Number of epochs in SGD. Default is 1");
  ParamP = Env.GetIfArgPrefixFlt("-p:", 1,
   "Return hyperparameter. Default is 1");
  ParamQ = Env.GetIfArgPrefixFlt("-q:", 1,
   "Inout hyperparameter. Default is 1");

  NumCommunities = Env.GetIfArgPrefixInt("-nc:", 10000,
   "Number of small communities. Default is 10000");
  UpdateRateThreshold = Env.GetIfArgPrefixFlt("-ut:", 0.3,
   "Update rate threshold when grow raw communities. Default is 0.3");

  CommunityDetectionOption = Env.GetIfArgPrefixFlt("-cdo:", 2, 
    "Community detection algorithm to use on super graph: 1 is Girvan-Newman,2 is Clauset-Newman-Moore, 3 is Infomap. Default is Clauset-Newman-Moore");

  MergeThreshold = Env.GetIfArgPrefixFlt("-mt:", 0.4, 
    "Quantile of cluster sizes under which the cluster should be merged by community detection method. Default is 0.4");


  Verbose = Env.IsArgStr("-v", "Verbose output.");
  Directed = Env.IsArgStr("-dr", "Graph is directed.");
  Weighted = Env.IsArgStr("-w", "Graph is weighted.");
  Smart = Env.IsArgStr("-ss", "Get raw community starting from lowest degree instead of random");
  Debug = Env.IsArgStr("-debug", "In Debug Mode");
}

void ReadGraph(TStr& InFile, bool& Directed, bool& Weighted, bool& Verbose, PWNet& InNet) {
  TFIn FIn(InFile);
  int64 LineCnt = 0;
  try {
    while (!FIn.Eof()) {
      TStr Ln;
      FIn.GetNextLn(Ln);
      TStr Line, Comment;
      Ln.SplitOnCh(Line,'#',Comment);
      TStrV Tokens;
      Line.SplitOnWs(Tokens);
      if(Tokens.Len()<2){ continue; }
      int64 SrcNId = Tokens[0].GetInt();
      int64 DstNId = Tokens[1].GetInt();
      double Weight = 1.0;
      if (Weighted) { Weight = Tokens[2].GetFlt(); }
      if (!InNet->IsNode(SrcNId)){ InNet->AddNode(SrcNId); }
      if (!InNet->IsNode(DstNId)){ InNet->AddNode(DstNId); }
      InNet->AddEdge(SrcNId,DstNId,Weight);
      if (!Directed){ InNet->AddEdge(DstNId,SrcNId,Weight); }
      LineCnt++;
    }
    if (Verbose) { printf("Read %lld lines from %s\n", (long long)LineCnt, InFile.CStr()); }
  } catch (PExcept Except) {
    if (Verbose) {
      printf("Read %lld lines from %s, then %s\n", (long long)LineCnt, InFile.CStr(),
       Except->GetStr().CStr());
    }
  }
}

void WriteOutput(TStr& OutFile, TIntFltVH& EmbeddingsHV) {
  TFOut FOut(OutFile);
  bool First = 1;
  for (int i = EmbeddingsHV.FFirstKeyId(); EmbeddingsHV.FNextKeyId(i);) {
    if (First) {
      FOut.PutInt(EmbeddingsHV.Len());
      FOut.PutCh(' ');
      FOut.PutInt(EmbeddingsHV[i].Len());
      FOut.PutLn();
      First = 0;
    }
    FOut.PutInt(EmbeddingsHV.GetKey(i));
    for (int64 j = 0; j < EmbeddingsHV[i].Len(); j++) {
      FOut.PutCh(' ');
      FOut.PutFlt(EmbeddingsHV[i][j]);
    }
    FOut.PutLn();
  }
}


void GetRandomWalks(PWNet& InNet, TVVec<TInt, int64>& WalksVV, TIntV& NIdsV, double& ParamP, double& ParamQ,
  int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose) {
  int64 AllWalks = WalksVV.GetXDim();
  TRnd Rnd(time(NULL));
  int64 WalksDone = 0;
  for (int64 i = 0; i < NumWalks; i++) {
    NIdsV.Shuffle(Rnd);
#pragma omp parallel for schedule(dynamic)
    for (int64 j = 0; j < NIdsV.Len(); j++) {
      if ( Verbose && WalksDone%10000 == 0 ) {
        printf("\rWalking Progress: %.2lf%%",(double)WalksDone*100/(double)AllWalks);fflush(stdout);
      }
      TIntV WalkV;
      SimulateWalk(InNet, NIdsV[j], WalkLen, Rnd, WalkV);
      for (int64 k = 0; k < WalkV.Len(); k++) { 
        WalksVV.PutXY(i*NIdsV.Len()+j, k, WalkV[k]);
      }
      WalksDone++;
    }
  }
  if (Verbose) {
    printf("\n");
    fflush(stdout);
  }
}

void ComputeMetricsForNodes(const PWNet& InNet, const TVVec<TInt, int64>& WalksVV, TIntFltH& MetricCounter) {
  for (int64 i = 0; i < WalksVV.GetXDim(); i++) {
    for (int64 j = 0; j < WalksVV.GetYDim(); j++) {
      if ( MetricCounter.IsKey(WalksVV(i, j)) ) {
        MetricCounter.GetDat(WalksVV(i, j))+= 1;
      } else {
        MetricCounter.AddDat(WalksVV(i, j), 0.0);
      }
    }
  }
  for (TIntFltH::TIter ThashIter = MetricCounter.BegI(); ThashIter < MetricCounter.EndI(); ThashIter++) {
    (*ThashIter).Dat /= InNet->GetNI((*ThashIter).Key).GetDeg();
  }
}


void SelectRepresentativeNodes(PWNet& InNet, THashSet<TInt>& RepresentativeNodes, TInt NodeNum, 
  double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose) {

  TIntV NIdsV;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    NIdsV.Add(NI.GetId());
  }

  int64 AllWalks = (int64)NumWalks * NIdsV.Len();
  TVVec<TInt, int64> WalksVV(AllWalks,WalkLen);
  GetRandomWalks(InNet, WalksVV, NIdsV, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);

  TIntFltH MetricCounter;
  ComputeMetricsForNodes(InNet, WalksVV, MetricCounter);

  std::vector<TFlt> MetricVector;
  for(TIntFltH::TIter ThashIter = MetricCounter.BegI(); ThashIter < MetricCounter.EndI(); ThashIter++) {
    MetricVector.push_back((*ThashIter).Dat);
  }

  // Only select top NodeNum of nodes as representative nodes
  std::nth_element(MetricVector.begin(), MetricVector.begin() + (NIdsV.Len() - NodeNum), MetricVector.end());
  TFlt MetricThreshold = MetricVector[(NIdsV.Len() - NodeNum)];

  for(TIntFltH::TIter ThashIter = MetricCounter.BegI(); ThashIter < MetricCounter.EndI(); ThashIter++) {
    if((*ThashIter).Dat < MetricThreshold) {continue;}
    RepresentativeNodes.AddKey((*ThashIter).Key);
  }

  printf("Number of selected nodes: %d\n", RepresentativeNodes.Len());

}


void LearnAndWriteOutputEmbeddingForAll(TStr& OutFile, std::ofstream& StatsStream, PWNet& SuperNet, TVec<PWNet>& NetVector,
  double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose) {
    //Super-graph node2vec
  std::clock_t begin = std::clock();
  TIntFltVH EmbeddingsHVSuperNet;
  node2vec(SuperNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, Verbose, 
    EmbeddingsHVSuperNet);
  std::clock_t end = std::clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  StatsStream << elapsed_secs << "\n";

  //Small-graph node2vec
  printf("Start learning for small net");
  TFOut FOut(OutFile);
  FOut.PutCh(' ');
  FOut.PutLn();
  for (int i = 0; i < NetVector.Len(); i++) {
    begin = std::clock();
    printf("This for the %d cluster\n", i);
    TIntFltVH EmbeddingsHVSmallNet;
    PWNet CurrSmallNet = NetVector[i];
    node2vec(CurrSmallNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, Verbose, 
    EmbeddingsHVSmallNet);
    for (TIntFltVH::TIter iter = EmbeddingsHVSmallNet.BegI(); iter < EmbeddingsHVSmallNet.EndI(); iter++) {
        FOut.PutInt(iter->Key);
        FOut.PutCh(' ');
        for (int j = 0; j < (iter->Dat).Len(); j++) {
          FOut.PutFlt((iter->Dat)[j]);
          FOut.PutCh(' ');
        }
        
        for (int j = 0; j < EmbeddingsHVSuperNet.GetDat(i).Len(); j++) {
          FOut.PutFlt(EmbeddingsHVSuperNet.GetDat(i)[j]);
          FOut.PutCh(' ');
        }
        
        FOut.PutLn();
    }
    CurrSmallNet.Clr();
    end = std::clock();
    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    StatsStream << elapsed_secs << "\t";
  }
}

void OutputNodeDistribution(std::string OutFile, std::string Comment, std::vector<std::vector<int> >& N2C) {
  std::ofstream StatsStream;
  StatsStream.open(OutFile.c_str());
  StatsStream << "---Output Size of Each Communities---";
  StatsStream << "---" << Comment << "---" << std::endl;
  for (int i = 0; i < N2C.size(); i++) {
 		StatsStream << N2C[i].size() << " ";
  }
  StatsStream << std::endl;
}














