#include "edep_vs_p0.C"

void edep_vs_p0_top(){
    // const unsigned int n_files = 4;
    const unsigned int n_files = 8;
    // const unsigned int n_files = 1;

    // const TString file_roots [n_files] = {"st_1mm_deg_10mm"};
    const TString file_roots [n_files] = {"Air_5mm",
        "st_1mm_deg_1mm" ,
        "st_1mm_deg_2mm" ,
        "st_1mm_deg_5mm" ,
        "st_1mm_deg_8mm" ,
        "st_1mm_deg_10mm",
        "st_1mm_deg_12mm",
        "st_1mm_deg_15mm"};
        // const TString file_roots [n_files] = {"Air_5mm",
        //     "st_1mm_deg_2mm" ,
        //     "st_1mm_deg_8mm" ,
        //     "st_1mm_deg_12mm"};

    const TString file_prefix = "../../output/st_and_degrader/st_and_degrader_";
    const TString img_prefix = "../../../images/st_and_degrader/edep_vs_p0_";
    const TString save_file_name = "edep_vs_p0_all";

    edep_vs_p0(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



