#include "useful_for_root/music_root.C"
#include "useful_for_root/drawing.C"

// This creates two sets of simple comparison histograms:
//      * The number of muons in scint 1 with the number of muons at the same
//        place for the first file (i.e. the first file root is used as a basis)
//      * The number of stopped muons Vs the number of muons that don't stop
//
// These histograms are saved (to file_prefix/save_file_name), printed to screen
// and saved as .eps files (in img_prefix)

void edep_in_degrader(const int& n_files, // number of file roots
    // basic elements of names differentiateing the files
    const TString* file_roots,                       
    // file prefix, mainly the directory but also any other common element
    const TString& file_prefix,                      
    // as above but for the saved images
    const TString& img_prefix,          
    // root to use for the root file of histograms             
    const TString& save_file_name,
    // reduces the number of entries read and stops stuff getting saved
    const bool& testing = false       
){
    const unsigned int n_funcs = 3; // mainly a simple check for arrays

    // prefixes for the histograms produced by the different functions 
    const TString func_names[n_funcs] = {"edep_in_degrader_","pre_degrager_muon_mom_","post_degrader_muon_mom_"};

    // the functions applied to every hit
    cut_func_ptr cuts[n_funcs] = {&deg_edep_muon, // energy deposited in the degrader
            // &mom_at_pre_deg_muon,                       // momentums of muons pre degrader (special counter 1004)
            // &mom_at_post_deg_muon};                     // momentums of all muons at scint1 (i.e. post degrader)
            &mom_at_pre_deg_muon_parent,                       // momentums of muons pre degrader (special counter 1004)
            &mom_at_post_deg_muon_parent};                     // momentums of all muons at scint1 (i.e. post degrader)

    TH1F* hists[n_files][n_funcs]; // array to hold the produced histograms

    // function that opens the files, uses the trees to fills the 1D histograms
    // based on the functions and then saves the histograms
    const TString axis_titles [n_funcs*3] = {"Energy (MeV)", "Count", "Z", 
        "Momentum (MeV/c)", "Count", "Z",
        "Momentum (MeV/c)", "Count", "Z"};
    const int       bins [n_funcs*3] = {100, 100, 100,50,   50,  50,   50,  50,  50      };
    const double    mins [n_funcs*3] = {0,   0,   0,  0,    0,   0,    0,   0,   0       };
    const double    maxs [n_funcs*3] = {50,  50,  50, 200,  200,  200,200,  200,  200      };
    
    fill_hists<TH1F>(n_files, file_prefix, save_file_name, file_roots, 
        func_names, n_funcs, hists, cuts, 1, ".root", ".eps",
        axis_titles, bins, mins, maxs, testing, testing);


    // loop over all the file roots and draw the associated histograms
    for(unsigned int file = 0; file < n_files; ++file) {
        // compare number of stopped and not stopped muons
        TString img_location1;
        if (testing){
            img_location = "";
        } else {
            img_location = img_prefix + file_roots[file] + "_edep.eps";
        }

        TString h_title = func_names[0]+file_roots[file];
        draw_pretty_hist(hists[file][0], h_title, img_location1);

        // compare the muons ast scint1 with those at scint 1 in file 0
        TString img_location2;
        if (testing){
            img_location2 = "";
        } else {
            img_location2 = img_prefix + file_roots[file] + "_post_degrader.eps";
        }
        // TString img_location = "";
        TString h_title2 = "momentum on either side of degrader"+file_roots[file];
        draw_pretty_two_hists(hists[file][1], hists[file][2], h_title2, 
            func_names[1], func_names[2], img_location2);
    }
}


typedef vector<int> intvec;
typedef vector<double> dblvec;

