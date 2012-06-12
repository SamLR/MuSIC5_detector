#include "stopped_muon_momentum.C"

void stopped_muon_momentum_top(){
    const unsigned int n_files = 6;
    // const unsigned int n_files = 4;
    // const unsigned int n_files = 8;
    // const unsigned int n_files = 1;
    // const unsigned int n_files = 13;

    // const TString file_roots [n_files] = {"st_1mm_deg_10mm"};
        // const TString file_roots [n_files] = {"Air_5mm",
        //     "st_1mm_deg_1mm" ,
        //     "st_1mm_deg_2mm" ,
        //     "st_1mm_deg_5mm" ,
        //     "st_1mm_deg_8mm" ,
        //     "st_1mm_deg_10mm",
        //     "st_1mm_deg_12mm",
        //     "st_1mm_deg_15mm"};     
        // const TString file_roots [n_files] = {"Air_5mm",
        //     "st_1mm_deg_2mm" ,
        //     "st_1mm_deg_8mm" ,
        //     "st_1mm_deg_12mm"};  

    // This one works well
    // const TString file_roots [n_files] = {"Copper_2mm",
    //     "Copper_8mm",
    //     "Magnesium_2mm",
    //     "Magnesium_8mm"};  
            // this is not a good idea - don't have enough colours and plot is unreadable
            // const TString file_roots [n_files] = {"Air_5mm",
            //     "Copper_0.2mm",
            //     "Copper_1mm",
            //     "Copper_2mm",
            //     "Copper_5mm",
            //     "Copper_8mm",  
            //     "Copper_10mm",
            //     "Magnesium_0.2mm",
            //     "Magnesium_1mm",
            //     "Magnesium_2mm",
            //     "Magnesium_5mm",
            //     "Magnesium_8mm",
            //     "Magnesium_10mm"};
        
    const TString file_roots [n_files] = {"deg_1mm",
        "deg_2mm",
        "deg_5mm",
        "deg_8mm",
        "deg_10mm",
        "deg_12mm"};

    // const TString file_prefix = "../../output/ST_optimisation/ST_optimisation_";
    const TString stopping_target = "Copper_1mm";
    const TString file_prefix = "../../output/st_and_degrader/st_and_degrader_st_"+stopping_target+"_";
    const TString img_prefix = "../../../images/st_and_degrader/init_mom_stopped_mu_ST_cu_1mm_range_of_deg";
    const TString save_file_name = "init_mom_stopped_mu_cu_1mm_degrader_1_2_5_8_10_12mm";

    stopped_muon_momentum(n_files, file_roots, file_prefix, img_prefix, save_file_name);
}



