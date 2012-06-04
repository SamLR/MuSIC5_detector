#include "useful.C"
#include "music_root_useful.C"

void analyse(const unsigned int& n_files,
    const TString& air_root,
    const TString& file_prefix,
    const TString& img_prefix,
    const TString& save_root,
    const TString* file_roots,
    const TString* func_name_roots,
    const unsigned int& n_funcs,
    const TH1F* hists[n_files][n_funcs],
    cut_func_ptr cuts,
    cut_func_ptr air_cut,
    const TString& file_suffix=".root",
    const TString& img_suffix=".eps",
    const TString xtitle = "Momentum (MeV)",
    const TString ytitle = "Count",
    const int n_bins = 50, 
    const int x_min = 0, 
    const int x_max = 200
){
    // root file to save things in
    TString out_file_name = file_prefix + save_root + file_suffix;
    TFile* out_file = init_file(out_file_name, "RECREATE");
    // branch to address to
    in_branch_struct branch;
        
    for(unsigned int file_no = 0; file_no < n_files; ++file_no) {
        TH1F** hist_set = hists[file_no]; // alias to the set of hists we want
        cout << endl<< "Starting file "<< file_roots[file_no] << endl;
        // munge the filename
        TString resolved_filename = file_prefix + file_roots[file_no] + file_suffix;
        cout << resolved_filename << endl;
        // open up and initialise things
        in_file = init_file(resolved_filename);
        in_tree = init_tree<in_branch_struct>(in_file, "t", branch, &set_in_branch_address);
        
        // create the histograms then fill them in the loop
        out_file->cd(); // make sure they're added to the save file
        for(unsigned int func = 0; func < n_funcs; ++func) {
            // initialise the histograms
            TString name = func_name_roots[func] + file_roots[file_no];
            hist_set[func] = init_1Dhist(name, n_bins, x_min, x_max, xtitle, ytitle);
        }
        // loop-de-loop        
        loop_entries<in_branch_struct, TH1F*>(in_tree, branch, hist_set, n_funcs, cuts, true);
    }
    cout << "out_file not enabled. Uncomment it, please" << endl;
    // out_file->Write();
    cout << "It... is done." << endl;
}
