#include "stdafx.h"
#include "GetRawCommunities.h"

#include <queue>

bool ComparePair(const std::pair<int, int>& p1, const std::pair<int, int>& p2){
	return p1.first < p2.first;
}

void GetSortedDegreePairs(PWNet InNet, std::vector<std::pair<int, int> >& AllDegreePairs){
	for(TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++){
		AllDegreePairs.push_back(std::pair<int, int>(NI.GetId(), NI.GetDeg()));
	}
	std::sort(AllDegreePairs.begin(), AllDegreePairs.end(), ComparePair);
}

int GrowCommunityByBFS(PWNet InNet, std::vector<std::vector<int> >& C2N, 
	THash<TInt, TInt>& N2C, TIntV& Merged, int StartNId, 
	int CommunityID, double UpdateRateThreshold, int CommunitySize){
	
	std::vector<int> Community;
	double UpDateRate = 1;
	int CurrentSize = 0;
	std::queue<int> Q;
	Q.push_back(StartNId);

	while(UpDateRate > UpdateRateThreshold && CurrentSize < CommunitySize && !Q.empty()){
		int QSize = Q.size();
		int AllNeighbors = 0;
		int MergedNeighbors = 0;
		for(int i = 0; i < QSize; i++){// level order BFS
			int curtNId = Q.pop_front();
			TWNet::TNode curtNI = PWNet.GetNI(curtNId);
			for(int j = 0; j < curtNI.GetDeg(); j++){
				AllNeighbors++;
				int NeighborID = curtNI.GetNbrNId(j);
				if(N2C.IsKey(NeighborID)){
					continue;
				}else{
					MergedNeighbors++;
					CurrentSize++;
					N2C.AddDat(NeighborID) = CommunityID;
					Community.push_back(NeighborID);
					Merged.Add(NeighborID);
					Q.push_back(NeighborID);
				}
			}
		}
		UpDateRate = (double)(MergedNeighbors) / (double)(AllNeighbors);
	}

	C2N.push_back(Community);
	return CurrentSize;
}

void GetRawCommunities(PWNet InNet, std::vector<std::vector<int> >& C2N,
	THash<TInt, TInt>& N2C, double UpdateRateThreshold, int NumCommunities){

	int AllNodes = InNet->GetNodes();
	int Settled = 0;
	int CommunityID = 0;
	int CommunitySize = AllNodes / NumCommunities;
	while(Settled < AllNodes){
		std::vector<std::pair<int, int> > SortedDegreePairs;
		GetSortedDegreePairs(InNet, SortedDegreePairs);
		int NumStartNode = (int)(SortedDegreePairs.size() * 0.1) + 1;
		NumStartNode = NumStartNode < SortedDegreePairs.size() ? NumStartNode : SortedDegreePairs.size();
		TIntV Merged;
		for(int i = 0; i < NumStartNode; i++){
			if(!N2C.IsKey(SortedDegreePairs[i].first)){
				Settled += GrowCommunityByBFS(InNet, C2N, N2C, Merged,
					SortedDegreePairs[i].first, CommunityID, 
					UpdateRateThreshold, CommunitySize);
				CommunityID++;
			}
		}
		for(int j = 0; j < Merged.Len(); j++){//Deleted those already settled
			InNet.DelNode(Merged[j]);
		}
	}
}