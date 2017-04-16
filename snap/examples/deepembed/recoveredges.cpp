#include "stdafx.h"
#include "n2v.h"
#include "deeputils.h"


void TestRECOVEREDGES() {
}

void RecoverEdges(PWNet& InNet, PWNet& OutNet, THashSet<TInt>& RepresentativeNodes, 
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose){

  TIntV Srcs; 
  for(THashSet<TInt>::TIter ThashIter = RepresentativeNodes.BegI(); ThashIter < RepresentativeNodes.EndI(); ThashIter++){
    OutNet->AddNode((*ThashIter));
    Srcs.Add((*ThashIter));
  }

  int64 WalksDone = 0;
  int64 AllWalks = Srcs.Len() * NumWalks;
  TRnd Rnd(time(NULL));

  for (int64 i = 0; i < NumWalks; i++) {
    Srcs.Shuffle(Rnd);
 #pragma omp parallel for schedule(dynamic)
    for(int64 j = 0; j < Srcs.Len(); j++){
      if ( Verbose && WalksDone%10000 == 0 ) {
        printf("\rWalking Progress: %.2lf%%",(double)WalksDone*100/(double)AllWalks);fflush(stdout);
      }
      TIntV WalkV;
      SimulateWalk(InNet, Srcs[j], WalkLen, Rnd, WalkV);
      
      for (int64 k = 0; k < WalkV.Len(); k++) { 
        if(WalkV[k] == Srcs[j]){
          continue;
        }
        if(RepresentativeNodes.IsKey(WalkV[k])){
            if(OutNet->IsEdge(Srcs[j], WalkV[k])){
                // printf("%f\n", OutNet->GetEDat(Srcs[j], WalkV[k]).Val);
                (OutNet->GetEDat(Srcs[j], WalkV[k])) += 1;
                (OutNet->GetEDat(WalkV[k], Srcs[j])) += 1;
            }else{
                OutNet->AddEdge(Srcs[j], WalkV[k]);
                OutNet->AddEdge(WalkV[k], Srcs[j]);
                // printf("%f ", (OutNet->GetEDat(Srcs[j], WalkV[k])).Val);
                OutNet->SetEDat(Srcs[j], WalkV[k], 1.0);
                OutNet->SetEDat(WalkV[k], Srcs[j], 1.0); 
                // printf("%f\n", (OutNet->GetEDat(Srcs[j], WalkV[k])).Val);
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