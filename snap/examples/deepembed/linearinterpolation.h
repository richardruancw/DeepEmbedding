#ifndef LINEARINTERPOLATION_H
#define LINEARINTERPOLATION_H

void TestLINEARINTERPOLATION();

void LinearInterpolation(PWNet& InNet, TIntFltVH EmbeddingsHVForSample, TIntFltVH EmbeddingsHVForAll, 
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);

#endif //SAMPLEGRAPH_H