#ifndef LINEARINTERPOLATION_H
#define LINEARINTERPOLATION_H

void TestLINEARINTERPOLATION();

void LevelWiseSpread(PWNet& InNet, TIntFltVH& EmbeddingsHVForSample, TIntFltVH& EmbeddingsHVForAll, int& Dimensions);

TFltV AddVectors(TFltV& v1, TFltV& v2);

TFltV RefactorVector(TFltV& v1, TInt& factor);

#endif //SAMPLEGRAPH_H