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

void LinearInterpolation(PWNet& InNet, TIntFltVH& EmbeddingsHVForSample, TIntFltVH& EmbeddingsHVForAll, TIntV& Unsettled,
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose){

	TIntIntH Count;
	TIntV Srcs;
	TRnd Rnd(time(NULL));
	printf("%d\n", InNet->GetNodes());
	for(TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++){
		Srcs.Add(NI.GetId());
	}
  	for (int64 i = 0; i < NumWalks; i++) {
  		printf("Round: %d\n", i);
  		printf("Len of Srcs: %d\n", Srcs.Len());
		// #pragma omp parallel for schedule(dynamic)
		for(int64 j = 0; j < Srcs.Len(); j++){
			if(EmbeddingsHVForSample.IsKey(Srcs[j])){ // If the node itself is a representative
				EmbeddingsHVForAll.AddDat(Srcs[j]) = TFltV(EmbeddingsHVForSample(Srcs[j]));
			}
			else{ // Else do the interpolation
				TIntV WalkV;
	      		SimulateWalk(InNet, Srcs[j], WalkLen, Rnd, WalkV);
	      		for(int k = 0; k < WalkV.Len(); k++){
	      			if(EmbeddingsHVForSample.IsKey(WalkV[k])){ // If the visited node is a representative node
	      				if(EmbeddingsHVForAll.IsKey(Srcs[j])){
	      					EmbeddingsHVForAll(Srcs[j]) = AddVectors(EmbeddingsHVForAll(Srcs[j]), EmbeddingsHVForSample(WalkV[k]));
	      					Count(Srcs[j]) += 1;
	      				}
	      				else{
	      					EmbeddingsHVForAll.AddDat(Srcs[j]) = TFltV(EmbeddingsHVForSample(WalkV[k]));
	      					Count.AddDat(Srcs[j]) = 1;
	      				}
	      			}// Else do nothing
	      		}
			}
		}
	}

	// #pragma omp parallel for schedule(dynamic)
	int bad = 0;
	int good = 0;
	for(int64 j = 0; j < Srcs.Len(); j++){
		if(EmbeddingsHVForSample.IsKey(Srcs[j])){
			good += 1;
			continue;
		}
  		if(Count(Srcs[j]) == 0){	// If visited no representative node at all
  			bad += 1;
  			Unsettled.Add(Srcs[j]);
  		}else{	// Else take the average
  			good += 1;
  			EmbeddingsHVForAll.GetDat(Srcs[j]) = RefactorVector(EmbeddingsHVForAll.GetDat(Srcs[j]), Count.GetDat(Srcs[j]));
  		}
	}
	printf("Bad: %d\n", bad);
	printf("Good: %d\n", good);
}
 




