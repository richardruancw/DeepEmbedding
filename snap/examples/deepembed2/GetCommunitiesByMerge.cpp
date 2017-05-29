#include "stdafx.h"

#include "biasedrandomwalk.h"
#include <vector>
#include <algorithm>


bool SizeCompare(const std::pair<int, int>& Left, const std::pair<int, int>& Right) {
		return Left.second < Right.second;
	}


void GetTopGroup(std::vector<int>& TopGroup, const std::vector<std::vector<int> >& C2N, 
	const int& NumCommunities) {

	std::vector<std::pair<int, int> > IdAndSize;
	for (int i = 0; i < C2N.size(); i++) {
		IdAndSize.push_back(std::make_pair(i, C2N[i].size()));
	}
	std::sort(IdAndSize.begin(), IdAndSize.end(), SizeCompare);
	int count = 0;
	for (int i = IdAndSize.size() - 1; i > -1; i--) {
		TopGroup.push_back(IdAndSize[i].first);
		count++;
		if (count >= NumCommunities) {return;}
	}
}

bool InputIsValid(const std::vector<std::vector<int> >& C2N, THash<TInt, TInt>& N2C) {
	for (int i = 0; i < C2N.size(); i++) {
		for (int j = 0; j < C2N[i].size(); j++) {
			if (N2C.GetDat(C2N[i][j]) != i) {
				printf("The input N2C and C2N is not consistent !\n");
				return false;
			}
		}
	}
	return true;
}

/* Check the boundary conditions of a group vectors
 * If NodeId is not an boundary node, return == 0. 
 * If NodeId is an active node, return > 0
 * If NodeId is an dead boundary node return < 0
 */

int GetBoundayCondition(int Round, const PWNet& InNet, int NodeId, const THash<TInt, TInt>& N2C, 
	const THash<TInt, TInt>& OldC2NewC, THashSet<TInt>& Recorder, bool RecordDir = false) {
	int Condition = 0;
	int NumOutSider = 0;
	int flag = -1;
	int OldGroupId = N2C.GetDat(NodeId);
	TWNet::TNodeI NodeIter= InNet->GetNI(NodeId);
	for (int e = 0; e < NodeIter.GetOutDeg(); e++) {
		int NeighborGroup = N2C.GetDat(NodeIter.GetOutNId(e));

		if (NeighborGroup == OldGroupId) {continue;}
		Condition++;
		if (!OldC2NewC.IsKey(NeighborGroup)) {
			flag = 1;
			if (RecordDir) {
				Recorder.AddKey(NodeIter.GetOutNId(e));
				NumOutSider++;
			}
		}
		//if (NumOutSider > Round) {break;}
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
			int Condition = GetBoundayCondition(0, InNet, NodeId, N2C, OldC2NewC, PlaceHolder);
			// Went to inner nodes, no need to check further, stop
			//if (Condition == 0) {break;}
			// Record active boundary nodes for this group
			if (Condition > 0) {ActiveBoundary[i].push_back(NodeId);}
		}
	}
}



void GetNewActiveBoundaryForOneGroup(int Round, const PWNet& InNet, int CurrGroup, const std::vector<std::vector<int> >& ActiveBoundary,
	std::vector<std::vector<int> >& TempActiveBoundary, const THash<TInt, TInt>& N2C, const THash<TInt, TInt>& OldC2NewC) {

	THashSet<TInt> OutSiderRecorder;
	for (int i = 0; i < ActiveBoundary[CurrGroup].size(); i++) {
		int NodeId = ActiveBoundary[CurrGroup][i];
		// record possible merge nodes to OutSiderRecorder starting form this active nodes
		GetBoundayCondition(Round, InNet, NodeId, N2C, OldC2NewC, OutSiderRecorder, true);
	}
	// Write temporary active nods for this group
	for (THashSet<TInt>::TIter ThashIter = OutSiderRecorder.BegI(); ThashIter < OutSiderRecorder.EndI(); ThashIter++) {
		TempActiveBoundary[CurrGroup].push_back(*ThashIter);
	}
}



