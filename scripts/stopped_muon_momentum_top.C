#include "stopped_muon_momentum.C"

void stopped_muon_momentum_top(){
    const unsigned int n_files = 6;
    // const unsigned int n_files = 2;
        
    // const TString file_roots [n_files] = {"1mm","12mm"};
    const TString file_roots [n_files] = {"1mm","2mm","5mm","8mm","10mm","12mm"};

    const int n_targets = 3;
    const TString stopping_targets [n_targets] = { "Copper_0.5mm", "Magnesium_2.5mm", "Magnesium_5mm"};   
    const TString deg = "deg_Al";

    for(unsigned int i = 0; i < n_targets; ++i) {
        const TString img_prefix = "../../../images/realistic/initial_mom_of_muons_stopped_in_"+stopping_targets[i];
        const TString save_file_name = "init_mom_of_muons_stopped_in_"+stopping_targets[i]+"_"+deg+"_1_2_5_8_10_12mm";
        const TString file_prefix = "../../output/realistic/realistic_st_"+stopping_targets[i]+"_" + deg + "_";
        stopped_muon_momentum(n_files, file_roots, file_prefix, img_prefix, save_file_name, stopping_targets[i]);
    }
}



