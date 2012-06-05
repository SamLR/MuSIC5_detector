#include "stopped_and_degrader_effect.C"
#include "useful_drawing.C"

void comparisons_stopped_and_air(const TString file_prefix = "../../output/ST_optimisation/ST_optimisation_"){
    // const unsigned int n_files = 3;
    const unsigned int n_files = 6;

    const TString air_root = "Air_5mm"; // background comparison for scint 1
     // const TString file_roots [n_files] = {"Aluminium_20mm",
     //     "Aluminium_50mm",
     //     "Aluminium_100mm"};
    const TString file_roots [n_files] = {"Air_5mm",
        "Aluminium_0.2mm",
        "Aluminium_10mm",
        "Aluminium_1mm",
        "Aluminium_2mm",
        "Aluminium_5mm"};
     //     "Polystyrene_0.2mm",
     //     "Polystyrene_10mm",
     //     "Polystyrene_1mm",
     //     "Polystyrene_2mm",
     //     "Polystyrene_5mm"};
    // const TString file_prefix = "../../output/ST_optimisation/ST_optimisation_";
    // const TString file_prefix = "../../output/degrader_optimisation/degrader_optimisation_";
    const TString img_prefix = "../../../images/degrader_optimisation/";
    // const TString img_prefix = "../../../images/ST_optimisation/ST_optimisation_";
    const TString save_root = "saved_hist_ST";



    const unsigned int n_funcs = 3;
    const TString func_names[n_funcs] = {"stopped_muons","not_stopped_muons","all_muons_scint1"};
    cut_func_ptr cuts[n_funcs] = {&stopped_muons,
        &not_stopped_muons,
        &all_scint1_muons};                     
    cut_func_ptr air_cut[] = {&all_scint1_muons};

    TH1F* hists[n_files][n_funcs];

    analyse1d(n_files, file_prefix, save_root, file_roots, 
        func_names, n_funcs, hists, cuts );

    for(unsigned int file = 1; file < n_files; ++file) {

        cout << file << " " << file_roots[file] << endl;
         // make pretty pictures! 
        TString h_title = "Muons stopped after " + file_roots[file];
        // TString img_location = img_prefix + file_roots[file] + "_stopped" + img_suffix;
        draw_pretty_two_hists(hists[file][1], hists[file][0], h_title, 
            "not stopped", "stopped");   
            // "not stopped", "stopped", img_location);   

        h_title = "Comparison to Air of "+ file_roots[file];
        // img_location = img_prefix + file_roots[file] + "_post_degrader" + img_suffix;
        draw_pretty_two_hists(hists[0][0], hists[file][2], h_title, 
            "air", file_roots[file]);
            // "air", file_roots[file], img_location);
    }
}

typedef vector<int> intvec;
typedef vector<double> dblvec;
void all_scint1_muons(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    // loop over all hits
    // is it a muon or an electron
    // if it's a muon is it in scint1?
    //      has it already been seen?
    //      if not add it's momentum & trackID to vector
    // if it's an electron is it in scint1 or 2?
    //      has it already been seen? 
    //      if not does its parentID match a trackID in the vector?
    //      if so Fill the hist with that momentum and add track to seen_e vec

    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_muons;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        int counter = branch.counter[hit];
        int trackID = branch.trkid[hit];

        if ((abs(pid) == 13) && (counter == 1)) {
            // muon in scint 1, have we already seen it? 
            bool seen_it = is_in<int>(scint1_muons, trackID);

            if (!seen_it) { 
                // not seen it so add the track ID to the list 
                // and record the momentum
                scint1_muons.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                hist->Fill(mom);
            } 
        } 
    }
}

void stopped_muons(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    // loop over all hits
    // is it a muon or an electron
    // if it's a muon is it in scint1?
    //      has it already been seen?
    //      if not add it's momentum & trackID to vector
    // if it's an electron is it in scint1 or 2?
    //      has it already been seen? 
    //      if not does its parentID match a trackID in the vector?
    //      if so Fill the hist with that momentum and add track to seen_e vec

    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_muons;
    dblvec muon_moms;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        int counter = branch.counter[hit];
        int trackID = branch.trkid[hit];

        if ((abs(pid) == 13) && (counter == 1)) {
            // muon in scint 1, have we already seen it? 
            bool seen_it = is_in<int>(scint1_muons, trackID);

            if (!seen_it) { 
                // not seen it so add the track ID to the list 
                // and record the momentum
                scint1_muons.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                muon_moms.push_back(mom);
            } 
        } else if (abs(pid) == 11 && (counter == 3 || counter == 1)) { 
            // see if this electron has a muon parent by looping through the 
            // seen muon tracks
            for(unsigned int i = 0; i < scint1_muons.size(); ++i) {
                if(scint1_muons[i] == branch.parentid[hit]){
                    hist->Fill(muon_moms[i]);
                    return; // mission complete! (assume 1 muon/event)
                }
            }
        }
    }
}

