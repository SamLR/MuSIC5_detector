#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

//
// plot the edep in scint1 for a variety of thicknesses of degrader 
//

void stopped_muon_edep(const int& n_files, // number of file roots
    // basic elements of names differentiateing the files
    const TString* file_roots,                       
    // file prefix, mainly the directory but also any other common element
    const TString& file_prefix,                      
    // as above but for the saved images
    const TString& img_prefix,          
    // root to use for the root file of histograms             
    const TString& save_file_name,                                     
    // stopping target info
    const TString& st_name,
    // reduces the number of entries read and stops stuff getting saved
    const bool& testing = false       
){               
    const unsigned int n_funcs = 2; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"Stopped_muon_edep_in_scint1", "Stopped_muon_edep_in_scint2"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&edep_in_scint1_of_stopped_muons, &edep_in_scint2_of_stopped_muons};

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    const TString axis_titles [n_funcs*3] = {"Edep (MeV)", "count", "Z","Edep (MeV)", "count", "Z"};
    const int    bins [n_funcs*3] = { 100, 100, 100, 100, 100, 100}; 
    const double mins [n_funcs*3] = {   0,   0,   0,   0,   0,   0};
    const double maxs [n_funcs*3] = {  50,  50,  50,0.05,0.05,0.05};  

    const int dimension = 1;
    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root", 
        axis_titles, bins, mins, maxs, testing, testing);

    // loop over all the file roots and draw the associated histograms
        // compare the momentum distributions based on PID
    TString img_save_location1;
    TString img_save_location2;
    if (testing){
        img_save_location1 = "";
        img_save_location2 = "";
    } else {
        img_save_location1 = img_prefix + "scint1.eps";
        img_save_location2 = img_prefix + "scint2.eps";
    }
    TH1F* a_hists[n_funcs][n_files];
    
    for(unsigned int file = 0; file < n_files; ++file) {
        for(unsigned int func = 0; func < n_funcs; ++func) {
            a_hists[func][file] = hists[file][func];
        }
    }
    TString h_title1 = "Energy deposited by muons in scint1 with a " + st_name +" target and varying thicknesses of degrader";
    draw_pretty_hists(n_files, a_hists[0], h_title1, file_roots,img_save_location1,1002201);
    TString h_title2 = "Energy deposited by muons in scint2 with a " + st_name +" target and varying thicknesses of degrader";
    draw_pretty_hists(n_files, a_hists[1], h_title2, file_roots,img_save_location2,1002201);

}
void edep_in_scint1_of_stopped_muons(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &muon_deg);
    // edep_sum(branch, hist, &muon_scint);
    // edep_sum_flagged(branch, hist, &muon_scint1, &mu_decaying_in_ST,verbose);
}

void edep_in_scint2_of_stopped_muons(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &muon_scint2);
    // edep_sum_flagged(branch, hist, &muon_scint2, &mu_decaying_in_ST,verbose);
}


