#ifndef SAMPLEN2V_H
#define SAMPLEN2V_H

#include "stdafx.h"

#include "Snap.h"
#include "biasedrandomwalk.h"
#include "word2vec.h"

/// Calculates node2vec feature representation for nodes and writes them into EmbeddinsHV, see http://arxiv.org/pdf/1607.00653v1.pdf
void samplenode2vec(PWNet& InNet, const THashSet<TInt>& RepresentativeNodes, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV); 

/// Version for unweighted graphs
void samplenode2vec(PNGraph& InNet, const THashSet<TInt>& RepresentativeNodes, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV); 

/// Version for weighted graphs. Edges must have TFlt attribute "weight"
void samplenode2vec(PNEANet& InNet, const THashSet<TInt>& RepresentativeNodes, double& ParamP, double& ParamQ, int& Dimensions,
 int& WalkLen, int& NumWalks, int& WinSize, int& Iter, bool& Verbose,
 TIntFltVH& EmbeddingsHV);
#endif //SAMPLEN2V_H
