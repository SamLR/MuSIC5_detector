#include "stopped_and_degrader_effect.C"

void st_and_degrader_comparison(){
    const unsigned int n_files = 16;

    const TString air_root = "Air_5mm"; // background comparison for scint 1
    const TString file_roots [n_files] = { "st_10mm_10mm",
        "st_10mm_15mm",
        "st_10mm_1mm",
        "st_10mm_5mm",
        "st_15mm_10mm",
        "st_15mm_15mm",
        "st_15mm_1mm",
        "st_15mm_5mm",
        "st_1mm_10mm",
        "st_1mm_15mm",
        "st_1mm_1mm",
        "st_1mm_5mm",
        "st_5mm_10mm",
        "st_5mm_15mm",
        "st_5mm_1mm",
        "st_5mm_5mm"};

    const TString file_prefix = "../../output/st_and_degrader/st_and_degrader_";
    const TString img_prefix = "../../../images/st_and_degrader/";
    const TString save_root = "saved_hist";

    const unsigned int n_funcs = 3;
    cut_func_ptr cuts[n_funcs] = {&stopped_muons,
        &not_stopped_muons,
        &all_scint1_muons};                     
    cut_func_ptr air_cut[] = {&all_scint1_muons};

    do_it_all(n_files, air_root, file_prefix, img_prefix, 
        save_root, file_roots, n_funcs, cuts, air_cut);

}



