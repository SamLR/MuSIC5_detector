#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"

#include <iostream>
#include "useful_for_root/drawing.C"

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
TH1F* make_hist(const TString file_root);
int get_index(const int* array, const int length, const int target);
bool is_charged(const int pdgid);

void make_inclusive_dt_hists() {
	const int n_files = 4;
	const TString file_roots [n_files] = { "Air_5mm",
		"Aluminium_0.5mm",
		"Aluminium_1mm",
		"Aluminium_5mm" };

	const TString abs_path = "~/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector/scripts/";
	const TString prefix = abs_path+"../../output/final/final_st_Copper_0.5mm_deg_";
	const TString suffix = ".root";
	const TString out_file_root = "inclusive_dt_hists";
	
	// create the out file in the local dir
	const TString out_file_name = abs_path+out_file_root + suffix;
	TFile* out_file = new TFile(out_file_name, "RECREATE");
	
	TH1F* hists [n_files];
	
	int integrals[n_files];	 // integral of stopped muon hist (i.e. no under/overflow)
	int counts[n_files];	 // number of entries in the stopped muon hist

	for(unsigned int file = 0; file < n_files; ++file) {
		TString file_name = prefix + file_roots[file] + suffix;
		cout << "Opening: "<<file_name<<endl;
		TFile* in_file = new TFile (file_name, "READ");
		TTree* in_tree = (TTree*) in_file->Get("t");
		in_branch branch; 
		set_addresses(in_tree, branch);
		// make sure the histograms are saved in the out file
		out_file->cd();
		hists[file] = make_hist(file_roots[file]);
		const int n_entries = in_tree->GetEntries();
		cout << "Found "<< n_entries<<" entries"<<endl;

		for(int entry = 0; entry < n_entries; ++entry) {
			in_tree->GetEntry(entry);

			const int max_index = 500;
			int parent_ids_scint1 [max_index];
			int chid_ids [max_index];
			double time_at_scint1[max_index]; // what we're actually going to plot
			// index bounds; don't want to loop over 500 zeros
			int n_seen_parent = 0;	
			int n_seen_child = 0;
			
			for(int i = 0; i < max_index; ++i) {
				parent_ids_scint1[i]	   = -1;
				chid_ids[i]		   = -1;
				time_at_scint1[i] = -1.0;
			}

			for(int hit = 0; hit < branch.n_hits; ++hit) {
				// is it a charged particle in scint 1? 
				//	 first step?
				//	 add id to appropriate array
				//	 record time
				//	 ++ n_seen_parent
				// is it a charged in scint 2
				//	 first step
				//   check it's not the primary particle for the run 
				//	 is its parent id in the scint1 array 
				//	 plot dt
				const int pid = branch.pdgid[hit];
				if (not is_charged(pid)) continue; // can't detect it
				
				const int counter = branch.counter[hit];
				const int id = branch.id[hit];
				const bool first_step = branch.first_step[hit];
				
				// remember parents can also be daughters!
				if (counter == 1 && first_step) {
					// first step in scint 1 now a triggering muon
					const int index = get_index(parent_ids_scint1, n_seen_parent, id);
					if (index == -1) {
							// new muon, log it!
						parent_ids_scint1[n_seen_parent] = id;
						time_at_scint1[n_seen_parent++] = branch.tof[hit];
					} 
				}
					// electron/positron stuff
				if (counter == 3 && first_step && (id != 0)) {
					// first step in scint 2
					const int index = get_index(chid_ids, n_seen_child, id);
					if (index == -1) {
						// new child particle!
						chid_ids[n_seen_child++] = id; // log it!
						const int parent_index = get_index(parent_ids_scint1, n_seen_parent, branch.parentid[hit]);
						// have we seen its parent?
						if (parent_index != -1) {
							const double dt = branch.tof[hit] - time_at_scint1[parent_index];
							hists[file]->Fill(dt);
						}
					}
				}
			}
		}
		// 'GetEntries()' includes over & under flow bins, integral only includes the bounds
		integrals[file] = hists[file]->Integral();
		counts[file] = hists[file]->GetEntries();
		cout << "File done, "<< integrals[file] << " stopped muons." << endl;	
	}
	
	out_file->Write();
	
	printf("%16s %5s %5s\n","Filename","int", "count");
	for(unsigned int file = 0; file < n_files; ++file) {
		hists[file]->Rebin(100);
		printf ("%16s %5i %5i\n",file_roots[file].Data(), integrals[file], counts[file]);
	}
	
	TString title2 = "Charged particle decay times (100ns bins)";
	TString save_location2 = "images/charged_decay_times_rebin.svg";
	// 1002201 is the magic number for stats
	draw_pretty_hists(n_files,hists,title2,file_roots, save_location2,1112201);//1002201);
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

TH1F* make_hist(const TString file_root) {
	TString name = "parent-daughter_dts_for_"+file_root;
	TH1F* res = new TH1F(name,name, 20000, 1, 20001);
	res->GetXaxis()->SetTitle("Delta time (ns)");
	res->GetYaxis()->SetTitle("Count");
	return res;
}

int get_index(const int* array, const int length, const int target) {
	for(int index = 0; index < length; ++index) {
		if (array[index] == target) {
			return index;
		}
	}
	return -1;
}

bool is_charged(const int pdgid) {
	if (pdgid == -211 || pdgid == 211 || // pion
		pdgid ==  -13 || pdgid == 13  || // muon
		pdgid ==  -11 || pdgid == 11  || // electron
		pdgid == 2212) {				 // proton
		return true;
	} else {
		return false;
	}
}

