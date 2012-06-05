#include "comparisons_stopped_and_air.C"

void degrader_comparison2(){
    const unsigned int n_files = 6;

    const TString file_roots [n_files] = {"Air_5mm",
        "Aluminium_0.2mm",
        "Aluminium_10mm",
        "Aluminium_1mm",
        "Aluminium_2mm",
        "Aluminium_5mm"};

    const TString file_prefix = "../../output/degrader_optimisation/degrader_optimisation_";
    const TString img_prefix = "../../../images/degrader_optimisation/";

    const TString save_file_name = "saved_hist_deg";
    
    comparisons_stopped_and_air(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}