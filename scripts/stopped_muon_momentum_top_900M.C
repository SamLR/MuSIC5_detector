#include "stopped_muon_momentum.C"

void stopped_muon_momentum_top_900M(){ 
    const unsigned int n_files = 4;
    const TString file_roots [n_files] ={"Air_5mm", 
                                         "Aluminium_0.5mm", 
                                         "Aluminium_1mm", 
                                         "Aluminium_5mm"};        // 
    const TString runname = "final_900M";
    
    const TString img_prefix = "images/";
    const TString save_file_name = "stopped_muon_momentum";
    const TString file_prefix = "../../output/final_900M/final_st_Copper_0.5mm_deg_";
    stopped_muon_momentum(n_files, file_roots, file_prefix, img_prefix, save_file_name, "");
}



