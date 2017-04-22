#ifndef GETRAWCOMMYNITIES_H
#define GETRAWCOMMYNITIES_H

ComparePair(const std::pair<int, int>& p1, const std::pair<int, int>& p2);

void GetSortedDegreePairs(PWNet InNet, std::vector<std::pair<int, int> >& AllDegreePairs);

int GrowCommunityByBFS(PWNet InNet, std::vector<std::vector<int> >& C2N, 
	THash<TInt, TInt>& N2C, TIntV& Merged, int StartNId, 
	int CommunityID, double UpdateRateThreshold, int CommunitySize);

void GetRawCommunities(PWNet InNet, std::vector<std::vector<int> >& C2N,
	THash<TInt, TInt>& N2C, double UpdateRateThreshold, int NumCommunities);

#endif //GETRAWCOMMYNITIES_H