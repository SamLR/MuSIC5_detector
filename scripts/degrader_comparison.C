#include "useful.C"

struct in_branch_struct {
    // structure as passed in from geant4
    int g_iev;
    int in_event;
    int in_trackID;
    int in_pdg;
    double in_x; 
    double in_y; 
    double in_z;
    double in_px;
    double in_py;
    double in_pz;
    double in_tof;
    int g_nhit;
    int procid[MAX_HIT];
    int counter[MAX_HIT];
    int trkid[MAX_HIT];
    int parentid[MAX_HIT];
    int pdgid[MAX_HIT];
    double x[MAX_HIT];
    double y[MAX_HIT];
    double z[MAX_HIT];
    double px[MAX_HIT];
    double py[MAX_HIT];
    double pz[MAX_HIT];
    double edep[MAX_HIT];
    double tof[MAX_HIT];
};

void set_in_branch_address(const in_branch_struct&, const TTree*);

void degrader_comarison(){
    // open file
    // get the tree
    // set branches
    // loop 
    // cut -> muons, at either scint1 or 2
    // categorise either stopped or not
    //  stopped == muon in ST with momentum == 0? 
    // two processes that muons can decay via
    //      'mu- capture @ rest' and 'decay' (numbers 15 & 10 respectively)
    // draw Vs momentum
    // save hists & muon trees
    // same for other charged particles ?
    // look for charged particles 
    TString file_roots [] = { // FIXME files here please!
        
    };
    TString file_prefix = "../../output";
    TString file_suffix = ".root";
    const unsigned int n_files = sizeof(file_roots)/sizeof(TString);
    TH1F* hists [2*n_files]; // one for stopped, one for not
    
    for(unsigned int file_no = 0; file_no < n_files; ++file_no) {
        
        TString resolved_filename = file_prefix + file_roots[file_no] + file_suffix;
        
        TFile* in_file = init_file(resolved_filename);
        in_branch_struct branch;
        TTree* in_tree = init_tree(in_tree, "t", branch, &set_in_branch_address);
        
    }
    
}

void set_in_branch_address(const in_branch_struct& branch, const TTree* tree) { 
    // assign the addresses of the various branchs
    tree->SetBranchAddress("iev",&branch.g_iev);
    tree->SetBranchAddress("in_EventID",&branch.in_event);
    tree->SetBranchAddress("in_TrackID",&branch.in_trackID);
    tree->SetBranchAddress("in_PDGid",&branch.in_pdg);
    tree->SetBranchAddress("in_x",&branch.in_x);
    tree->SetBranchAddress("in_y",&branch.in_y);
    tree->SetBranchAddress("in_z",&branch.in_z); 
    tree->SetBranchAddress("in_Px",&branch.in_px); 
    tree->SetBranchAddress("in_Py",&branch.in_py); 
    tree->SetBranchAddress("in_Pz",&branch.in_pz); 
    tree->SetBranchAddress("in_tof",&branch.in_tof); 

    tree->SetBranchAddress("nhit",&branch.g_nhit);
    tree->SetBranchAddress("procid",&branch.procid);
    tree->SetBranchAddress("counter",&branch.counter);
    tree->SetBranchAddress("trkid",&branch.trkid);
    tree->SetBranchAddress("parentid",&branch.parentid);
    tree->SetBranchAddress("pdgid",&branch.pdgid);
    tree->SetBranchAddress("x",&branch.x);
    tree->SetBranchAddress("y",&branch.y);
    tree->SetBranchAddress("z",&branch.z);
    tree->SetBranchAddress("px",&branch.px);
    tree->SetBranchAddress("py",&branch.py);
    tree->SetBranchAddress("pz",&branch.pz);
    tree->SetBranchAddress("edep",&branch.edep);
    tree->SetBranchAddress("tof",&branch.tof);
}


