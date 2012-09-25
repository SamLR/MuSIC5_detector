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
    const unsigned int n_funcs = 3; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"all_muon_edep_in_scint1", "all_muon_edep_in_scint2",
        "Stopped_muon_edep_in_scint1"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&edep_in_scint1_of_muons,
         &edep_in_scint2_of_muons,
         &edep_in_scint1_of_stopped_muons};

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    const TString axis_titles [n_funcs*3] = {"Edep (MeV)", "count", "Z","Edep (MeV)", "count", "Z","Edep (MeV)", "count", "Z"};
    const int    bins [n_funcs*3] = { 100, 100, 100,100, 100, 100,100, 100, 100};//  100, 100, 100}; 
    const double mins [n_funcs*3] = {   0,   0,   0,  0,   0,   0,  0,   0,   0};//    0,   0,   0};
    const double maxs [n_funcs*3] = {  50,  50,  50, 50,  50,  50, 50,  50,  50};// 0.05,0.05,0.05};  

    const int dimension = 1;
    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root", 
        axis_titles, bins, mins, maxs, testing, testing);

    // loop over all the file roots and draw the associated histograms
        // compare the momentum distributions based on PID

    TH1F* a_hists[n_funcs][n_files];
    for(unsigned int file = 0; file < n_files; ++file) {        
        for(unsigned int func = 0; func < n_funcs; ++func) {
            a_hists[func][file] = hists[file][func];
        }
    }
    
    for(unsigned int i = 0; i < n_funcs; ++i) {
        TString img_save_location;
        if (testing){
            img_save_location = "";
        } else {
            img_save_location = img_prefix + func_names[i];
        }
        
        TString title = func_names[i];// +" "+ st_name;
        
        draw_pretty_hists(n_files, &(a_hists[i][0]), title, file_roots, img_save_location,1002201);
    }
}

void edep_in_scint1_of_muons(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &muon_scint1);
}

void edep_in_scint2_of_muons(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, &muon_scint2);
}

void edep_in_scint1_of_stopped_muons(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum_flagged(branch, hist, &muon_scint1, &mu_decaying_in_ST,verbose);
}



