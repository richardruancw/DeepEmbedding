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
  PGraph G3 = TSnap::LoadEdgeList<PGraph>("small_pins.tsv", 1,2,'\t');

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

        if(Net->IsEdge(n1,n2)){
          Net->GetEDat(n1, n2)+=1;
        }
        else{
          Net->AddEdge(n1,n2,1);
        }
      }
    }
	count++;

  if(count%1000 == 0){
    printf("\rCompressing process: %2lf%%",100*(double)count/(double)numBoards);
    fflush(stdout);}
    }
  std::cout<<"mei you huai ren \n"<<std::endl;
  TFOut FOut("collapsed_unweighted.txt");
  for(TWNet::TEdgeI EI = Net->BegEI(); EI < Net -> EndEI(); EI++){
    FOut.PutInt(EI.GetSrcNId());
    FOut.PutCh(' ');
    FOut.PutInt(EI.GetDstNId());
    FOut.PutCh(' ');
    FOut.PutInt(EI.GetDat());
    FOut.PutLn();
  }
}
