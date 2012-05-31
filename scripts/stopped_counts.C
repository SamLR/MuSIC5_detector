// create a 2d plot based on the number of stopped muons 
// and another of number of stopped charged particles
// whilst varying stopping target and degrader thickness

#include "stopped_and_degrader_effect.C"

void stopped_counts(){
    
    const unsigned int n_dz = 4;
    const TString file_roots[n_dz * n_dz] = {"1mm_1mm",  "1mm_5mm",  "1mm_10mm",  "1mm_15mm",
        "5mm_1mm",  "5mm_5mm",  "5mm_10mm",  "5mm_15mm",
        "10mm_1mm", "10mm_5mm", "10mm_10mm", "10mm_15mm",
        "15mm_1mm", "15mm_5mm", "15mm_10mm", "15mm_15mm"};

    const unsigned int dZs[n_dz] = {1, 5, 10, 15}; // the thicknesses

    const unsigned int n_func = 2; // charged count and muon count
    cut_func_ptr cuts [n_func] = {&stopped_charged_count, &stopped_muons_count};

    const TString dat_file_dir = "../../output/st_and_degrader/"; // where in files are and where to create out files
    const TString in_file_pref = "st_and_degrader_st_"; // any file prefix
    const TString in_file_suff = ".root"; // file suffix
    const TString save_file_root = "st_and_deg_stopped_counts"; 
    const TString img_dir  = "../../../images/st_and_degrader/stopped"; // where to save pretty pictures
    const TString img_suff = ".eps"; // save format

    unsigned int counts [n_dz][n_dz][n_func]; // the output from the loops will go here

    TString out_file_name = dat_file_dir + save_file_root + in_file_suff;
    TFile* out_file = init_file(out_file_name, "RECREATE");

    TH2F* stopped_muon_hist = init_2Dhist ("stopped_muons", 4, 1, 20, 4, 1, 20,
        "Degrader thickness (mm)", "Stopping target thickness (mm)"); // axis titles
    TH2F* stopped_charged_hist = init_2Dhist ("stopped_charged_particles", 4, 1, 20, 4, 1, 20,
        "Degrader thickness (mm)", "Stopping target thickness (mm)"); // axis titles

    for(unsigned int sti = 0; sti < n_dz; ++sti) {
        for(unsigned int degi = 0; degi < n_dz; ++degi) {
            cout << endl;
            cout << "File: " << file_roots[sti*n_dz + degi] << " st_dz: "<< dZs[sti] << " deg_dz: "<< dZs[degi] << endl;
            TString resolved_filename = dat_file_dir + in_file_pref + file_roots[degi+sti] + in_file_suff;

                // open up and initialise things
            in_file = init_file(resolved_filename);
            in_branch_struct branch;
            in_tree = init_tree<in_branch_struct>(in_file, "t", branch, &set_in_branch_address);

            // loop over the trees counting the number of stopped muons & charged particles
            loop_entries<in_branch_struct, unsigned int>(in_tree, branch, counts[degi][sti], n_func, cuts, true);
            // loop_entries<in_branch_struct, TH1F>(in_tree, branch, hist_set, n_funcs, cuts, true);
            stopped_charged_hist->Fill(dZs[degi], dZs[sti], counts[degi][sti][0]);
            stopped_muon_hist->Fill(dZs[degi], dZs[sti], counts[degi][sti][1]);
        }
    }
    TCanvas* can_muon = new TCanvas("muons", "muons");
    stopped_muon_hist->Draw("COLZ");
    
    TCanvas* can_charged = new TCanvas("charged", "charged");
    stopped_charged_hist->Draw("COLZ");
    // create histogram
    
    out_file->Write();
}


typedef bool (*pid_check)(const int&);
void stopped_pid_count(const in_branch_struct& branch, const int& count, pid_check check){
    // Loop over all hits
       // 
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_track_ids;
    intvec scint2_track_ids;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        bool charged = (*check)(pid);

        if ( !charged ) continue;

        int counter = branch.counter[hit];

        if ( counter==1 ) {
               // in scint 1, have we already seen it?             
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(scint1_track_ids, trackID);

            if ( !seen_it ) { 
                   // new track, add it to the track list
                   // and record the momentum
                scint1_track_ids.push_back(trackID);
                bool seen_parent_scint1 = is_in<int>(scint1_track_ids,branch.parentid[hit]);
                if (seen_parent_scint1) ++count;
            } 
        } else if ( counter == 3 ) { 
               // in scint 2, if it's not been seen already it may be a decay product
            int trackID = branch.trkid[hit];
            bool seen_it_scint2 = is_in<int>(scint2_track_ids, trackID);

            if ( !seen_it_scint2 ){  // new one for scint 2

                scint2_track_ids.push_back(trackID);
                bool seen_parent_scint1 = is_in<int>(scint1_track_ids,branch.parentid[hit]);
                if (seen_parent_scint1) ++count;

            }
        }
    }
}

bool muon_check(const int& pid){
    return (abs(pid)==13);
}

bool charged_check(const int& pid){
    return ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );
}

void stopped_muons_count(const in_branch_struct& branch, const int& count, const bool verbose ){
    stopped_pid_count(branch, count, &muon_check);
}

void stopped_charged_count(const in_branch_struct& branch, const int& count, const bool verbose ){
    stopped_pid_count(branch, count, &charged_check);
}
