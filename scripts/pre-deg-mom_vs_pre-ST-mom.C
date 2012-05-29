#include "2d_plotting.C"

void pre_deg_mom_vs_pre_ST_mom(){         
    const unsigned int n_files = 6;

    const TString file_roots [n_files] = { "Air_5mm",
        "Aluminium_0.2mm",
        "Aluminium_10mm",
        "Aluminium_1mm",
        "Aluminium_2mm",
        "Aluminium_5mm"};

    const TString file_prefix = "../../output/degrader_optimisation/degrader_optimisation_";
    const TString img_prefix = "../../../images/degrader_optimisation_2d_plots/";
    const TString save_root = "saved_hist";
    
    const unsigned int n_funcs = 1;
    cut_func_ptr cuts[n_funcs] = {&preDeg_vs_preST_muons};
    const TString hist_titles[n_funcs] = {"Comparison of momentums at degrader with stopping target"};
    const TString hist_names[n_funcs] = {"dev_vs_ST_"};

    do_it_all2D(n_files, file_prefix, img_prefix, save_root, file_roots, 
        n_funcs, cuts, hist_titles, hist_names);
    // do_it_all2D(n_files, file_prefix, img_prefix, 
    //     save_root, file_roots, n_funcs, cuts);

}



