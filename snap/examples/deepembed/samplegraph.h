#ifndef SAMPLEGRAPH_H
#define SAMPLEGRAPH_H

void TestSAMPLEGRAPH();

void BuildSampleGraph(PWNet& InNet, PWNet& SampleNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);

#endif //SAMPLEGRAPH_H