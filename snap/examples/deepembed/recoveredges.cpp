#include "stdafx.h"
#include "n2v.h"
#include "deeputils.h"


void TestRECOVEREDGES() {
}

void RecoverEdges(PWNet& InNet, PWNet& OutNet, THashSet<TInt>& RepresentativeNodes, 
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose){
  int64 AllWalks = Srcs.Len() * NumWalks;
  TRnd Rnd(time(NULL));
  int64 WalksDone = 0;

  for(THashSet<TInt>::TIter ThashIter = RepresentativeNodes.BegI(); 
    ThashIter < RepresentativeNodes.EndI(); ThashIter++){
    OutNet->AddNode((*ThashIter).Key);
  }

  for (int64 i = 0; i < NumWalks; i++) {
    Srcs.Shuffle(Rnd);
#pragma omp parallel for schedule(dynamic)
    for(THashSet<TInt>::TIter ThashIter = RepresentativeNodes.BegI(); 
      ThashIter < RepresentativeNodes.EndI(); ThashIter++){
      if ( Verbose && WalksDone%10000 == 0 ) {
        printf("\rWalking Progress: %.2lf%%",(double)WalksDone*100/(double)AllWalks);fflush(stdout);
      }
      TIntV WalkV;
      SimulateWalk(InNet, (*ThashIter).Key, WalkLen, Rnd, WalkV);
      
      for (int64 k = 0; k < WalkV.Len(); k++) { 
        if(WalkV[k] == (*ThashIter)){
          continue;
        }
        if(RepresentativeNodes.IsKey(WalkV[k])){
            if(OutNet->IsEdge((*ThashIter).Key, WalkV[k])){
                OutNet->GetEDat((*ThashIter).Key, WalkV[k]) += 1;
                OutNet->GetEDat(WalkV[k], (*ThashIter).Key) += 1;
            }else{
                OutNet->AddEdge((*ThashIter).Key, WalkV[k]);
                OutNet->AddEdge(WalkV[k], (*ThashIter).Key);
                OutNet->SetEDat((*ThashIter).Key, WalkV[k], 1);
                OutNet->SetEDat(WalkV[k], (*ThashIter).Key, 1); 
            }
        }
      }
      WalksDone++;
    }
  }
  if (Verbose) {
    printf("\n");
    fflush(stdout);
  }
}