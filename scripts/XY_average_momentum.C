#include "useful_for_root/cuts.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * XY distribution of average momentum for all charged particles and muons 
//      * XY distribution of the number muons and the number of charged particles
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void XY_average_momentum(const int& n_files, // number of file roots
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
    if (n_files > 1) cout << "WARNING: you probably don't want to use this with more than one file" << endl;
    const unsigned int n_funcs = 4; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"xy_summed_muon_momentum_pre_deg", 
        "xy_summed_charged_momentum_pre_deg","xy_summed_muons_pre_deg", "xy_summed_charged_pre_deg"};

    // the functions applied to every hit
    entry_fptr cuts[n_funcs] = {&xy_sum_mu_mom_pre_deg,
        &xy_sum_charged_mom_pre_deg, &xy_sum_muons_pre_deg, &xy_sum_charged_pre_deg};

    TH2F* hists[n_files][n_funcs]; // array to hold the produced histograms

    const TString axis_titles [n_funcs*3] = 
        {"position X (mm)", "position Y (mm)", "Summed Momentum (MeV/c)", // for muons
        "position X (mm)", "position Y (mm)", "Summed Momentum (MeV/c)", // all charged
        "position X (mm)", "position Y (mm)", "muon count",  // count muons
        "position X (mm)", "position Y (mm)", "charged_count"}; // count charged
    const int    bins [n_funcs*3] = {  50,  50, 50,  50,  50, 50,  50,  50, 50,  50,  50, 50}; 
    const double mins [n_funcs*3] = {-200,-200,  0,-200,-200,  0,-200,-200,  0,-200,-200,  0};
    const double maxs [n_funcs*3] = { 200, 200,200, 200, 200,200, 200, 200,200, 200, 200,200};  

    const int dimension = 2;
    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    fill_hists<TH2F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, dimension, ".root", 
        axis_titles, bins, mins, maxs, testing, testing);

    const unsigned int n_derived_hists = 2; 
    TH2F* derived_hists[n_files][n_derived_hists];
    // derived hist [0] = average muon momentum
    // derived hist [1] = average charged particle momentum

    for(unsigned int file = 0; file < n_files; ++file) {
        // create and draw the 'averaged' histograms
        // plot;
        //      1) average momentum muon
        //      2) average momentum all charged particles
        //      3) number of muons
        //      4) number of charged particles

        derived_hists [file][0] = init_2Dhist ("XY distribution of average muon momentum",
            50,-200, 200, 50, -200, 200, // number of bins, min & max for x then y
            "X Position (mm)", "Y Position (mm)");
        derived_hists [file][0]->Divide(hists[file][0], hists[file][2]);

        derived_hists [file][1] = init_2Dhist ("XY distribution of average momentum for charged particles",
            50,-200, 200, 50, -200, 200, // number of bins, min & max for x then y
            "X Position (mm)", "Y Position (mm)");
        derived_hists [file][1]->Divide(hists[file][1], hists[file][3]);

        // image file names: blank if testing
        const TString img_locn_1 = (testing) ? "" : img_prefix + "_averaged_muon_momentum.eps";
        const TString img_locn_2 = (testing) ? "" : img_prefix + "_averaged_charged_particle_momentum.eps";
        const TString img_locn_3 = (testing) ? "" : img_prefix + "_number_of_muons.eps";
        const TString img_locn_4 = (testing) ? "" : img_prefix + "_number_of_charged_particles.eps";
        // "" to use default titles
        draw_pretty_hist(derived_hists[file][0], "", img_locn_1, "COLZ"); // µ ave mom
        draw_pretty_hist(derived_hists[file][1], "", img_locn_2, "COLZ"); // charged ave mom
        draw_pretty_hist(hists[file][2], "", img_locn_3, "COLZ");         // n µ
        draw_pretty_hist(hists[file][3], "", img_locn_4, "COLZ");         // n charged
    }
}   
// sum the muon momentums at the degrader
void xy_sum_mu_mom_pre_deg     (const in_branch_struct& branch, const TH2F* hist, const bool verbose){
    xy_sum_at(branch, hist, &first_deg_muon, &get_momentum, verbose);
}
// count the muons at the degrader
void xy_sum_muons_pre_deg      (const in_branch_struct& branch, const TH2F* hist, const bool verbose){
    xy_sum_at(branch, hist, &first_deg_muon, &get_one, verbose);
}
// sum the momentums of all charged particles at the degrader
void xy_sum_charged_mom_pre_deg(const in_branch_struct& branch, const TH2F* hist, const bool verbose){
    xy_sum_at(branch, hist, &first_deg_charged, &get_momentum, verbose);
}
// count all charged particles at the degrader
void xy_sum_charged_pre_deg    (const in_branch_struct& branch, const TH2F* hist, const bool verbose){
    xy_sum_at(branch, hist, &first_deg_charged, &get_one, verbose);
}


