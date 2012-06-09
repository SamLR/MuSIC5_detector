// #include "useful_for_root/music_root.C"
#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * The number of muons in scint 1 with the number of muons at the same
//        place for the first file (i.e. the first file root is used as a basis)
//      * The number of stopped muons Vs the number of muons that don't stop
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void muon_edep_in_degrader_charge(const int& n_files, // number of file roots
    // basic elements of names differentiateing the files
    const TString* file_roots,                       
    // file prefix, mainly the directory but also any other common element
    const TString& file_prefix,                      
    // as above but for the saved images
    const TString& img_prefix,          
    // root to use for the root file of histograms             
    const TString& save_file_name,
    // reduces the number of entries read and stops stuff getting saved
    const bool& testing = false       
){
    const unsigned int n_funcs = 2; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"mu_plus_edep_in_degrader_","mu_minus_edep_in_degrader_"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&deg_edep_muon_plus, // energy deposited in the degrader by µ+
            &deg_edep_muon_minus};                     // energy deposited in the degrader by µ-

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    const TString axis_titles [n_funcs*3] = {"Energy (MeV)", "Count", "Z", 
        "Energy (MeV)", "Count", "Z"};
    const int       bins [n_funcs*3] = {100, 100, 100,100, 100, 100,};
    const double    mins [n_funcs*3] = {0,   0,   0,  0,   0,   0,  };
    const double    maxs [n_funcs*3] = {50,  50,  50, 50,  50,  50, };
    
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, 1, ".root", ".eps",
        axis_titles, bins, mins, maxs, testing, testing);


    // loop over all the file roots and draw the associated histograms
    for(unsigned int file = 0; file < n_files; ++file) {
        // compare edep based on charge
        TString img_location;
        if (testing){
            img_location = "";
        } else {        
            img_location = img_prefix + file_roots[file] + "_charge_based_edep_comparison.eps";
        }
        TString h_title = "energy deposited by positive & negative muons in a degrader file: st_"+file_roots[file];
        draw_pretty_two_hists(hists[file][0], hists[file][1], h_title, 
            func_names[0], func_names[1], img_location);
    }
}


void deg_edep_muon(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &muon_deg);
}
void deg_edep_muon_plus(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &mu_plus_deg);
}
void deg_edep_muon_minus(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &mu_minus_deg);
}

void mom_at_pre_deg_muon_parent(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &muon_deg_no_parent);
}
void mom_at_post_deg_muon_parent(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &muon_scint1_no_parent);
}
