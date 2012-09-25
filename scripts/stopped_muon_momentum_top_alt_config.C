#include "stopped_muon_momentum.C"

void stopped_muon_momentum_top_alt_config(){    
	
	
    const unsigned int n_files = 6;
    const TString file_roots [n_files] ={"0.25mm_deg_Al_0mm", 
        "0.50mm_deg_Al_0mm", 
        "0.75mm_deg_Al_0mm",
        "1mm_deg_Al_0mm", 
        "1.25mm_deg_Al_0mm", 
        "1.5mm_deg_Al_0mm"};         

    const TString runname = "final_alt";
    
    const TString img_prefix = "../../../images/"+runname+"/no_degrader_misc_thickness_Mg_target_";
    const TString save_file_name = "muon_edep_with_no_degrader_and_Mg_target";

    const TString file_prefix = "../../output/"+runname+"/"+runname+"_st_Magnesium_";
    stopped_muon_momentum(n_files, file_roots, file_prefix, img_prefix, save_file_name, "Magnesium");
	
 }



