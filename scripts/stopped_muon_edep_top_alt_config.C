#include "stopped_muon_edep.C"

void stopped_muon_edep_top(){    
    
    const unsigned int n_files = 6;
    const TString file_roots [n_files] ={"0.25mm_deg_Al_0mm", 
        "0.50mm_deg_Al_0mm", 
        "0.75mm_deg_Al_0mm",
        "1mm_deg_Al_0mm", 
        "1.25mm_deg_Al_0mm", 
        "1.5mm_deg_Al_0mm"};         

    const TString runname = "final_alt";
    
    const TString img_prefix = "../../../images/"+runname+"/st_"+stopping_targets[i]+"_"+deg+"_0.5_1_5_8_12mm_";
    const TString save_file_name = "edep_of_muons_stopped_in_"+stopping_targets[i]+"_"+deg+"_0.5_1_5_8_12mm";

    const TString file_prefix = "../../output/"+runname+"/"+runname+"_st_";
    stopped_muon_edep(n_files, file_roots, file_prefix, img_prefix, save_file_name, stopping_targets[i]);

}



