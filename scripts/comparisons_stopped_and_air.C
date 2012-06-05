#include "useful_for_root/music_root.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * The number of muons in scint 1 with the number of muons at the same
//        place for the first file (i.e. the first file root is used as a basis)
//      * The number of stopped muons Vs the number of muons that don't stop
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void comparisons_stopped_and_air(const int& n_files, // number of file roots
    const TString* file_roots,                       // basic elements of names differentiateing the files
    const TString& file_prefix,                      // file prefix, mainly the directory but also any other common element
    const TString& img_prefix,                       // as above but for the saved images
    const TString& save_file_name                    // root to use for the root file of histograms
){
    const unsigned int n_funcs = 3; // mainly a simple check for arrays
    
    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"stopped_muons","not_stopped_muons","all_muons_scint1"};
    
    // the functions applied to every hit
    cut_func_ptr cuts[n_funcs] = {&stopped_muons, // fills with the momentums of stopped muons
        &not_stopped_muons,                       // fills with the momentums of muons that don't stop
        &all_scint1_muons};                       // fills with the momentums of all muons at scint1

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, 1);


    // loop over all the file roots and draw the associated histograms
    for(unsigned int file = 1; file < n_files; ++file) {

        // compare number of stopped and not stopped muons
        TString h_title = "Muons stopped after " + file_roots[file];
        TString img_location = img_prefix + file_roots[file] + "_stopped.eps";
        draw_pretty_two_hists(hists[file][1], hists[file][0], h_title, 
            "not stopped", "stopped", img_location);   
        
        // compare the muons ast scint1 with those at scint 1 in file 0
        TString h_title2 = "Comparison to "+file_roots[0]+ " to "+ file_roots[file];
        img_location = img_prefix + file_roots[file] + "_post_degrader.eps";
        draw_pretty_two_hists(hists[0][0], hists[file][2], h_title2, 
            file_roots[0], file_roots[file], img_location);
            
        // draw pretty two hists draws two histograms, one of top of the other,
        // sets the titles, moves the stats box so they're all visible 
        // and creates a legend so you can tell the histograms apart
    }
}

typedef vector<int> intvec;
typedef vector<double> dblvec;

// this is a function the should be wrapped to be used as a cut function
// allows basic pid checking functions to be employed at scint 1
void all_scint1(const in_branch_struct& branch, const TH1* hist, pid_cut* check_pid){
    // loop over all hits checking the pid of each hit and recording the 
    // momentum if it's correct, in scint1 and a new hit
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    intvec scint1_muons;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        
        int counter = branch.counter[hit];
        if (counter != 1) continue;
        
        bool right_pid = (*check_pid)(branch.pdgid[hit]);
        
        if (right_pid) {
            // correct particle in scint 1, have we seen it before?
            int trackID = branch.trkid[hit];
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

// wrapper of all_scint1 that selects any charged particle
void all_scint1_charged(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    all_scint1(branch, hist, &charged_check);
}

// all_scint1 wrapper that only selects muons
void all_scint1_muons(const in_branch_struct& branch, const TH1* hist, const bool verbose){
    all_scint1(branch, hist, &muon_check);
}

// this looks for stopped muons by looking for muons in scint1 and then electrons
// in scint1 or 2 that have the same muon as a parent
void stopped_muons(const in_branch_struct& branch, const TH1* hist, const bool verbose){

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

// looks for muons that don't stop: those that are seen in both scint1& 2
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

// uses the same approach as stopped_muons but for all charged particles
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

// same as not_stopped_muons but for anything that's charged
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