void edep_sum(const in_branch_struct& branch, const TH1F* hist, 
    const int& wanted_counter, pid_cut* check_pid
){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    dblvec edeps;
    intvec seen_tracks;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {

        int counter = branch.counter[hit];
        if (counter != wanted_counter) continue;

        bool right_pid = (*check_pid)(branch,hit);

        if (right_pid) {
            // if it's new add it to the track & edep vectors
            // otherwise add its edep to the correct value
            const int trackID = branch.trkid[hit];
            const double edep = branch.edep[hit];
            bool seen_it = is_in(seen_tracks, trackID);

            if(seen_it){
                // find and increment the edep value
                // insert for loop
                for(unsigned int i = 0; i < seen_tracks.size(); ++i) {
                    if(seen_tracks[i] == trackID){
                        edeps[i] += edep;
                    }
                }
            } else {
                // new track add it to the vectors
                // trackID & edep should have same index
                seen_tracks.push_back(trackID);
                edeps.push_back(edep);
            }
        } 
    }

    // Now that all the various track's deposited energies have been read,
    // fill the histogram (want to plot the sum of the energy deposited in the
    // counter, not just the energy deposited in a step).
    if (edeps.size() == 0) return; // nothing to be filled
    dblvec::iterator iter = edeps.begin();

    for(iter; iter < edeps.end(); ++iter) hist->Fill( (*iter) ); 

}

void deg_edep_muon(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    edep_sum(branch, hist, eCN_degrader, &muon_only_check);
}

void mom_at(const in_branch_struct& branch, const TH1F* hist, 
    const int& wanted_counter, pid_cut* check_pid
){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    // dblvec edeps;
    intvec seen_tracks;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {

        int counter = branch.counter[hit];
        if (counter != wanted_counter) continue;

        bool right_pid = (*check_pid)(branch, hit);

        if (right_pid) {
            // if it's new add it to the track & edep vectors
            // otherwise add its edep to the correct value
            const int trackID = branch.trkid[hit];
            bool seen_it = is_in(seen_tracks, trackID);

            if(!seen_it){
                // new particle, record it and then make sure we don't see it again
                seen_tracks.push_back(trackID);
                double momentum = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                hist->Fill(momentum);
            } 
        } 
    }
}

void mom_at_pre_deg_muon_parent(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, eCN_degrader, &first_muon_only_check_no_parent);
    // mom_at(branch, hist, eCN_pre_deg, &parent_muon_only_check);
}

void mom_at_post_deg_muon_parent(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, eCN_scint1, &first_muon_only_check_no_parent);
    // mom_at(branch, hist, eCN_scint1, &parent_muon_only_check);
}


void mom_at_pre_deg_muon(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, eCN_pre_deg, &muon_only_check);
}

void mom_at_post_deg_muon(const in_branch_struct& branch, const TH1F* hist, const bool verbose){
    mom_at(branch, hist, eCN_scint1, &muon_only_check);
}



void test_edep_sum(){

    in_branch_struct branch;
    TH1F* test = new TH1F("Test", "test", 100, -50, 50);
    branch.g_nhit = 5 + 1;

    for(unsigned int i = 0; i < 5; ++i) { // should have 4 entries with monotomically increasing edep
        branch.counter[i] = 1;
        branch.pdgid[i] = 13;
        branch.trkid[i] = i;
        branch.edep[i] = i;
        branch.px[i] = 2;
        branch.py[i] = 2;
        branch.pz[i] = 2;

    }
    // one entry should be much higher at 20. (2 entries have track id == 2)
    branch.counter[5] = 1; 
    branch.pdgid  [5] = -13;
    branch.trkid  [5] = 2; 
    branch.edep   [5] = 19;
    branch.px[i] = 3;
    branch.py[i] = 3;
    branch.pz[i] = 3;
    // this entry should be ignored (wrong counter)
    branch.counter[6] = 2;
    branch.pdgid  [6] = 13;
    branch.trkid  [6] = 3; 
    branch.edep   [6] = 40;
    branch.px[i] = 20;
    branch.py[i] = 20;
    branch.pz[i] = 20;
    // this entry should also be ignored (wrong pid)
    branch.counter[7] = 1;
    branch.pdgid  [7] = 14;
    branch.trkid  [7] = 2; 
    branch.edep   [7] = -40;
    branch.px[i] = 10;
    branch.py[i] = 10;
    branch.pz[i] = 10;

    // edep_sum(branch, test, 1, &muon_check);
    mom_at(branch, test, 1, &muon_check);
    test->Draw();
}

void edep_in_degrader(){
    test_edep_sum();
}