void ResolveConflictsBySort(std::vector<std::vector<int> >& NewC2N, std::vector<std::vector<int> > TempActiveBoundary, 
	THash<TInt, TInt>& N2C, const std::vector<int>& TopGroup, std::vector<std::vector<int> >& ActiveBoundary, int& NumSettledNodes) {
		// resolve conflicts in the temporary active boundary, start from small to large group
	THashSet<TInt> AssignedNodes;
	std::vector<std::pair<int, int> > IdAndSize;
	for (int i = 0; i < NewC2N.size(); i++) {
		IdAndSize.push_back(std::make_pair(i, NewC2N[i].size()));
	}
	std::sort(IdAndSize.begin(), IdAndSize.end(), SizeCompare);
	for (int i = 0; i < IdAndSize.size(); i++) {
		int CurrGroup = IdAndSize[i].first;
		std::vector<int> Temp;
		for (int j = 0; j < TempActiveBoundary[CurrGroup].size(); j++) {
			int CurrNode = TempActiveBoundary[CurrGroup][j];
			if (AssignedNodes.IsKey(CurrNode)) {continue;}
			// Record 
			AssignedNodes.AddKey(CurrNode);
			// Give this nodes to one new group. Mark it and push it to the new boundary.
			// 1. Change N2C; 2. Put to active boundary; 3. Get merged by CurrGroup.
			N2C.GetDat(CurrNode) = TopGroup[CurrGroup];
			Temp.push_back(CurrNode);
			NewC2N[CurrGroup].push_back(CurrNode);
		}
		ActiveBoundary[CurrGroup] = Temp;
	}
	NumSettledNodes += AssignedNodes.Len();
}


void ResolveConflictsByRandomAssign(std::vector<std::vector<int> >& NewC2N, std::vector<std::vector<int> >& TempActiveBoundary, 
	THash<TInt, TInt>& N2C, const std::vector<int>& TopGroup, THash<TInt, TInt>& OldC2NewC,
	std::vector<std::vector<int> >& ActiveBoundary, int& NumSettledNodes) {
	TRnd seed(10);
	THash<TInt, TIntV> NodeToCand;
	for (int i = 0; i < TempActiveBoundary.size(); i++) {
		for (int j = 0; j < TempActiveBoundary[i].size(); j++) {
			TInt CurrNode = TempActiveBoundary[i][j];
			if (!NodeToCand.IsKey(CurrNode)) {
				NodeToCand.AddDat(CurrNode, TIntV());
			}
			NodeToCand.GetDat(CurrNode).Add(TopGroup[i]);
		}
	}

	ActiveBoundary.clear();
	for (int i = 0; i < NewC2N.size(); i++) {ActiveBoundary.push_back(std::vector<int> ());}
	for (THash<TInt, TIntV>::TIter iter = NodeToCand.BegI(); iter < NodeToCand.EndI(); iter++) {
		int NodeId = (*iter).Key;
		int ToOldGroup = (*iter).Dat.GetRndVal(seed);
		int ToNewGroup = OldC2NewC.GetDat(ToOldGroup);
		// Update 
		N2C.GetDat(NodeId) = ToOldGroup;
		NewC2N[ToNewGroup].push_back(NodeId);
		ActiveBoundary[ToNewGroup].push_back(NodeId);
		NumSettledNodes++;
	}

}


