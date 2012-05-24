#include "stopped_and_degrader_effect.C"

void degrader_comparison(){
    const unsigned int n_files = 3;

     const TString air_root = "Air_5mm"; // background comparison for scint 1
     // const TString file_roots [n_files] = {"Aluminium_20mm",
     //     "Aluminium_50mm",
     //     "Aluminium_100mm"};
     const TString file_roots [n_files] = {"Aluminium_0.2mm",
         "Aluminium_10mm",
         "Aluminium_1mm",
         "Aluminium_2mm",
         "Aluminium_5mm"};
     //     "Polystyrene_0.2mm",
     //     "Polystyrene_10mm",
     //     "Polystyrene_1mm",
     //     "Polystyrene_2mm",
     //     "Polystyrene_5mm"};
     const TString file_prefix = "../../output/degrader_optimisation/degrader_optimisation_";
     const TString img_prefix = "../../../images/degrader_optimisation/";
     const TString save_root = "saved_hist_ST";

     const unsigned int n_funcs = 3;
     cut_func_ptr cuts[n_funcs] = {&stopped_muons,
         &not_stopped_muons,
         &all_scint1_muons};                     
     cut_func_ptr air_cut[] = {&all_scint1_muons};
         
     do_it_all(n_files, air_root, file_prefix, img_prefix, 
         save_root, file_roots, n_funcs, cuts, air_cut);

}