void not_stopped_muons(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_muons;
    dblvec muon_moms;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        int counter = branch.counter[hit];
        int trackID = branch.trkid[hit];

        if ( abs(pid)!= 13 ) continue; // not a muon? not interested

        if (counter == 1) {
                 // muon in scint 1, have we already seen it? 
            bool seen_it = is_in<int>(scint1_muons, trackID);

            if (!seen_it) { 
                     // not seen it so add the track ID to the list 
                     // and record the momentum
                scint1_muons.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                muon_moms.push_back(mom);
            } 
        } else if ( counter == 3 ) { 
            // find the momentum of the muon in scint 1
            for(unsigned int i = 0; i < scint1_muons.size(); ++i) {
                if(scint1_muons[i] == trackID){
                    hist->Fill(muon_moms[i]);
                    return; // mission complete! (assume 1 muon/event)
                }
            }
        }
    }
}

void all_scint1_charged(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    // loop over all hits
    // is it a muon or an electron
    // if it's a muon is it in scint1?
    //      has it already been seen?
    //      if not add it's momentum & trackID to vector
    // if it's an electron is it in scint1 or 2?
    //      has it already been seen? 
    //      if not does its parentID match a trackID in the vector?
    //      if so Fill the hist with that momentum and add track to seen_e vec

    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec seen_tracks;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );

        if ( !charged ) continue;

        int counter = branch.counter[hit];

        if ( counter == 1 ) {
            int trackID = branch.trkid[hit];
            // muon in scint 1, have we already seen it? 
            bool seen_it = is_in<int>(seen_tracks, trackID);

            if (!seen_it) { 
                // not seen it so add the track ID to the list 
                // and record the momentum
                seen_tracks.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                hist->Fill(mom);
            } 
        } 
    }
}

void stopped_charged(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    // Loop over all hits
    // 
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_track_ids;
    intvec scint2_track_ids;
    dblvec momentums;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );

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
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                momentums.push_back(mom);

                // figure out if it's a decay product
                for(unsigned int i = 0; i < scint1_track_ids.size(); ++i) {
                    if( scint1_track_ids[i] ==  branch.parentid[hit] ){
                        hist->Fill(momentums[i]); // plot the momentum of the parent
                        break;
                    }
                } 
            } 
        } else if ( counter == 3 ) { 
            // in scint 2, if it's not been seen already it may be a decay product
            int trackID = branch.trkid[hit];
            bool seen_it_scint2 = is_in<int>(scint2_track_ids, trackID);
            if ( seen_it_scint2 ) continue; // already seen it, boring!

            scint2_track_ids.push_back(trackID);
            bool seen_it_scint1 = is_in<int>(scint1_track_ids, trackID);

            if ( seen_it_scint1 ) { 
                // seen in both scints so check if it's a decay
                for(unsigned int i = 0; i < scint1_track_ids.size(); ++i) {
                    // find the parent
                    if( scint1_track_ids[i] ==  branch.parentid[hit] ) {
                        hist->Fill(momentums[i]); // plot the momentum of the parent
                        break; // found it, move to the next hit
                    }
                }
            }
        }
    }
}

void not_stopped_charged(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_tracks_ids;
    intvec scint2_tracks_ids;
    dblvec scint1_momentums;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        int pid = branch.pdgid[hit];
        bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );

        if ( !charged ) continue;

        int counter = branch.counter[hit];        
        if (counter == 1) {
                 // in scint 1, have we already seen it? 
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(scint1_tracks_ids, trackID);

            if (!seen_it) { 
                     // not seen it so add the track ID to the list 
                     // and record the momentum
                scint1_tracks_ids.push_back(trackID);
                double mom = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                scint1_momentums.push_back(mom);
            } 
        } else if ( counter == 3 ) { 
            // in scint3 have we seen this particle in scint2?
            int trackID = branch.trkid[hit];
            bool seen_it = is_in<int>(scint2_tracks_ids, trackID);

            if (!seen_it) { 
                // new particle, lets find what it's momentum in scint1 was
                for(unsigned int i = 0; i < scint1_tracks_ids.size(); ++i) {
                    if(scint1_tracks_ids[i] == trackID){
                        hist->Fill(scint1_momentums[i]);
                        scint2_tracks_ids.push_back(trackID);
                        break; // mission complete next hit!
                    }
                }
            }
        }
    }
}



