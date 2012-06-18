#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

// This plots the tof recorded for various data in 3 forms:
//      * Truth: muons and electrons only
//      * Any decay: µ->e and π->e (other chains ignored)
//      * all: arbitary time difference between first particle passing some
//        cuts (e.g. in scint 1, no hit in scint 2 < 10ns) any subsequent hit
//        in scint 1 or 2
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void tof_calculation(const int& n_files, // number of file roots
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
    const TString func_names[n_funcs] = {"muon decay times", "pion and muon decay times"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&muon_tof, &charged_tof};

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    const TString axis_titles [n_funcs*3] = {"Time of flight (ns)", "Count", "",
        "Time of flight (ns)", "Count", ""};
    const int    bins [n_funcs*3] = {  50,  50,  50,  50,  50,  50,}; 
    const double mins [n_funcs*3] = {   0,   0,   0,   0,   0,   0,};
    const double maxs [n_funcs*3] = {2000,2000,2000,2000,2000,2000,};  

    const int dimension = 1;
    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root", 
        axis_titles, bins, mins, maxs, testing, testing);

    // loop over all the file roots and draw the associated histograms
    for(unsigned int file = 0; file < n_files; ++file) {
     
        TString img_save_location1;
        TString img_save_location2;
        if (testing){
            img_save_location1 = "";
            img_save_location2 = "";
        } else {
            img_save_location1 = img_prefix + "muons_"+ file_roots[file]+ ".eps";
            img_save_location2 = img_prefix + "charged_"+ file_roots[file]+ ".eps";
        }
        TString h_title1 = "Muon decay time";
        draw_pretty_hist(hists[file][0], h_title1, img_save_location1);
        TString h_title2 = "Muon & pion decay times";
        draw_pretty_hist(hists[file][1], h_title2, img_save_location2);
    }
}


void muon_tof(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    tof_parent_daughter(branch, hist, &first_scint1_muon, &e_first_in_scint1_or_2, verbose);
}

void charged_tof(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    tof_parent_daughter(branch, hist, &first_scint1_mu_or_pi, &e_or_mu_first_in_scint1_or_2, verbose);
}





