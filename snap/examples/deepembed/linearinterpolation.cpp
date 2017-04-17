#include "stdafx.h"
#include "n2v.h"
#include "deeputils.h"

TFltV AddVectors(TFltV& v1, TFltV& v2){
	TFltV re = TFltV(v1);
	for(int i = 0; i < re.Len(); i++){
		re[i] += v2[i];
	}
	return re;
} 

TFltV RefactorVector(TFltV& v1, TInt& factor){
	TFltV re = TFltV(v1);
	for(int i = 0; i < re.Len(); i++){
		re[i] /= factor;
	}
	return re;
}

void TestLINEARINTERPOLATION() {
}

void LinearInterpolation(PWNet& InNet, TIntFltVH& EmbeddingsHVForSample, TIntFltVH& EmbeddingsHVForAll, int& TotalRound,
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose){

	int64 Settled;
	int64 Unsettled;
	int64 TotalNodes = InNet->GetNodes();
	int Round = 0;

	while(Round < TotalRound){
		Round += 1;
		Settled = EmbeddingsHVForAll.Len();
		Unsettled = TotalNodes - Settled;
		printf("Begin round %d out of %d\n", Round, TotalRound);
		printf("Already settled %d out of %d, the portion is %f\n", 
			Settled, TotalNodes, (float)Settled / (float)TotalNodes );
		printf("Remain unsettled %d out of %d, the portion is %f\n", 
			Unsettled, TotalNodes, (float)Unsettled / (float)TotalNodes );

		TIntIntH Count;
		TIntFltVH Srcs;
		TIntV Ids;
		TRnd Rnd(time(NULL));
		for(TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++){
			if(EmbeddingsHVForAll.IsKey(NI.GetId())){
				continue;
			}
			Srcs.AddDat(NI.GetId()) = TFltV(Dimensions);
			Count.AddDat(NI.GetId()) = 0;
			Ids.Add(NI.GetId());
		}
	  	for (int64 i = 0; i < NumWalks; i++) {
			#pragma omp parallel for schedule(dynamic)
			for(int64 j = 0; j < Ids.Len(); j++){
				TIntV WalkV;
	      		SimulateWalk(InNet, Ids[j], WalkLen, Rnd, WalkV);
	      		for(int k = 0; k < WalkV.Len(); k++){
	      			if(EmbeddingsHVForAll.IsKey(WalkV[k])){ // If the visited node is settled
      					Srcs(Ids[j]) = AddVectors(Srcs(Ids[j]), EmbeddingsHVForAll(WalkV[k]));
      					Count(Ids[j]) += 1;
	      			}// Else do nothing
	      		}
			}
		}

		for(int64 j = 0; j < Ids.Len(); j++){
	  		if(Count(Ids[j]) == 0){	// If visited no settled node at all
	  			continue;
	  		}else{	// Else take the average
	  			EmbeddingsHVForAll.AddDat(Ids[j]) = RefactorVector(Srcs(Ids[j]), Count(Ids[j]));
	  		}
		}
	}
	printf("All rounds finished\n");
	printf("Already settled %d out of %d, the portion is %f\n", 
		Settled, TotalNodes, (float)Settled / (float)TotalNodes );
	printf("Remain unsettled %d out of %d, the portion is %f\n", 
		Unsettled, TotalNodes, (float)Unsettled / (float)TotalNodes );
}
 




