#include "2d_plotting.C"

void pre_deg_mom_vs_pre_ST_mom(){         
    const unsigned int n_files = 6;
    // const unsigned int n_files = 2;

    // const TString file_roots [n_files] = {"Air_5mm", "Aluminium_1mm"};
    const TString file_roots [n_files] = { "Air_5mm",
        "Aluminium_0.2mm",
        "Aluminium_10mm",
        "Aluminium_1mm", // this one has a special z range file == 3
        "Aluminium_2mm",
        "Aluminium_5mm"};

    const TString file_prefix = "../../output/degrader_optimisation/degrader_optimisation_";
    const TString img_dir = "../../../images/degrader_optimisation_2d_plots/";
    const TString save_root = "saved_hist";
    TString img_name_prefix = "ave_PID_deg_vs_ST_";

    const unsigned int n_funcs = 2;
    // should give us number of particles at that momentum position 
    // and pid weighted number of particles 
    // one divided by the other should give the average pid at that position
    cut_func_ptr cuts[n_funcs] = {&preDeg_vs_preST_charged_PIDweight, &preDeg_vs_preST_charged};
    const TString hist_names[n_funcs] = {"deg_vs_ST_", "deg_vs_ST_pid_"};

    TH2F* hists [n_files][n_funcs];
    do_it_all2D(n_files, file_prefix, save_root, file_roots, 
        n_funcs, cuts, hist_names, hists);

    const TString leg_title = "Primary particle colours";
    const TString xtitle = "Degrader Momentum (MeV)";
    const TString ytitle = "Stopping Target Momentum (MeV)";
    
    const int al_1mm_colors [7] = {kViolet+2, kAzure+1, kCyan, 
        kGreen, kSpring+10, kOrange-3, kRed};
    const TString al_1mm_names [7] = {"#pi^{-}", "#mu^{+}",
        "e^{+}", "e^{-}", "#mu^{-}", "#pi^{+}","p"};
    
    const int default_colors [6] = {kViolet+2, kAzure, kTeal, 
        kGreen, kOrange-3, kRed};
    const TString default_names [6] = {"#pi^{-}", "#mu^{+}",
        "e^{+}", "e^{-}", "#mu^{-}", "#pi^{+}"};

    // make pretty pictures! 
    for(unsigned int file = 0; file < n_files; ++file) {
        TString name = img_name_prefix + file_roots[file];
        TString img_file = img_dir +  name  + ".eps";
        
        TH2F* draw_hist = init_2Dhist(name, 50, 0, 200,50, 0, 200, xtitle, ytitle);
        draw_hist->Divide(hists[file][0], hists[file][1]);  
        
        TCanvas* can = new TCanvas(name, name);
        draw_hist->Draw("COLZ");
        draw_hist->SetMinimum(1);
        can->Update();
        edit_stats_box(can, draw_hist, 1002200, 0.65, 0.90, 0.15,0.50);
        if (file_roots[file] == "Aluminium_1mm" ){
            create_boxy_legend(draw_hist, leg_title, 7, al_1mm_names, al_1mm_colors);
        } else {               
            create_boxy_legend(draw_hist, leg_title, 6, default_names, default_colors);
        }    
        // can->Update();
        can->SaveAs(img_file);
    }
}

