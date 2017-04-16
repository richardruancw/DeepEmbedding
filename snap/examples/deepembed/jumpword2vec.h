#ifndef JUMPWORD_2_VEC_H
#define JUMPWORD_2_VEC_H

namespace JumpWord2Vec {

///Learns embeddings using SGD, Skip-gram with negative sampling.
void LearnEmbeddings(TVVec<TInt, int64>& WalksVV, int& Dimensions, int& WinSize,
 int& Iter, bool& Verbose, TIntFltVH& EmbeddingsHV, TInt& FakeNodeId);
}

#endif //JUMPWORD_2_VEC_H

