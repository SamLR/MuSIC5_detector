// #include <iostream>
#include "useful_for_root/drawing.C"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#define MAX_INDEX 5000

struct in_branch {
    int n_hits;
    int id[MAX_INDEX];
    int pdgid[MAX_INDEX];
    int counter[MAX_INDEX];
    int parentid[MAX_INDEX];
    bool first_step[MAX_INDEX];
    double px[MAX_INDEX];
    double py[MAX_INDEX];
    double pz[MAX_INDEX];
    
    double in_px;
    double in_py;
    double in_pz;
};

void set_addresses(in_branch& mom, TTree* tree);
TH1F* make_hist(const char* file_root);
double get_momentum(const in_branch& branch, const int hit);
int get_index(const int* array, const int length, const int target);

void quick_stopped_muon_mom() {
    // not used here but useful
    const double mu_per_uA = 1.0e-6/(9.0e8*1.602e-19); // muons per amp for our initial 900M
    const int n_files = 4;
    const char file_roots [n_files][16] = { "5mm_Air",
        "0.5mm_Aluminium",
        "1mm_Aluminium",
        "5mm_Aluminium"};
        
    const char prefix [] = "../../output/final/final_st_Copper_0.5mm_deg_";
    const char suffix [] = ".root";
    

    TH1F* hists [n_files];
    // int integrals [n_files];
    // int counts [n_files];
    
    
    // for(int file = 0; file < n_files; ++file) {
        // counts[file] = 0;
        // integrals[file] = 0;
    // }
    TFile* out_file = new TFile("stopped_muon_momenta_saved_hists.root", "RECREATE");
    for(int file = 0; file < n_files; ++file) {        
        char file_name [200];
        sprintf(file_name, "%s%s%s", prefix, file_roots[file], suffix);
        printf("Opening %15s, file %d of %d. ",file_roots[file], file+1, n_files);
        
        TFile* in_file = new TFile (file_name, "READ");
        TTree* in_tree = (TTree*) in_file->Get("truth");
        in_branch branch; 
        set_addresses(branch, in_tree);
        out_file->cd();
        hists[file] = make_hist(file_roots[file]);
        const int n_entries = in_tree->GetEntries();
        
        printf("Found %d entries\n", n_entries);

        for(int entry = 0; entry < n_entries; ++entry) {
            // if (entry > 1000000) break;
            
            if (entry%100000==0) {printf(".\n");}
            
            in_tree->GetEntry(entry);

            int muon_ids_pre_deg [MAX_INDEX];   // ensure we've logged a momentum
            int muon_ids_scint1 [MAX_INDEX];    // muons that have 'triggered'
            int electron_ids [MAX_INDEX];       // don't double count
            double muon_mom_pre_deg[MAX_INDEX]; // what we're actually going to plot
            // index bounds; don't want to loop over 500 zeros
            int n_seen_pre_deg = 0;  
            int n_seen_electrons = 0;
            const int hits_loop = (branch.n_hits > MAX_INDEX) ? MAX_INDEX : branch.n_hits;

            for(int i = 0; i < MAX_INDEX; ++i) {
                muon_ids_pre_deg[i] = -1;
                muon_ids_scint1[i]  = -1;
                electron_ids[i]     = -1;
                muon_mom_pre_deg[i] = -1.0;
            }
            
            for(int hit = 0; hit < hits_loop; ++hit) {
                // is it a muon at counter 5
                //   check we've not seen it
                //   add the id, and momentum to appropriate arrays 
                //   ++ n_seen_target
                // is it a muon in scint 1? 
                //   first step?
                //   add id to appropriate array
                //   ++ n_seen_scint1
                // is it an electron in scint 1 or 2
                //   first step
                //   is its parent id in the scint1 array (and hence from target)
                //   plot momentum
                const int pid = branch.pdgid[hit];
                const int counter = branch.counter[hit];
                const int id = branch.id[hit];
                const bool first_step = branch.first_step[hit];
                
                if (abs(pid) == 13) {
                    // muon stuff
                    if(counter == 5) {
                        // stuff for pre-degrader
                        const int index = get_index(muon_ids_pre_deg, n_seen_pre_deg, id);
                        if (index == -1) {
                            // new muon
                            muon_ids_pre_deg[n_seen_pre_deg] = id;
                            muon_mom_pre_deg[n_seen_pre_deg++] = get_momentum(branch, hit);
                        }
                    } else if (counter == 1 && first_step) {                        
                        // first step in scint 1 now a triggering muon
                        const int index = get_index(muon_ids_pre_deg, n_seen_pre_deg, id);

                        if (index != -1) {
                            // we've logged it at the target
                            // log at the same index for quicker access
                            muon_ids_scint1[index] = id;
                        } // else {
                        // // this branch shouldn't execute
                        // cout << "We've not see this one "<< id <<endl;
                        // for(unsigned int i = 0; i < n_seen_pre_deg; ++i) {
                        //     cout << muon_ids_pre_deg[i] << endl;
                        // }
                    }
                } else if (abs(pid) == 11) {
                    // electron/positron stuff
                    if ((counter == 3) &&  first_step) {
                        // first step in scint 2	
                        const int e_index = get_index(electron_ids, n_seen_electrons, id);
                        if (e_index == -1) {
                            // new electron!
                            electron_ids[n_seen_electrons++] = id; // log it!
                            // check against muon_ids_scint1 and hence muons_pre_deg
                            const int parent_index = get_index(muon_ids_scint1, n_seen_pre_deg, branch.parentid[hit]);
                            // have we seen its parent?
                            if (parent_index != -1) {
                                hists[file]->Fill(muon_mom_pre_deg[parent_index]);
                            }
                        }
                    }
                }
            }
        }
        // integrals[file] = hists[file]->Integral();
        // counts[file] = hists[file]->GetEntries();
        printf("File done\n");
        hists[file]->Scale(mu_per_uA);
    }
    out_file->Write();
    out_file->Close();
    
    printf("Because root sucks this will likely now segfault. Use the display macro to display the created histograms!");
    
    // Something below here apparently segfaults. Fuck knows why it just does. 
    // CBA sorting it, leaving this here as it works for small number of entries, 
    // moving display to another file that will read in the 'out_file'. fuck
    TString title("Stopped muon momentum");
    // TString save_location = "images/stopped_muon_momentum.svg";
    TString save_location1("images/stopped_muon_momentum.eps");
    TString save_location2("images/stopped_muon_momentum.svg");
    
    TString file_roots_tstrings[n_files] = {
        TString("5mm Air"),
        TString("0.5mm Aluminium"),
        TString("1mm Aluminium"),
        TString("5mm Aluminium")
    };
    printf("HERE I AM!\n");
    
    TCanvas* can = draw_pretty_hists(n_files, hists, title, file_roots_tstrings, save_location1, 1002201);
    can->SaveAs(save_location2);
}

