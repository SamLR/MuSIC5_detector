#include "useful.C"
#include "useful_inits.C"

struct in_branch_struct {
    // structure as passed in from geant4
    int g_iev;
    int in_event;
    int in_trackID;
    int in_pdg;
    double in_x; 
    double in_y; 
    double in_z;
    double in_px;
    double in_py;
    double in_pz;
    double in_tof;
    int g_nhit;
    int procid[500];
    int counter[500];
    int trkid[500];
    int parentid[500];
    int pdgid[500];
    double x[500];
    double y[500];
    double z[500];
    double px[500];
    double py[500];
    double pz[500];
    double edep[500];
    double tof[500];
};


void analyse1d(const unsigned int& n_files,
    const TString& file_prefix,
    const TString& save_file_name,
    const TString* file_roots,
    const TString* func_name_roots,
    const unsigned int& n_funcs,
    const TH1F* hists[n_files][n_funcs],
    cut_func_ptr cuts,
    const TString& file_suffix=".root",
    const TString& img_suffix=".eps",
    const TString xtitle = "Momentum (MeV)",
    const TString ytitle = "Count",
    const int n_bins = 50, 
    const int x_min = 0, 
    const int x_max = 200
){
    // root file to save things in
    TString out_file_name = file_prefix + save_file_name + file_suffix;
    TFile* out_file = init_file(out_file_name, "RECREATE");
    // branch to address to
    in_branch_struct branch;
        
    for(unsigned int file_no = 0; file_no < n_files; ++file_no) {
        TH1F** hist_set = hists[file_no]; // alias to the set of hists we want
        cout << endl<< "Starting file "<< file_roots[file_no] << endl;
        // munge the filename
        TString resolved_filename = file_prefix + file_roots[file_no] + file_suffix;
        cout << resolved_filename << endl;
        // open up and initialise things
        in_file = init_file(resolved_filename);
        in_tree = init_tree<in_branch_struct>(in_file, "t", branch, &set_in_branch_address);
        
        // create the histograms then fill them in the loop
        out_file->cd(); // make sure they're added to the save file
        for(unsigned int func = 0; func < n_funcs; ++func) {
            // initialise the histograms
            TString name = func_name_roots[func] + file_roots[file_no];
            hist_set[func] = init_1Dhist(name, n_bins, x_min, x_max, xtitle, ytitle);
        }
        // loop-de-loop        
        loop_entries<in_branch_struct, TH1F*>(in_tree, branch, hist_set, n_funcs, cuts, true);
    }
    cout << "out_file not enabled. Uncomment it, please" << endl;
    // out_file->Write();
    cout << "It... is done." << endl;
}

void set_in_branch_address(const in_branch_struct& branch, const TTree* tree) { 
    // assign the addresses of the various branchs
    tree->SetBranchAddress("iev",&branch.g_iev);
    tree->SetBranchAddress("in_EventID",&branch.in_event);
    tree->SetBranchAddress("in_TrackID",&branch.in_trackID);
    tree->SetBranchAddress("in_PDGid",&branch.in_pdg);
    tree->SetBranchAddress("in_x",&branch.in_x);
    tree->SetBranchAddress("in_y",&branch.in_y);
    tree->SetBranchAddress("in_z",&branch.in_z); 
    tree->SetBranchAddress("in_Px",&branch.in_px); 
    tree->SetBranchAddress("in_Py",&branch.in_py); 
    tree->SetBranchAddress("in_Pz",&branch.in_pz); 
    tree->SetBranchAddress("in_tof",&branch.in_tof); 

    tree->SetBranchAddress("nhit",&branch.g_nhit);
    tree->SetBranchAddress("procid",&branch.procid);
    tree->SetBranchAddress("counter",&branch.counter);
    tree->SetBranchAddress("trkid",&branch.trkid);
    tree->SetBranchAddress("parentid",&branch.parentid);
    tree->SetBranchAddress("pdgid",&branch.pdgid);
    tree->SetBranchAddress("x",&branch.x);
    tree->SetBranchAddress("y",&branch.y);
    tree->SetBranchAddress("z",&branch.z);
    tree->SetBranchAddress("px",&branch.px);
    tree->SetBranchAddress("py",&branch.py);
    tree->SetBranchAddress("pz",&branch.pz);
    tree->SetBranchAddress("edep",&branch.edep);
    tree->SetBranchAddress("tof",&branch.tof);
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
            
            if ( seen_it_scint1 ) { // seen in both scints so check if it's a decay
                for(unsigned int i = 0; i < scint1_track_ids.size(); ++i) {
                    // find the parent
                    if( scint1_track_ids[i] ==  branch.parentid[hit] ){
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

    