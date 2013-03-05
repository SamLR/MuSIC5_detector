#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"
#include "TMath.h"

#include <iostream>

struct in_branch {
	int n_hits;
	int id[500];
	int pdgid[500];
	int counter[500];
	int parentid[500];
	bool first_step[500];
	double tof[500];
};

using namespace std;

void set_addresses(TTree* tree, in_branch& branch);
int get_index(const int* array, const int length, const int target);

void acceptance() {
    const int n_files = 4;
    // const int n_files = 1;
    const TString file_roots [n_files] = { "Air_5mm",
        "Aluminium_0.5mm",
        "Aluminium_1mm",
        "Aluminium_5mm" };

    // const TString prefix = "../../output/final_900M/final_st_Copper_0.5mm_deg_";
	const TString prefix = "../../output/final/final_st_Copper_0.5mm_deg_";
	const TString suffix = ".root";
    const TString out_file_root = "dt_hists";
	
    unsigned int counts[n_files];     // number of muons making the cuts
    unsigned int counts_no_cut[n_files];     // number of muons making the cuts
    unsigned int init_muons[n_files]; // the number of input muons from g4bl.
    
    for(int file = 0; file < n_files; ++file) {
        counts_no_cut[file] = 0;
        counts[file] = 0;
        init_muons[file] = 0;
    }

	for(unsigned int file = 0; file < n_files; ++file) {

		TString file_name = prefix + file_roots[file] + suffix;
		cout << "Opening: "<<file_name<<endl;
		TFile* in_file = new TFile (file_name, "READ");
		TTree* in_tree = (TTree*) in_file->Get("t");
		in_branch branch; 
		set_addresses(in_tree, branch);
		
		const int n_entries = in_tree->GetEntries();
        init_muons[file] = in_tree->GetEntries("abs(in_PDGid)==13");
		cout << "Found "<< n_entries<<" entries";
        cout << " including "<< init_muons[file] <<" muons"<<endl;

		for(int entry = 0; entry < n_entries; ++entry) {
			in_tree->GetEntry(entry);

            const int max_index = 500;
			int muon_ids_scint1 [max_index];	  // muons that have 'triggered'
			int electron_ids [max_index];		  // don't double count
			double muon_time_at_scint1[max_index]; // what we're actually going to plot
			// index bounds; don't want to loop over 500 zeros
			int n_seen_muons = 0;  
			int n_seen_electrons = 0;
			
			for(int i = 0; i < max_index; ++i) {
                muon_ids_scint1[i]     = -1;
                electron_ids[i]        = -1;
                muon_time_at_scint1[i] = -1.0;
            }

			for(int hit = 0; hit < branch.n_hits; ++hit) {
				// is it a muon in scint 1? 
				//	 first step?
				//	 add id to appropriate array
				//	 record time
				//	 ++ n_seen_scint1
				// is it an electron in scint 2 (only used downstream scint for e)
				//	 first step
				//	 is its parent id in the scint1 array (and hence from target)
				//	 dt >50 ns? 
				const int pid = branch.pdgid[hit];
				const int counter = branch.counter[hit];
				const int id = branch.id[hit];
				const bool first_step = branch.first_step[hit];
				if (abs(pid) == 13) {
					// muon stuff
					if (counter == 1 && first_step) {
						// first step in scint 1 now a triggering muon
						const int index = get_index(muon_ids_scint1, n_seen_muons, id);
						if (index == -1) {
							// new muon, log it!
							muon_ids_scint1[n_seen_muons] = id;
							muon_time_at_scint1[n_seen_muons++] = branch.tof[hit];
						} 
					}
				} else if (abs(pid) == 11) {
					// electron/positron stuff
					if (counter == 3 &&  first_step) {
						// first step in scint 2	
						const int e_index = get_index(electron_ids, n_seen_electrons, id);
						if (e_index == -1) {
							// new electron!
							electron_ids[n_seen_electrons++] = id; // log it!
							const int parent_index = get_index(muon_ids_scint1, n_seen_muons, branch.parentid[hit]);
							// have we seen its parent?
							if (parent_index != -1) {
								const double dt = branch.tof[hit] - muon_time_at_scint1[parent_index];
                                ++counts_no_cut[file];
                                if (dt > 50){
                                    ++counts[file];
                                }
							}
						}
					}
				}
			}
		}
	}
    printf("%16s %5s %5s %5s %5s %5s %5s %7s\n","Filename","int", "er", "mu in", "er", "acc %%", "er", "pre-cut");
    for(unsigned int file = 0; file < n_files; ++file) {
        const float count    = static_cast<float>(counts[file]);
        const float count_er = sqrt(count);
        const float count_frac_er = count_er/count;
        
        const float init     = static_cast<float>(init_muons[file]);
        const float init_er  = sqrt(init);
        const float init_frac_er = init_er/init;
        
        const float acc = 100.0*count/init;
        const float acc_er = acc * sqrt(count_frac_er*count_frac_er + init_frac_er*init_frac_er);
        
        printf ("%16s %5i %5.1f %5i %5.1f %5.1f %5.1f %7i\n",file_roots[file].Data(), 
                counts[file], count_er, init_muons[file], init_er, acc, acc_er, counts_no_cut[file]);
	}
}

void set_addresses(TTree* tree, in_branch& branch) {
	tree->SetBranchAddress("nhit",&branch.n_hits);
	tree->SetBranchAddress("trkid",&branch.id);
	tree->SetBranchAddress("pdgid", &branch.pdgid);
	tree->SetBranchAddress("counter", &branch.counter);
	tree->SetBranchAddress("parentid", &branch.parentid);
	tree->SetBranchAddress("first_step", &branch.first_step);
	tree->SetBranchAddress("tof", &branch.tof);
}

int get_index(const int* array, const int length, const int target) {
	for(int index = 0; index < length; ++index) {
		if (array[index] == target) {
			return index;
		}
	}
	return -1;
}