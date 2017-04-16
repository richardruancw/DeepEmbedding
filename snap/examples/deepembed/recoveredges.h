#ifndef RECOVEREDGES_H
#define RECOVEREDGES_H

void TestRECOVEREDGES();

void RecoverEdges(PWNet& InNet, PWNet& OutNet, THashSet<TInt>& RepresentativeNodes, 
	double& ParamP, double& ParamQ, int& Dimensions, int& WalkLen, int& NumWalks, int& Iter, bool& Verbose);

#endif //RECOVEREDGES_H