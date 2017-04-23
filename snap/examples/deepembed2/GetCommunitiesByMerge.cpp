#include "stdafx.h"

#include "biasedrandomwalk.h"
#include <vector>
#include <algorithm>


void GetTopGroup(std::vector<int>& TopGroup, const std::vector<std::vector<int> >& C2N, 
	const int& NumCommunities) {

	std::vector<int> GroupSize;
	for (int i = 0; i < C2N.size(); i++) {GroupSize.push_back(C2N[i].size());}
	int CutPoint = C2N.size() - NumCommunities;
	std::nth_element(GroupSize.begin(), GroupSize.begin() + CutPoint, GroupSize.end());
	int CutThreshold = GroupSize[CutPoint];
	for (int i = 0; i < C2N.size(); i++) {
		if (C2N[i].size() >= CutThreshold) {TopGroup.push_back(i);}
	}
}


/* Check the boundary conditions of a group vectors
 * If NodeId is not an boundary node, return == 0. 
 * If NodeId is not an active node, return > 0
 * If NodeId is an dead boundary node return < 0
 */

int GetBoundayCondition(const PWNet& InNet, int NodeId, const THash<TInt, TInt>& N2C, 
	const THash<TInt, TInt>& OldC2NewC, THashSet<TInt> Recorder, bool RecordDir = false) {
	int Condition = 0;
	int flag = 1;
	int OldGroupId = N2C[NodeId];
	TWNet::TNodeI NodeIter= InNet->GetNI(NodeId);
	for (int e = 0; e < NodeIter.GetOutDeg(); e++) {
		int NeighborGroup = N2C[NodeIter.GetOutNId(e)];
		if (NeighborGroup == OldGroupId) {continue;}
		Condition++;
		if (!OldC2NewC.IsKey(NeighborGroup)) {
			flag = -1;
			if (RecordDir) {Recorder.AddKey(NodeIter.GetOutNId(e));}
		}
	}
	return Condition * flag;
}


void InitActiveBoundary(std::vector<std::vector<int> >& ActiveBoundary, const PWNet& InNet, 
	const std::vector<std::vector<int> >& NewC2N, const THash<TInt, TInt>& OldC2NewC, 
	const THash<TInt, TInt>& N2C) {

	THashSet<TInt> PlaceHolder;
	for (int i = 0; i < NewC2N.size(); i++) {
		ActiveBoundary.push_back(std::vector<int> ());
		for (int j = NewC2N[i].size() - 1; j > -1; j--) {
			int NodeId = NewC2N[i][j];
			int Condition = GetBoundayCondition(InNet, NodeId, N2C, OldC2NewC, PlaceHolder);
			// Went to inner nodes, no need to check further, stop
			if (Condition == 0) {break;}
			// Record active boundary nodes for this group
			else if (Condition > 0) {ActiveBoundary[i].push_back(NodeId);}
		}
	}
}



void GetNewActiveBoundaryForOneGroup(const PWNet& InNet, int CurrGroup, const std::vector<std::vector<int> >& ActiveBoundary,
	std::vector<std::vector<int> >& TempActiveBoundary, const THash<TInt, TInt>& N2C, const THash<TInt, TInt>& OldC2NewC) {

	THashSet<TInt> OutSiderRecorder;
	for (int i = 0; i < ActiveBoundary[CurrGroup].size(); i++) {
		int NodeId = ActiveBoundary[CurrGroup][i];
		int PossibleMergeNode;
		// record possible merge nodes to OutSiderRecorder starting form this active nodes
		GetBoundayCondition(InNet, NodeId, N2C, OldC2NewC, OutSiderRecorder, true);
	}
	// Write temporary active nods for this group
	for (THashSet<TInt>::TIter ThashIter = OutSiderRecorder.BegI(); ThashIter < OutSiderRecorder.EndI(); ThashIter++) {
		TempActiveBoundary[CurrGroup].push_back(*ThashIter);
	}
}


void ResolveConflicts() {

}

void UpdateC2N() {

}

void GetCommunitiesByMerge(const PWNet& InNet, std::vector<std::vector<int> >& C2N, std::vector<std::vector<int> >& NewC2N,
					 THash<TInt, TInt>& N2C, int& NumCommunities) {

	if (NumCommunities <= C2N.size()) {
		NewC2N = C2N;
		return;
	}

	int NumSettledNodes = 0;
	// Get top NumCommunities
	std::vector<int> TopGroup;
	GetTopGroup(TopGroup, C2N, NumCommunities);
	THash<TInt, TInt> OldC2NewC;
	/* Remark: N2C and C2N are ordered in original order, NewC2N are in [0, NumCommunities).
	 * Use OldC2NewC do conversion.
	 */
	for (int i = 0; i < TopGroup.size(); i++) {
		OldC2NewC.AddDat(TopGroup[i], i);
		NewC2N.push_back(C2N[TopGroup[i]]);
		NumSettledNodes += C2N[TopGroup[i]].size();
	}
	// Build active boundary for each group
	std::vector<std::vector<int> > ActiveBoundary;
	InitActiveBoundary(ActiveBoundary, InNet, NewC2N, OldC2NewC, N2C);

	std::vector<std::vector<int> > TempActiveBoundary(ActiveBoundary.size(), std::vector<int> ());	
	while (NumSettledNodes < InNet->GetNodes()) {
		for (int i = 0; i < NewC2N.size(); i++) {
			GetNewActiveBoundaryForOneGroup(InNet, i, ActiveBoundary, TempActiveBoundary, N2C, OldC2NewC);
		}

		ResolveConflicts(); // TO DO
	}

	UpdateC2N(); // TO DO

}