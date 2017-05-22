#ifndef GETCOMMUNITIESBYMERGE_H
#define GETCOMMUNITIESBYMERGE_H


/* Merge small communities to larger communities by level BFS
 * @parama InNet  Graph of original big network
 * @parama C2N  A vector of vector of node ids. Each sub vector is of a communities.
 * @parama N2C  A hash table map from node id to the community ID is belongs to.
 * @pre InNet.GetNodes() == # node id in C2N
 *
 * @post InNet.size() <= NumCommunities && # Unique communities id in N2C <= NumCommunities
 *       Unique communities id in N2C == InNet.size();
 */
void GetCommunitiesByMerge(const PWNet& InNet, const std::vector<std::vector<int> >& C2N, std::vector<std::vector<int> >& NewC2N,
					 THash<TInt, TInt>& N2C, int& NumCommunities);

#endif //GETCOMMUNITIESBYMERGE_H