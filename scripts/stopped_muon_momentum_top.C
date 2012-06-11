#include "stopped_muon_momentum.C"

void stopped_muon_momentum_top(){
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
    const TString img_prefix = "../../../images/st_and_degrader/init_mom_stopped_mu_all";
    const TString save_file_name = "init_mom_stopped_mu_all";

    stopped_muon_momentum(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



