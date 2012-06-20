#include "stopped_muon_edep.C"

void stopped_muon_edep_top(){    
    // const unsigned int n_files =2;    
    // const TString file_roots [n_files] = {"1mm","12mm"} ;

    const unsigned int n_files = 5;
    const TString file_roots [n_files] ={"0.5mm", "1mm", "5mm", "8mm", "12mm"};        // 
                // const unsigned int n_files = 6;
                // const TString file_roots [n_files] ={"1mm","2mm","5mm","8mm","10mm","12mm"};

    const int n_targets = 1;
    const TString stopping_targets [n_targets] = { "Copper_0.5mm"};//, "Magnesium_2.5mm", "Magnesium_5mm"};   
    const TString deg = "deg_Al";
    const TString runname = "final";
    
    for(unsigned int i = 0; i < n_targets; ++i) {
        
        const TString img_prefix = "../../../images/"+runname+"/st_"+stopping_targets[i]+"_"+deg+"_0.5_1_5_8_12mm_";
        const TString save_file_name = "edep_of_muons_stopped_in_"+stopping_targets[i]+"_"+deg+"_0.5_1_5_8_12mm";
        const TString file_prefix = "../../output/"+runname+"/"+runname+"_st_"+stopping_targets[i]+"_" + deg + "_";
        stopped_muon_edep(n_files, file_roots, file_prefix, img_prefix, save_file_name, stopping_targets[i]);
    }
}



