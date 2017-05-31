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

void LevelWiseSpread(PWNet& InNet, TIntFltVH& EmbeddingsHVForSample, TIntFltVH& EmbeddingsHVForAll, int& Dimensions){

	int64 Settled;
	int64 Unsettled;
	int64 TotalNodes = InNet->GetNodes();
	int Round = 0;

	while(true){
		Round += 1;
		Settled = EmbeddingsHVForAll.Len();
		Unsettled = TotalNodes - Settled;
		if(Unsettled == 0){
			break;
		}
		printf("Begin round %d\n", Round);
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

		for(int i = 0; i < Ids.Len(); i++){
			TWNet::TNodeI curtNode = InNet->GetNI(Ids[i]);
			for(int e = 0; e < curtNode.GetDeg(); e++){
				int NId = curtNode.GetNbrNId(e);
      			if(EmbeddingsHVForAll.IsKey(NId)){
      				Srcs(Ids[i]) = AddVectors(Srcs(Ids[i]), EmbeddingsHVForAll(NId));
      				Count(Ids[i]) += 1;
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
 




