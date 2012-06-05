#include "edep_in_degrader.C"

void st_and_degrader_edep_old(){
    const unsigned int n_files = 1;
    // const unsigned int n_files = 17;

    const TString file_roots [n_files] = {"Aluminium_10mm"};
    // const TString file_roots [n_files] = { "st_1mm_1mm" ,
    //     "st_1mm_5mm" ,
    //     "st_1mm_10mm",
    //     "st_1mm_15mm",
    //     // st 5mm
    //     "st_5mm_1mm" ,
    //     "st_5mm_5mm" ,
    //     "st_5mm_10mm",
    //     "st_5mm_15mm",
    //     // st 10mm
    //     "st_10mm_1mm" ,
    //     "st_10mm_5mm" ,
    //     "st_10mm_10mm",
    //     "st_10mm_15mm",
    //     // st 15mm
    //     "st_15mm_1mm" ,
    //     "st_15mm_5mm" ,
    //     "st_15mm_10mm",
    //     "st_15mm_15mm"};

    const TString file_prefix = "../../output/archive/04-06-12_1819.baaa595/degrader_optimisation/degrader_optimisation_";
    const TString img_prefix = "../../../images/st_and_degrader/old_";
    const TString save_file_name = "saved_hist_bk";

    edep_in_degrader(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



