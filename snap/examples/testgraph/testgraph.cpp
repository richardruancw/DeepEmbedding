#include "stdafx.h"

#include "n2v.h"

#ifdef USE_OPENMP
#include <omp.h>
#endif

void ParseArgs(int& argc, char* argv[], TStr& InFile, TStr& OutFile,
 int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter,
 bool& Verbose, double& ParamP, double& ParamQ, bool& Directed, bool& Weighted) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nAn algorithmic framework for representational learning on graphs."));
  InFile = Env.GetIfArgPrefixStr("-i:", "graph/karate.edgelist",
   "Input graph path");
  OutFile = Env.GetIfArgPrefixStr("-o:", "emb/karate.emb",
   "Output graph path");
  Dimensions = Env.GetIfArgPrefixInt("-d:", 128,
   "Number of dimensions. Default is 128");
  WalkLen = Env.GetIfArgPrefixInt("-l:", 80,
   "Length of walk per source. Default is 80");
  NumWalks = Env.GetIfArgPrefixInt("-r:", 10,
   "Number of walks per source. Default is 10");
  WinSize = Env.GetIfArgPrefixInt("-k:", 10,
   "Context size for optimization. Default is 10");
  Iter = Env.GetIfArgPrefixInt("-e:", 1,
   "Number of epochs in SGD. Default is 1");
  ParamP = Env.GetIfArgPrefixFlt("-p:", 1,
   "Return hyperparameter. Default is 1");
  ParamQ = Env.GetIfArgPrefixFlt("-q:", 1,
   "Inout hyperparameter. Default is 1");
  Verbose = Env.IsArgStr("-v", "Verbose output.");
  Directed = Env.IsArgStr("-dr", "Graph is directed.");
  Weighted = Env.IsArgStr("-w", "Graph is weighted.");
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

void GetRandomWalks(PWNet& InNet, TVVec<TInt, int64>& WalksVV, TIntV& NIdsV,double& ParamP, double& ParamQ, 
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

void TestWriteWalkVV(TVVec<TInt, int64>& WalksVV, TStr& OutFile) {
  TFOut FOut(OutFile);
  for (int64 i = 0; i < WalksVV.GetXDim(); i++) {
    for (int64 j = 0; j < WalksVV.GetYDim(); j++) {
      FOut.PutInt(WalksVV(i, j));
    }
    FOut.PutLn();
  }
}

void ComputeMetricsForNodes(const PWNet& InNet, const TVVec<TInt, int64>& WalksVV, TIntFltH MetricCounter) {
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

void BuildSampleGraph(PWNet& SampleNet, const TIntFltH& MetricCounter) {
  int pass;
}

int main(int argc, char* argv[]) {
  TStr InFile,OutFile;
  int Dimensions, WalkLen, NumWalks, WinSize, Iter;
  double ParamP, ParamQ;
  bool Directed, Weighted, Verbose;
  ParseArgs(argc, argv, InFile, OutFile, Dimensions, WalkLen, NumWalks, WinSize,
   Iter, Verbose, ParamP, ParamQ, Directed, Weighted);


  PWNet InNet = PWNet::New();

  ReadGraph(InFile, Directed, Weighted, Verbose, InNet);

  // Prepare for random walk
  //PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);

  TIntV NIdsV;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    NIdsV.Add(NI.GetId());
  }
  int64 AllWalks = (int64)NumWalks * NIdsV.Len();
  TVVec<TInt, int64> WalksVV(AllWalks,WalkLen);

  // Get the random walks in main memory as WalksVV
  GetRandomWalks(InNet, WalksVV, NIdsV, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);

  THash<TInt, TFlt> 

  TIntFltH MetricCounter;
  ComputeMetricsForNodes(InNet, WalksVV, MetricCounter);

  PWNet SampleNet = PWNet::New();
  BuildSampleGraph(SampleNet, MetricCounter)


  RecoverEdges(InNet, SampleNet);


  node2vec(InNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHVForSample);

  TIntFltVH EmbeddingsHVForSample;
  TIntFltVH EmbeddingsHVForAll;


  LinearInterpolation(InNet, EmbeddingsHVForAll, EmbeddingsHVForSample, 
    NIdsV, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);
  
  return 0;
}

