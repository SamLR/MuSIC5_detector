#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * The number of muons in scint 1 with the number of muons at the same
//        place for the first file (i.e. the first file root is used as a basis)
//      * The number of stopped muons Vs the number of muons that don't stop
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void pid_counter_at_post_degrader(const int& n_files, // number of file roots
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
    const unsigned int n_funcs = 1; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"pid_count"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&pid_count_at_scint1};

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    const TString axis_titles [n_funcs*3] = {"Particle", "count", "Z"};
    const int    bins [n_funcs*3] = {      __n_pids,       __n_pids,       __n_pids}; // only 8 options pid 1-7 or 0, unknown
    const double mins [n_funcs*3] = {        -0.5,         -0.5,         -0.5};  
    const double maxs [n_funcs*3] = { -0.5+__n_pids,  -0.5+__n_pids,  -0.5+__n_pids};  

    const int dimension = 1;
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root",
        axis_titles, bins, mins, maxs, testing, testing);

    // loop over all the file roots and draw the associated histograms
    for(unsigned int file = 0; file < n_files; ++file) {
        for(unsigned int apid = 0; apid < __n_pids; ++apid) {
            const TString bin_name = get_name_from_apid(apid);
            const int bin_number = hists[file][0]->FindBin(apid);
            hists[file][0]->GetXaxis()->SetBinLabel(bin_number, bin_name);
        }
    }
    const TString title = "PID counts at scint 1 for various thickness degraders";
    const TString img_save_location = (testing) ? "": img_prefix + ".eps";
    draw_pretty_hists(n_files,hists[0],title,file_roots,img_save_location);
}
void pid_count_at_scint1(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    particle_type_counter(branch, hist, &first_in_scint1);
}



