
#include "useful_for_root/looping.C"

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
    int procid[500];
    int counter[500];
    int trkid[500];
    int parentid[500];
    int pdgid[500];
    double x[500];
    double y[500];
    double z[500];
    double px[500];
    double py[500];
    double pz[500];
    double edep[500];
    double tof[500];
};

void do_it_all2D(const unsigned int& n_files,
    const TString& file_prefix,
    const TString& save_root,
    const TString* file_roots,
    const unsigned int& n_funcs,
    cut_func_ptr cuts,
    const TString* hist_names,
    TH2F* hists[n_files][n_funcs],
    const TString& file_suffix=".root",
    const TString xtitle = "Momentum at degrader (MeV)",
    const TString ytitle = "Momentum at stopping target (MeV)"
){
    TString out_file_name = file_prefix + save_root + file_suffix;
    TFile* out_file = init_file(out_file_name, "RECREATE");
    // array of function pointers to use

    for(unsigned int file_no = 0; file_no < n_files; ++file_no) {
        cout << endl<< "Starting file "<< file_roots[file_no] << endl;
        // munge the filename
        TString resolved_filename = file_prefix + file_roots[file_no] + file_suffix;
        
        // open up and initialise things
        in_file = init_file(resolved_filename);
        in_branch_struct branch;
        in_tree = init_tree<in_branch_struct>(in_file, "t", branch, &set_in_branch_address);

        // create the histograms then fill them in the loop
        out_file->cd(); // make sure they're added to the save file
        for(unsigned int i = 0; i < n_funcs; ++i) {
            TString name = hist_names[i] + file_roots[file_no]; // munge munge munge
            hists[file_no][i] = init_2Dhist (name, 50, 0, 200,50, 0, 200, xtitle, ytitle);   
        }
        loop_entries<in_branch_struct, TH2F*>(in_tree, branch, hists[file_no], n_funcs, cuts, true);
    }
    out_file->Write();

    cout << "It... is done." << endl;
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
