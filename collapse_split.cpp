#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <vector>

int main(int argc, char* argv[]) {
  //// what type of graph do you want to use?
  // typedef PUNGraph PGraph; // undirected graph
  typedef PNGraph PGraph;  //   directed graph
  
  typedef TNodeEDatNet<TInt, TFlt> TWNet;
  typedef TPt<TWNet> PWNet;
  //typedef PNEGraph PGraph;  //   directed multigraph
  //typedef TPt<TNodeNet<TInt> > PGraph;
  //typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;

  // this code is independent of what particular graph implementation/type we use
  std::cout<<"start loading..."<<std::endl;  
  PGraph G3 = TSnap::LoadEdgeList<PGraph>("pins.tsv", 1,2,'\t');

  std::vector<int> Degrees;
  int numBoards = 0;
  for(PGraph::TObj::TNodeI NI = G3->BegNI(); NI < G3->EndNI(); NI++){
    if(NI.GetInDeg() != 0){
    numBoards++;
    Degrees.push_back(NI.GetDeg());}
  }
  int size = Degrees.size();
  double frac = 0.90;
  std::nth_element(Degrees.begin(), Degrees.begin()+ (int)(size*frac),Degrees.end());
  int threshold = Degrees[(int)(size*frac)];

  PWNet Net =  PWNet::New();
  int count = 0;
  std::cout<<"start compressing..."<<std::endl;
  
  for(PGraph::TObj::TNodeI NI = G3->BegNI(); NI < G3->EndNI(); NI++){
    if(NI.GetInDeg() == 0 || NI.GetDeg() > threshold) {continue;}
	//if(NI.GetInDeg() == 0) {continue;}
    for(int i = 0 ; i < NI.GetInDeg() ; i++){
      const int n1 = NI.GetInNId(i);
      for(int j = 0; j< NI.GetInDeg() ; j++){
        if(i >= j) {continue;}
        const int n2 = NI.GetInNId(j);
        if(! Net->IsNode(n1)){
          Net->AddNode(n1);
        }
        if(! Net->IsNode(n2)){
          Net->AddNode(n2);
        }

        if(Net->IsEdge(n1,n2)){Net->GetEDat(n1, n2)+=1;}
        else{Net->AddEdge(n1,n2,1);}
      }
    }
	count++;
  if(count%1000 == 0){
    printf("\rCompressing process: %2lf%%",100*(double)count/(double)numBoards);
    fflush(stdout);
  }
}
std::cout<<"finish compressing! \n"<<std::endl;
  TFOut FOut("collapsed.txt");
  for(TWNet::TEdgeI EI = Net->BegEI(); EI < Net -> EndEI(); EI++){
    FOut.PutInt(EI.GetSrcNId());
    FOut.PutCh(' ');
    FOut.PutInt(EI.GetDstNId());
    FOut.PutCh(' ');
    FOut.PutInt(EI.GetDat());
    FOut.PutLn();
  }
}

void generateNeg(PWNet Net, std::vector<std::vector<int>> & negLinks, std::set<std::pair<int, int>> & seen, int numNeg, int numPos){
  while(numNeg < numPos){
    int node1 = Net->GetRndNId();
    int node2 = Net->GetRndNId();
    int temp;

    if(node1 > node2){
      temp = node1;
      node1 = node2;
      node2 = temp;
    }

    std::pair <int, int> query = std::make_pair(node1, node2);
    if(!(Net->IsEdge(node1,node2)) && (seen.find(query) == seen.end()) && node1 != node2){
      std::vector<int> negLink;
      negLink.push_back(query.first);
      negLink.push_back(query.second);
      negLink.push_back(0);
      
      negLinks.push_back(negLink);
      numNeg++;
      seen[query] = 0;
    }
  }
}

void generatePos(PWNet Net, std::vector<std::vector<int>> & posRemovable, 
  std::vector<std::vector<int>> & posNonRemovable){

  for(TWNet::TObj::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++){
    int source = EI.GetSrcNId();
    int dest = EI.GetDstNId();
    double weight = EI.GetDat();

    std::vector<int> l;
    l.push_back(source);
    l.push_back(dest);
    l.push_back(EI.GetDat());
    l.push_back(1);

    if(Net->GetNI(source).GetDeg() > 1 && Net->GetNI(dest).GetDeg() > 1){
      posRemovable.push_back(l);
    }else{
      posNonRemovable.push_back(l);
    }

  }
}