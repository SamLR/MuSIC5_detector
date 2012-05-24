#include "stopped_and_degrader_effect.C"

void ST_all_particles_stopping(){
    // const unsigned int n_files = 3;
    const unsigned int n_files = 5;

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
    const TString file_prefix = "../../output/archive/22-05-12_1810.9f4f5ac/ST_optimisation/ST_optimisation_";
    // const TString file_prefix = "../../output/ST_optimisation/ST_optimisation_";
    const TString img_prefix = "../../../images/ST_optimisation/ST_charged_";
    // const TString img_prefix = "../../../images/ST_optimisation/ST_optimisation_";
    const TString save_root = "saved_hist_ST_charged";
    
    const unsigned int n_funcs = 3;
    cut_func_ptr cuts[n_funcs] = {&stopped_charged,
         &not_stopped_charged,
         &all_scint1_charged};
         
     cut_func_ptr air_cut[] = {&all_scint1_charged};
         
     do_it_all(n_files, air_root, file_prefix, img_prefix, 
         save_root, file_roots, n_funcs, cuts, air_cut);
}