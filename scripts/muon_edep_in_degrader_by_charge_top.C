#include "muon_edep_in_degrader_charge.C"

void  muon_edep_in_degrader_by_charge_top(){
    const unsigned int n_files = 7;

    const TString file_roots [n_files] = {"1mm_deg_1mm" ,
        "1mm_deg_2mm" ,
        "1mm_deg_5mm" ,
        "1mm_deg_8mm" ,
        "1mm_deg_10mm",
        "1mm_deg_12mm",
        "1mm_deg_15mm"};

    const TString file_prefix = "../../output/st_and_degrader/st_and_degrader_st_";
    const TString img_prefix = "../../../images/st_and_degrader/";
    // const TString save_file_name = "";
    const TString save_file_name = "saved_hist_charged_based_muon_edep";

    muon_edep_in_degrader_by_charge(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



