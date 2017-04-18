#ifndef LINEARINTERPOLATION_H
#define LINEARINTERPOLATION_H

void TestLINEARINTERPOLATION();

void LinearInterpolation(PWNet& InNet, TIntFltVH& EmbeddingsHVForSample, TIntFltVH& EmbeddingsHVForAll, int& TotalRound,
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, int& WinSize, bool& Verbose);

TFltV AddVectors(TFltV& v1, TFltV& v2);

TFltV RefactorVector(TFltV& v1, TInt& factor);

#endif //SAMPLEGRAPH_H