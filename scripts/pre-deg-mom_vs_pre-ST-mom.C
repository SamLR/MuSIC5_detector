// #include "2d_plotting.C"
#include "useful_for_root/music_root.C"
#include "useful_for_root/drawing.C"
void pre_deg_mom_vs_pre_ST_mom(){         
    const unsigned int n_files = 6;

    const TString file_roots [n_files] = { "Air_5mm",
        "Aluminium_0.2mm",
        "Aluminium_10mm",
        "Aluminium_1mm", // this one has a special z range file == 3
        "Aluminium_2mm",
        "Aluminium_5mm"};

    const TString file_prefix = "../../output/degrader_optimisation/degrader_optimisation_";
    const TString img_dir = "../../../images/degrader_optimisation_2d_plots/";
    const TString save_file_name = "saved_hist";
    TString img_name_prefix = "ave_PID_deg_vs_ST_";

    const unsigned int n_funcs = 2;
    // should give us number of particles at that momentum position 
    // and pid weighted number of particles 
    // one divided by the other should give the average pid at that position
    cut_func_ptr cuts[n_funcs] = {&preDeg_vs_preST_charged_PIDweight, &preDeg_vs_preST_charged};
    const TString hist_names[n_funcs] = {"deg_vs_ST_", "deg_vs_ST_pid_"};

    TH2F* hists [n_files][n_funcs];
    // do_it_all2D(n_files, file_prefix, save_root, file_roots, 
    //     n_funcs, cuts, hist_names, hists);
    
    fill_hists<TH2F>(n_files,file_prefix, save_file_name, file_roots,
        hist_names, n_funcs, hists, cuts, 2);

    const TString leg_title = "Primary particle colours";
    const TString xtitle = "Degrader Momentum (MeV)";
    const TString ytitle = "Stopping Target Momentum (MeV)";

    // make pretty pictures! 
    for(unsigned int file = 0; file < n_files; ++file) {
        TString name = img_name_prefix + file_roots[file];
        TString img_file = img_dir +  name  + ".eps";

        TH2F* draw_hist = init_2Dhist(name, 50, 0, 200,50, 0, 200, xtitle, ytitle);
        draw_hist->Divide(hists[file][0], hists[file][1]);  

        if (file_roots[file] == "Aluminium_1mm" ){
            hist_draw(draw_hist, leg_title, name, true);
        } else {               
            hist_draw(draw_hist, leg_title, name, false);
        }    
    }
}

const int al_1mm_colors [7] = {kViolet+2, kAzure+1, kCyan, kGreen, kSpring+10, kOrange-3, kRed};
const TString al_1mm_names [7] = {"#pi^{-}", "#mu^{+}", "e^{+}", "e^{-}", "#mu^{-}", "#pi^{+}","p"};

const int default_colors [6] = {kViolet+2, kAzure, kTeal, kGreen, kOrange-3, kRed};
const TString default_names [6] = {"#pi^{-}", "#mu^{+}", "e^{+}", "e^{-}", "#mu^{-}", "#pi^{+}"};

void hist_draw(TH2* draw_hist, const TString& leg_title, const TString& name, const bool longlist){
    TCanvas* can = new TCanvas(name, name);
    draw_hist->Draw("COLZ");
    draw_hist->SetMinimum(1);
    can->Update();
    edit_stats_box(can, draw_hist, 1002200, 0.65, 0.90, 0.15,0.50);
    if (longlist){
        create_boxy_legend(draw_hist, leg_title, 7, al_1mm_names, al_1mm_colors);
    } else {               
        create_boxy_legend(draw_hist, leg_title, 6, default_names, default_colors);
    }    

}


typedef vector<int> intvec;
typedef vector<double> dblvec;



void preDeg_vs_preST_charged_PIDweight(const in_branch_struct& branch, const TH2* hist, const bool verbose){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec target_tracks_ids;
    intvec degrader_tracks_ids;
    dblvec degrader_momentums;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];

        // if ( abs(pid) != 13 ) continue;
        bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );
        if (!charged) continue;
        
        int counter = branch.counter[hit];        
        if (counter == 1004) {
            // just before degrader
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(degrader_tracks_ids, trackID);

            if (!seen_it) { 
                     // not seen it so add the track ID to the list 
                     // and record the momentum
                degrader_tracks_ids.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                degrader_momentums.push_back(mom);
            } 
        } else if ( counter == 1002 ) { 
            // just before target
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(target_tracks_ids, trackID);
            if (!seen_it) { 
                // new particle, lets find what it's momentum in scint1 was
                for(unsigned int i = 0; i < degrader_tracks_ids.size(); ++i) {
                    if(degrader_tracks_ids[i] == trackID){
                        double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                        hist->Fill(degrader_momentums[i],mom, pid_to_apid(pid));
                        target_tracks_ids.push_back(trackID);
                        break; // mission complete next hit!
                    }
                }
            }
        }
    }
}

void preDeg_vs_preST_charged(const in_branch_struct& branch, const TH2* hist, const bool verbose){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec target_tracks_ids;
    intvec degrader_tracks_ids;
    dblvec degrader_momentums;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];

        // if ( abs(pid) != 13 ) continue;
        bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );

        int counter = branch.counter[hit];        
        if (counter == 1004) {
            // just before degrader
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(degrader_tracks_ids, trackID);

            if (!seen_it) { 
                     // not seen it so add the track ID to the list 
                     // and record the momentum
                degrader_tracks_ids.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                degrader_momentums.push_back(mom);
            } 
        } else if ( counter == 1002 ) { 
            // just before target
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(target_tracks_ids, trackID);
            if (!seen_it) { 
                // new particle, lets find what it's momentum in scint1 was
                for(unsigned int i = 0; i < degrader_tracks_ids.size(); ++i) {
                    if(degrader_tracks_ids[i] == trackID){
                        double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                        hist->Fill(degrader_momentums[i],mom);
                        target_tracks_ids.push_back(trackID);
                        break; // mission complete next hit!
                    }
                }
            }
        }
    }
}