void GetCommunitiesByMerge(const PWNet& InNet, const std::vector<std::vector<int> >& C2N, std::vector<std::vector<int> >& NewC2N,
					 THash<TInt, TInt>& N2C, int& NumCommunities) {

	IAssert(TSnap::IsWeaklyConn<PWNet>(InNet));

	if (NumCommunities >= C2N.size()) {
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
	std::vector<std::vector<int> > TempActiveBoundary;
	InitActiveBoundary(ActiveBoundary, InNet, NewC2N, OldC2NewC, N2C);
	
	int Round = 0;
	int LastSettleNodes = 0;
	while (NumSettledNodes < InNet->GetNodes() && LastSettleNodes!= NumSettledNodes) {
		printf("Before the %d round of merging, we have %d nodes, target is %d\n", ++Round, NumSettledNodes, InNet->GetNodes());
		LastSettleNodes = NumSettledNodes;
		//#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < NewC2N.size(); i++) {
			TempActiveBoundary.push_back(std::vector<int> ());
			GetNewActiveBoundaryForOneGroup(Round, InNet, i, ActiveBoundary, TempActiveBoundary, N2C, OldC2NewC);
		}

		ResolveConflictsBySort(NewC2N, TempActiveBoundary, N2C, TopGroup, ActiveBoundary, NumSettledNodes);

		//ResolveConflictsByRandomAssign(NewC2N, TempActiveBoundary, N2C, TopGroup, OldC2NewC, ActiveBoundary, NumSettledNodes);

		// Clear 
		TempActiveBoundary.clear();
		printf("After the %d round of merging, we have %d nodes, target is %d\n", Round, NumSettledNodes, InNet->GetNodes());
	}

	if (NumSettledNodes < InNet->GetNodes()) {
		for (TWNet::TEdgeI EI = InNet->BegEI(); EI < InNet->EndEI(); EI++) {
			int sourceGroup = N2C.GetDat(EI.GetSrcNId());
			int endGroup = N2C.GetDat(EI.GetDstNId());
			if (!OldC2NewC.IsKey(sourceGroup) && OldC2NewC.IsKey(endGroup)) {
				N2C.GetDat(EI.GetSrcNId()) = endGroup;
				NewC2N[OldC2NewC.GetDat(endGroup)].push_back(EI.GetSrcNId());
				NumSettledNodes++;
			} else if (OldC2NewC.IsKey(sourceGroup) && !OldC2NewC.IsKey(endGroup)) {
				N2C.GetDat(EI.GetDstNId()) = sourceGroup;
				NewC2N[OldC2NewC.GetDat(sourceGroup)].push_back(EI.GetDstNId());
				NumSettledNodes++;
			}
		}
		printf("The final commnuites has num of nodes: %d\n", NumSettledNodes);
		printf("The final looks like: \n");
		for (int i = 0; i < NewC2N.size(); i++) {
			printf("%d  ", NewC2N[i].size());
		}
		printf("\n");
	}


	// Update C2N
	for (THash<TInt, TInt>::TIter ThashIter = N2C.BegI(); ThashIter < N2C.EndI(); ThashIter++) {
		IAssert(OldC2NewC.IsKey((*ThashIter).Dat));
		(*ThashIter).Dat = OldC2NewC.GetDat((*ThashIter).Dat);
	}


    // Following are debug code for following tasks.
    /*
	IAssert(InputIsValid(NewC2N, N2C));
	int TotalNodes = 0;
	for (int i = 0; i < NewC2N.size(); i++) {
		PWNet smallNet = PWNet::New();
		THashSet<TInt> Duplicated;
		for (int j = 0; j < NewC2N[i].size(); j++) {
			IAssert(!Duplicated.IsKey(NewC2N[i][j]));
			Duplicated.AddKey(NewC2N[i][j]);
			int NodeOne = NewC2N[i][j];
			for (int k = j+1; k < NewC2N[i].size(); k++) {
				int NodeTwo = NewC2N[i][k];
				IAssert(InNet->IsNode(NodeOne) && InNet->IsNode(NodeTwo));
				if (InNet->IsEdge(NodeOne, NodeTwo) && InNet->IsEdge(NodeTwo, NodeOne)) {
					if (!smallNet->IsNode(NodeOne)) {smallNet->AddNode(NodeOne); TotalNodes++;}
					if (!smallNet->IsNode(NodeTwo)) {smallNet->AddNode(NodeTwo); TotalNodes++;}
					double weight12 = InNet->GetEI(NodeOne, NodeTwo).GetDat();
					double weight21 = InNet->GetEI(NodeTwo, NodeOne).GetDat();
					if (!smallNet->IsEdge(NodeOne, NodeTwo)) {smallNet->AddEdge(NodeOne, NodeTwo, weight12);}	
					if (!smallNet->IsEdge(NodeTwo, NodeOne)) {smallNet->AddEdge(NodeTwo, NodeOne, weight21);}	
				}
			}
		}

		IAssert(Duplicated.Len() == NewC2N[i].size());
		printf("%d , %d\n", smallNet->GetNodes(), NewC2N[i].size());
		IAssert(smallNet->GetNodes() == NewC2N[i].size());
		IAssert(TSnap::IsWeaklyConn<PWNet>(smallNet));
	}
	IAssert(TotalNodes == NumSettledNodes);

    
	for (TWNet::TNodeI iter = InNet->BegNI(); iter < InNet->EndNI(); iter++) {
		int group = N2C.GetDat(iter.GetId());
		int flag = 0;
		for (int i = 0; i < NewC2N[group].size(); i++) {
			if (NewC2N[group][i] == iter.GetId()) {
				flag = 1;
			}
		}
		IAssert(flag == 1);
	}
	*/

}