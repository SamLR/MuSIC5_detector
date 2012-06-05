
#include "useful.C"

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

void do_it_all3D(const unsigned int& n_files,
    const TString& file_prefix,
    const TString& img_prefix,
    const TString& save_root,
    const TString* file_roots,
    const unsigned int& n_funcs,
    cut_func_ptr cuts,
    const TString* hist_titles,
    const TString* hist_names,
    const TString& file_suffix=".root",
    const TString& img_suffix=".eps",
    const TString xtitle = "Momentum at degrader (MeV)",
    const TString ytitle = "Momentum at stopping target (MeV)", 
    const TString ztitle = "particle", 
){
    TString out_file_name = file_prefix + save_root + file_suffix;
    TFile* out_file = init_file(out_file_name, "RECREATE");
    // array of function pointers to use

    TH3F* hists [n_files][n_funcs]; // one for stopped, one for not
    for(unsigned int file_no = 0; file_no < n_files; ++file_no) {
        in_branch_struct branch;
        TH3F** hist_set = hists[file_no]; // alias to the pair of hists we want

        cout << endl<< "Starting file "<< file_roots[file_no] << endl;
        // munge the filename
        TString resolved_filename = file_prefix + file_roots[file_no] + file_suffix;
        // open up and initialise things
        in_file = init_file(resolved_filename);
        
        in_tree = init_tree<in_branch_struct>(in_file, "t", branch, &set_in_branch_address);

        // create the histograms then fill them in the loop
        out_file->cd(); // make sure they're added to the save file
        for(unsigned int i = 0; i < n_funcs; ++i) {
            TString name = hist_names[i] + file_roots[file_no]; // munge munge munge
            hist_set[i] = init_2Dhist (name, 50, 0, 200,50, 0, 200, xtitle, ytitle);   
        }

        // loop-de-loop
        loop_entries<in_branch_struct, TH3F>(in_tree, branch, hist_set, n_funcs, cuts, true);

        // make pretty pictures! 
        for(unsigned int i = 0; i < n_funcs; ++i) {
            TString title = hist_titles[i] + file_roots[file_no]; // munge munge munge
            TString img_location = img_prefix + file_roots[file_no] + title + img_suffix;
            TString option = "SCAT";
            draw_pretty_hist(hist_set[i], img_location, option);   
        }
    }
    out_file->Write();

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

const unsigned int convertPID(const int inpid){
    // convert a PID to one in a restricted range (for z plotting)
    // ( e==11 || µ==13 || π==211 || p==2212 );
    switch (inpid){
        case -211: return -3;
        case  -13: return -2;
        case  -11: return -1;
        case   11: return  1;
        case   13: return  2;
        case  211: return  3;
        case 2212: return  4;
    }
}

void preDeg_vs_preST_muons(const in_branch_struct& branch, const TH3* hist, const bool verbose){
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
                        hist->Fill(degrader_momentums[i],mom, convertPID(pid));
                        target_tracks_ids.push_back(trackID);
                        break; // mission complete next hit!
                    }
                }
            }
        }
    }
}
