#include "stdafx.h"
#include "n2v.h"

void samplenode2vec(PWNet& InNet, const THashSet<TInt>& RepresentativeNodes, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV) {
  // Preprocess transition probabilities
  PreprocessTransitionProbs(InNet, ParamP, ParamQ, Verbose);
  // Read representative nodes to vector
  TIntV RepNIdsV;
  for (THashSet<TInt>::TIter ThastIter = RepresentativeNodes.BegI(); ThastIter < RepresentativeNodes.EndI(); ThastIter++) {
    RepNIdsV.Add((*ThastIter));
  }
  //Generate random walks
  int64 AllWalks = (int64)NumWalks * RepNIdsV.Len();
  TVVec<TInt, int64> WalksVV(AllWalks,WalkLen);

  TRnd Rnd(time(NULL));
  int64 WalksDone = 0;

  // replace nodes which was visited but not in sample nodes with a fake node id.
  TInt FakeNodeId = InNet->GetMxNId();

  for (int64 i = 0; i < NumWalks; i++) {
    RepNIdsV.Shuffle(Rnd);
#pragma omp parallel for schedule(dynamic)
    for (int64 j = 0; j < RepNIdsV.Len(); j++) {

      if ( Verbose && WalksDone%10000 == 0 ) {
        printf("\rWalking Progress: %.2lf%%",(double)WalksDone*100/(double)AllWalks);fflush(stdout);
      }
      TIntV WalkV;
      SimulateWalk(InNet, RepNIdsV[j], WalkLen, Rnd, WalkV);
      for (int64 k = 0; k < WalkV.Len(); k++) { 
        if (RepresentativeNodes.IsKey(WalkV[k])) {
          WalksVV.PutXY(i*RepNIdsV.Len()+j, k, WalkV[k]);
        } else {
          WalksVV.PutXY(i*RepNIdsV.Len()+j, k, FakeNodeId);
        }
      }
      WalksDone++;
    }
  }
  if (Verbose) {
    printf("\n");
    fflush(stdout);
  }
  //Learning embeddings
  LearnEmbeddings(WalksVV, Dimensions, WinSize, Iter, Verbose, EmbeddingsHV);
  //Delete embedding for the fake node
  EmbeddingsHV.DelKey(FakeNodeId);
}

void samplenode2vec(PNGraph& InNet, const THashSet<TInt>& RepresentativeNodes, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV) {
  PWNet NewNet = PWNet::New();
  for (TNGraph::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
    if (!NewNet->IsNode(EI.GetSrcNId())) { NewNet->AddNode(EI.GetSrcNId()); }
    if (!NewNet->IsNode(EI.GetDstNId())) { NewNet->AddNode(EI.GetDstNId()); }
    NewNet->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), 1.0);
  }
  node2vec(NewNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHV);
}

void samplenode2vec(PNEANet& InNet, const THashSet<TInt>& RepresentativeNodes, double& ParamP, double& ParamQ,
 int& Dimensions, int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV) {
  PWNet NewNet = PWNet::New();
  for (TNEANet::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
    if (!NewNet->IsNode(EI.GetSrcNId())) { NewNet->AddNode(EI.GetSrcNId()); }
    if (!NewNet->IsNode(EI.GetDstNId())) { NewNet->AddNode(EI.GetDstNId()); }
    NewNet->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), InNet->GetFltAttrDatE(EI,"weight"));
  }
  node2vec(NewNet, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, WinSize, Iter, 
   Verbose, EmbeddingsHV);
}
