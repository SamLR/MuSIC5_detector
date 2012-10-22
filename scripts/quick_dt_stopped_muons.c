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

void set_addresses(const in_branch& mom, const TTree* tree);
TH1F* make_hist(const TString* file_root);
const int get_index(const int* array, const int length, const int target);

void quick_dt_stopped_muons() {
	const int n_files = 6;
	const TString file_roots [n_files] = { "Air_5mm",
		"Aluminium_0.5mm",
		"Aluminium_1mm",
		"Aluminium_5mm",
		"Aluminium_8mm",
		"Aluminium_12mm" };

    const TString abs_path = "~/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector/scripts/";
	const TString prefix = abs_path+"../../output/final/final_st_Copper_0.5mm_deg_";
	const TString suffix = ".root";
    const TString out_file_root = "dt_hists";
    
    // create the out file in the local dir
    const TString out_file_name = abs_path+out_file_root + suffix;
    TFile* out_file = new TFile(out_file_name, "RECREATE");
    
	TH1F* hists [n_files];
	
    int integrals[n_files];  // integral of stopped muon hist (i.e. no under/overflow)
    int counts[n_files];     // number of entries in the stopped muon hist
    int init_muons[n_files]; // the number of input muons from g4bl.

	for(unsigned int file = 0; file < n_files; ++file) {
		TString file_name = prefix + file_roots[file] + suffix;
		cout << "Opening: "<<file_name<<endl;
		TFile* in_file = new TFile (file_name, "READ");
		TTree* in_tree = (TTree*) in_file->Get("t");
		in_branch branch; 
		set_addresses(branch, in_tree);
		// make sure the histograms are saved in the out file
        out_file->cd();
		hists[file] = make_hist(file_roots[file]);
		const int n_entries = in_tree->GetEntries();
        init_muons[file] = in_tree->GetEntries("abs(in_PDGid)==13");
		cout << "Found "<< n_entries<<" entries";
        cout << " including "<< init_muons[file] <<" muons"<<endl;

		for(unsigned int entry = 0; entry < n_entries; ++entry) {
			in_tree->GetEntry(entry);

            const int max_index = 500;
			int muon_ids_scint1 [max_index];	  // muons that have 'triggered'
			int electron_ids [max_index];		  // don't double count
			double muon_time_at_scint1[max_index]; // what we're actually going to plot
			// index bounds; don't want to loop over 500 zeros
			int n_seen_muons = 0;  
			int n_seen_electrons = 0;
			
			for(unsigned int i = 0; i < max_index; ++i) {
                muon_ids_scint1[i]     = -1;
                electron_ids[i]        = -1;
                muon_time_at_scint1[i] = -1.0;
            }

			for(unsigned int hit = 0; hit < branch.n_hits; ++hit) {
				// is it a muon in scint 1? 
				//	 first step?
				//	 add id to appropriate array
				//	 record time
				//	 ++ n_seen_scint1
				// is it an electron in scint 1 or 2
				//	 first step
				//	 is its parent id in the scint1 array (and hence from target)
				//	 plot dt
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
					if (counter == 3 &&  first_step) {// in realistic set up we only use scint 2
						// first step in scint 2	
						const int e_index = get_index(electron_ids, n_seen_electrons, id);
						if (e_index == -1) {
							// new electron!
							electron_ids[n_seen_electrons++] = id; // log it!
							const int parent_index = get_index(muon_ids_scint1, n_seen_muons, branch.parentid[hit]);
							// have we seen its parent?
							if (parent_index != -1) {
								const double dt = branch.tof[hit] - muon_time_at_scint1[parent_index];
								hists[file]->Fill(dt);
							}
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
    
    
    // TString title = "Muon decay times";
    // TString save_location = "images/muon_decay_times.svg";
    // // 1002201 is the magic number for stats
    // draw_pretty_hists(n_files,hists,title,file_roots, save_location,1002201);
	
    printf("%16s %5s %5s %5s %5s\n","Filename","int", "count", "mu in","eff");
	for(unsigned int file = 0; file < n_files; ++file) {
		hists[file]->Rebin(100);
        const float eff = 100.0*integrals[file]/((float)init_muons[file]);
        printf ("%16s %5i %5i %5i %5.1f%%\n",file_roots[file].Data(), 
                integrals[file], counts[file], init_muons[file], eff);
	}
	
    TString title2 = "Muon decay times (100ns bins)";
    TString save_location2 = "images/muon_decay_times_rebin.svg";
    // 1002201 is the magic number for stats
    draw_pretty_hists(n_files,hists,title2,file_roots, save_location2,1112201);//1002201);
}

void set_addresses(const in_branch& branch, const TTree* tree) {
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

const double get_momentum(const in_branch& branch, const int hit){
	const double px = branch.px[hit];
	const double py = branch.py[hit];
	const double pz = branch.pz[hit];

	return sqrt(px*px + py*py + pz*pz);
}

const int get_index(const int* array, const int length, const int target) {
	for(int index = 0; index < length; ++index) {
		if (array[index] == target) {
			return index;
		}
	}
	return -1;
}