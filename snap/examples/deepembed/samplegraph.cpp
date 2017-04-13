#include "stdafx.h"
#include "n2v.h"
#include "deeputils.h"



void TestSAMPLEGRAPH() {
	PWNet InNet = PWNet::New();
	float i = 100;
	printf("%f\n", i);
}

void BuildSampleGraph(PWNet& InNet, PWNet& SampleNet, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& Iter, bool& Verbose) {

	TIntV NIdsV;
	for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
		NIdsV.Add(NI.GetId());
	}
	int64 AllWalks = (int64)NumWalks * NIdsV.Len();
	TVVec<TInt, int64> WalksVV(AllWalks,WalkLen);

	// Get the random walks in main memory as WalksVV
	GetRandomWalks(InNet, WalksVV, NIdsV, ParamP, ParamQ, Dimensions, WalkLen, NumWalks, Iter, Verbose);

	TIntFltH MetricCounter;
	ComputeMetricsForNodes(InNet, WalksVV, MetricCounter);
	

}
