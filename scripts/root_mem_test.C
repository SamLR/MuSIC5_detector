// 
//  root_mem_test.C
//  Test to see if ROOT works as hoped WRT writing TTrees that exceed 
//  maximum size or if excess files are only written on `to_write()`
//  
//  Created by Sam Cook on 2013-06-18.
// 

#include "TTree.h"
#include "TFile.h"

void root_mem_test(){
  TFile* file = new TFile("mem_test.root", "RECREATE");
  TTree* tree = new TTree("t", "t");
  Long64_t to_write; // 64b integer, i.e. 8B
  
  tree->Branch("Filler", &to_write);
  
  tree->SetMaxTreeSize(64L); // in bytes, i.e. can store 8 values
  
  for(Long64_t i = 0; i < 16; ++i) {
    to_write = i;
    tree->Fill();
  }
}