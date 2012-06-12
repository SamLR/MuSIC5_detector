#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * The number of muons in scint 1 with the number of muons at the same
//        place for the first file (i.e. the first file root is used as a basis)
//      * The number of stopped muons Vs the number of muons that don't stop
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void momentum_post_degrader_by_pid(const int& n_files, // number of file roots
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
    const unsigned int n_funcs = 5; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"charged", "mu", "e", "pi", "p"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&charged_momentum_post_deg,
        &muon_momentum_post_deg,
        &electron_momentum_post_deg,
        &pion_momentum_post_deg,
        &proton_momentum_post_deg };

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    const TString axis_titles [n_funcs*3] = {"Momentum (MeV/c)", "count", "Z", // x, y, z 
        "Momentum (MeV/c)", "count", "Z",
        "Momentum (MeV/c)", "count", "Z",
        "Momentum (MeV/c)", "count", "Z",
        "Momentum (MeV/c)", "count", "Z"};
    const int       bins [n_funcs*3] = { 50, 50, 50,  50, 50, 50,  50, 50, 50,  50, 50, 50,  50, 50, 50};
    const double    mins [n_funcs*3] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,   0,  0,  0,   0,  0,  0};  
    const double    maxs [n_funcs*3] = {200,200,200, 200,200,200, 200,200,200, 200,200,200, 200,200,200};  

    const int dimension = 1;
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root",
        axis_titles, bins, mins, maxs, testing, testing);


    // loop over all the file roots and draw the associated histograms
    for(unsigned int file = 0; file < n_files; ++file) {
     
        // compare the momentum distributions based on PID
        TString img_save_location;
        if (testing){
            img_save_location = "";
        } else {
            img_save_location = img_prefix + file_roots[file] + ".eps";
        }
        TString h_title = "Momentum at scintillator 1. File: "+file_roots[file];
        draw_pretty_hists(n_funcs, hists[file], h_title, func_names, img_save_location,1002201);
    }
}

void muon_momentum_post_deg(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &first_scint1_muon);
}
void electron_momentum_post_deg(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &first_scint1_e);
}
void pion_momentum_post_deg(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &first_scint1_pion);
}
void proton_momentum_post_deg(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &first_scint1_proton);
}
void charged_momentum_post_deg(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, &first_scint1_charged);
}


