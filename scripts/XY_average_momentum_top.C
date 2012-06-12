#include "XY_average_momentum.C"

void XY_average_momentum_top(){
    const unsigned int n_files = 1;

    const TString file_roots [n_files] = {"Air_5mm"};

    const TString file_prefix = "../../output/ST_optimisation/ST_optimisation_";
    const TString img_prefix = "../../../images/ST_optimisation/pre_degrader";
    const TString save_file_name = "predegrader_xy_plots";

    XY_average_momentum(n_files, file_roots, file_prefix, img_prefix, save_file_name, true);
}