void set_addresses(in_branch& branch, TTree* tree) {
    tree->SetBranchAddress("nhit", &branch.n_hits);
    tree->SetBranchAddress("trkid", &branch.id);
    tree->SetBranchAddress("pdgid", &branch.pdgid);
    tree->SetBranchAddress("counter", &branch.counter);
    tree->SetBranchAddress("parentid", &branch.parentid);
    tree->SetBranchAddress("first_step", &branch.first_step);
    tree->SetBranchAddress("px", &branch.px);
    tree->SetBranchAddress("py", &branch.py);
    tree->SetBranchAddress("pz", &branch.pz);
    
    tree->SetBranchAddress("in_Px", &branch.in_px);
    tree->SetBranchAddress("in_Py", &branch.in_py);
    tree->SetBranchAddress("in_Pz", &branch.in_pz);
}

TH1F* make_hist(const char* file_root) {
    // TString name = TString("Muon_momentum_with_")+file_root;
    char name [50];
    sprintf(name, "Muon_momentum_with_%s", file_root);
    TH1F* res = new TH1F(name,name, 60, 0, 240);
    res->GetXaxis()->SetTitle("Momentum (MeV/c)");
    res->GetYaxis()->SetTitle("Stopped muons/#muA");
    return res;
}

double get_momentum(const in_branch& branch, const int hit){
    const double px = branch.in_px;
    const double py = branch.in_py;
    const double pz = branch.in_pz;
    // const double px = branch.px[hit];
    // const double py = branch.py[hit];
    // const double pz = branch.pz[hit];

    return sqrt(px*px + py*py + pz*pz);
}

int get_index(const int* array, const int length, const int target) {
    for(int index = 0; index < length; ++index) {
        if (array[index] == target) {
            return index;
        }
    }
    return -1;
}