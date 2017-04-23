#include "stdafx.h"
#include "biasedrandomwalk.h"

#include <iostream>
#include <utility>
#include <queue>
<<<<<<< Updated upstream
#include <vector>
#include <algorithm>

=======
/*
>>>>>>> Stashed changes
bool ComparePair(const std::pair<int, int>& p1, const std::pair<int, int>& p2){
	return p1.second < p2.second;
}

void GetSortedDegreePairs(PUNGraph InNet, std::vector<std::pair<int, int> >& AllDegreePairs){
	for(TUNGraph::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++){
		AllDegreePairs.push_back(std::pair<int, int>(NI.GetId(), NI.GetDeg()));
	}
	std::sort(AllDegreePairs.begin(), AllDegreePairs.end(), ComparePair);
}

int GrowCommunityByBFS(PUNGraph InNet, std::vector< std::vector<int> >& C2N, 
	THash<TInt, TInt>& N2C, TIntV& Merged, int StartNId, 
	int CommunityID, double UpdateRateThreshold, int CommunitySize){
	
	TIntH Visited;
	std::vector<int> Community;
	double UpDateRate = 1;
	int CurrentSize = 0;
	std::queue<int> Q;
	Q.push(StartNId);
	Visited.AddKey(StartNId);

	while(UpDateRate > UpdateRateThreshold && CurrentSize < CommunitySize && !Q.empty()){
		int QSize = Q.size();
		int AllNeighbors = 0;
		int GoodNeighbors = 0;
		for(int i = 0; i < QSize; i++){// level order BFS
			int curtNId = Q.front();
			Q.pop();
			CurrentSize++;
			N2C.AddDat(curtNId) = CommunityID;
			Community.push_back(curtNId);
			Merged.Add(curtNId);
			if(CurrentSize >= CommunitySize){
				break;
			}
			TUNGraph::TNodeI curtNI = InNet->GetNI(curtNId);
			for(int j = 0; j < curtNI.GetDeg(); j++){
				AllNeighbors++;
				int NeighborID = curtNI.GetNbrNId(j);
				if(Visited.IsKey(NeighborID) || N2C.IsKey(NeighborID)){
					continue;
				}else{
					GoodNeighbors++;
					Q.push(NeighborID);
					Visited.AddKey(NeighborID);
				}
			}
		}
		if(AllNeighbors == 0){
			UpDateRate = 0;
		}else{
			UpDateRate = (double)(GoodNeighbors) / (double)(AllNeighbors);
		}
		// printf("UpDateRate: %f\n", UpDateRate);
	}
	// printf("Finish growing. Final Size: %d\n", CurrentSize);
	C2N.push_back(Community);
	return CurrentSize;
}

void GetRawCommunities(PWNet InNet, std::vector<std::vector<int> >& C2N,
	THash<TInt, TInt>& N2C, double UpdateRateThreshold, int NumCommunities){
	
	PUNGraph Net = TSnap::ConvertGraph<PUNGraph, PWNet>(InNet);
	int AllNodes = Net->GetNodes();
	int Settled = 0;
	int CommunityID = 0;
	int CommunitySize = AllNodes / NumCommunities;
	while(Settled < AllNodes){
		std::vector<std::pair<int, int> > SortedDegreePairs;
		GetSortedDegreePairs(Net, SortedDegreePairs);
		int NumStartNode = (int)(SortedDegreePairs.size() * 0.1) + 1;
		// printf("NumStartNode: %d\n", NumStartNode);
		NumStartNode = NumStartNode < SortedDegreePairs.size() ? NumStartNode : SortedDegreePairs.size();
		TIntV Merged;
		for(int i = 0; i < NumStartNode; i++){
			if(!N2C.IsKey(SortedDegreePairs[i].first)){
				Settled += GrowCommunityByBFS(Net, C2N, N2C, Merged,
					SortedDegreePairs[i].first, CommunityID, 
					UpdateRateThreshold, CommunitySize);
				CommunityID++;
			}
		}

		// printf("Begin Deleting Settled Nodes\n");
		for(int j = 0; j < Merged.Len(); j++){//Deleted those already settled
			Net->DelNode(Merged[j]);
		}
		// printf("Finish Deleting Settled Nodes\n");
	}
}

*/