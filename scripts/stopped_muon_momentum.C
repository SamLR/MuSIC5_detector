#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * The number of muons in scint 1 with the number of muons at the same
//        place for the first file (i.e. the first file root is used as a basis)
//      * The number of stopped muons Vs the number of muons that don't stop
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void stopped_muon_momentum(const int& n_files, // number of file roots
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
    const unsigned int n_funcs = 1; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"Stopped_muons_initial_momentum"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&stopped_in_ST_mom_in_deg};

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    const TString axis_titles [n_funcs*3] = {"Initial momentum (MeV/c)", "count", "Z"};
    const int    bins [n_funcs*3] = { 50, 50, 50}; 
    const double mins [n_funcs*3] = {  0,  0,  0};
    const double maxs [n_funcs*3] = {200,200,200};  

    const int dimension = 1;
    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root", 
        axis_titles, bins, mins, maxs, testing, testing);

    // loop over all the file roots and draw the associated histograms
        // compare the momentum distributions based on PID
    TString img_save_location;
    if (testing){
        img_save_location = "";
    } else {
        img_save_location = img_prefix + st_name + ".eps";
    }
    TString h_title = "Initial momentum for muons in a " + st_name + " target with varying thicknesses of degrader";
        // TString h_title = "Initial momentum for stopped muons with varying stopping targets";
    draw_pretty_hists(n_files, hists, h_title, file_roots,img_save_location,1002201);

}
void stopped_in_ST_mom_in_deg(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    record_momentum_at_cut1_if_also_cut2(branch, hist, &first_scint1_muon, &mu_decaying_in_ST, verbose);
}



