#include "tof_calculation.C"

void tof_calculation_top(){
    const unsigned int n_files = 6;
    // const unsigned int n_files = 1;
    // const TString file_roots [n_files] = {"deg_1mm"}; 
    const TString file_roots [n_files] = {"deg_1mm",
        "deg_2mm",
        "deg_5mm",
        "deg_8mm",
        "deg_10mm",
        "deg_12mm"};

    // const TString file_prefix = "../../output/ST_optimisation/ST_optimisation_";
    const TString stopping_target = "Copper_1mm";
    const TString file_prefix = "../../output/st_and_degrader/st_and_degrader_st_"+stopping_target+"_";
    const TString img_prefix = "../../../images/st_and_degrader/tof_";
    const TString save_file_name = "tof_measurements";

    tof_calculation(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



