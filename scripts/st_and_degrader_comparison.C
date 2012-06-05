#include "comparisons_stopped_and_air.C"

void st_and_degrader_comparison(){
    const unsigned int n_files = 17;

    const TString file_roots [n_files] = { "Air_5mm",
        "st_10mm_10mm",
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
    const TString save_file_name = "saved_hist";

    comparisons_stopped_and_air(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



