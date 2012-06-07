#include "edep_in_degrader.C"

void st_and_degrader_edep(){
    // const unsigned int n_files = 2;
    // const unsigned int n_files = 17;
    const unsigned int n_files = 23;

    // const TString file_roots [n_files] = { "1mm_deg_1mm" ,
    //     "10mm_deg_10mm"};
    // const TString file_roots [n_files] = { "1mm_deg_1mm" ,
    //      "1mm_deg_5mm" ,
    //      "1mm_deg_10mm",
    //      "1mm_deg_15mm",
    //      // st 5mm
    //      "5mm_deg_1mm" ,
    //      "5mm_deg_5mm" ,
    //      "5mm_deg_10mm",
    //      "5mm_deg_15mm",
    //     // st 10mm
    //     "10mm_deg_1mm" ,
    //     "10mm_deg_5mm" ,
    //     "10mm_deg_10mm",
    //     "10mm_deg_15mm",
    //     // st 15mm
    //     "15mm_deg_1mm" ,
    //     "15mm_deg_5mm" ,
    //     "15mm_deg_10mm",
    //     "15mm_deg_15mm"};
    const TString file_roots [n_files] = {"1mm_deg_1mm" ,        
        "1mm_deg_15mm",
        // st 2 mm
        "2mm_deg_1mm" ,
        "2mm_deg_2mm" ,
        "2mm_deg_5mm" ,
        "2mm_deg_8mm" ,
        "2mm_deg_10mm",
        "2mm_deg_12mm",
        "2mm_deg_15mm",
        // st 5mm
        "5mm_deg_1mm" ,
        "5mm_deg_2mm" ,
        "5mm_deg_5mm" ,
        "5mm_deg_8mm" ,
        "5mm_deg_10mm",
        "5mm_deg_12mm",
        "5mm_deg_15mm",
        // st 10 mm
        "10mm_deg_1mm" ,
        "10mm_deg_2mm" ,
        "10mm_deg_5mm" ,
        "10mm_deg_8mm" ,
        "10mm_deg_10mm",
        "10mm_deg_12mm",
        "10mm_deg_15mm"};

    // const TString file_prefix = "../../output/archive/06-06-12_1613.00f4e4f/st_and_degrader/st_and_degrader_st_";
    const TString file_prefix = "../../output/st_and_degrader/st_and_degrader_st_";
    const TString img_prefix = "../../../images/st_and_degrader/";
    // const TString save_file_name = "";
    const TString save_file_name = "saved_hist";

    edep_in_degrader(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



