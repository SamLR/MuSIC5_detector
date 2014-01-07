// #include <iostream>
#include "useful_for_root/drawing.C"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#define MAX_INDEX 5000

struct in_branch {
    int n_hits;
    int pdgid[MAX_INDEX];
    int counter[MAX_INDEX];
    bool first_step[MAX_INDEX];
    double time[MAX_INDEX];
};

void set_addresses(in_branch& mom, TTree* tree);
int get_index(const int* array, const int length, const int target);
bool is_charged(const int pdgid);

void quick_stopped_muon_mom() {
    
    const int n_files = 4;
    const char file_roots [n_files][16] = { "5mm_Air",
        "0.5mm_Aluminium",
        "1mm_Aluminium",
        "5mm_Aluminium"};
    const char prefix [] = "../../output/final/final_st_Copper_0.5mm_deg_";
    const char suffix [] = ".root";

    int trigger_count [n_files];
    
    
    for(int file = 0; file < n_files; ++file) {        
        trigger_count[file] = 0;
        
        char file_name [200];
        sprintf(file_name, "%s%s%s", prefix, file_roots[file], suffix);
        
        printf("Opening file, %16s, %d of %d... ",file_roots[file], file+1, n_files);
        
        TFile* in_file = new TFile (file_name, "READ");
        TTree* in_tree = (TTree*) in_file->Get("truth");
        in_branch branch; 
        set_addresses(branch, in_tree);
        
        const int n_entries = in_tree->GetEntries();
        printf("Found %d entries\n", n_entries);

        for(int entry = 0; entry < n_entries; ++entry) {
            // if (entry > 1000000) break;
            
            if (entry%100000==0) {printf(".\n");}
            in_tree->GetEntry(entry);
            
            int u_time_index = 0;
            int d_time_index = 0;
            
            double u_times[MAX_INDEX];
            double d_times[MAX_INDEX];
            
            for(int hit = 0; hit < hits_loop; ++hit) {
                // Only pay attention to the first step in a volume
                if (!branch.first_step[hit]) {continue;}
                
                // Ignore non-charged particles
                const int pid = branch.pdgid[hit];
                if (!is_charged(pid)) {continue;}
                
                const int counter = branch.counter[hit];
                
                if (counter == 1) {
                    u_times[u_time_index++] = branch.tree[hit];
                } else if (counter == 3) {
                    d_times[d_time_index++] = branch.tree[hit];
                }
            }
            
            for(int u = 0; u < u_time_index; ++u) {
                for(int d = 0; d < d_time_index; ++d) {
                    if (TMath::Abs(u_times[u] - d_times[d])<50.0) {
                        /* code */
                    }
                }
            }
        }
        printf("File done\n");
    }
}

void set_addresses(in_branch& branch, TTree* tree) {
    tree->SetBranchAddress("nhit", &branch.n_hits);
    tree->SetBranchAddress("pdgid", &branch.pdgid);
    tree->SetBranchAddress("counter", &branch.counter);
    tree->SetBranchAddress("first_step", &branch.first_step);
    tree->SetBranchAddress("tof", &branch.time);
}

bool is_charged(const int pdgid) {
    return pdgid == -211 || pdgid == -13 || pdgid == -11 ||                // Anti pi, mu, e
           pdgid == 11   || pdgid == 13  || pdgid == 211 || pdgid == 2212; // e, mu, pi, p
}

int get_index(const int* array, const int length, const int target) {
    for(int index = 0; index < length; ++index) {
        if (array[index] == target) {
            return index;
        }
    }
    return -1;
}