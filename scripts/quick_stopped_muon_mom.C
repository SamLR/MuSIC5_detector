#include "useful_for_root/drawing.C"

struct in_branch {
	int n_hits;
	int id[500];
	int pdgid[500];
	int counter[500];
	int parentid[500];
	bool first_step[500];
	double px[500];
	double py[500];
	double pz[500];
};

void set_addresses(const in_branch& mom, const TTree* tree);
TH1F* make_hist(const TString* file_root);
const double get_momentum(const in_branch& branch, const int hit);
const int get_index(const int* array, const int length, const int target);

void quick_stopped_muon_mom() {
	const int n_files = 6;
	const TString file_roots [n_files] = { "Air_5mm",
		"Aluminium_0.5mm",
		"Aluminium_1mm",
		"Aluminium_5mm",
		"Aluminium_8mm",
		"Aluminium_12mm" };

	const TString prefix = "../../output/final/final_st_Copper_0.5mm_deg_";
	const TString suffix = ".root";

	TH1F* hists [n_files];

	for(unsigned int file = 0; file < n_files; ++file) {
		TString file_name = prefix + file_roots[file] + suffix;
		cout << "Opening: "<<file_name<<endl;
		TFile* in_file = new TFile (file_name, "READ");
		TTree* in_tree = (TTree*) in_file->Get("t");
		in_branch branch; 
		set_addresses(branch, in_tree);
		hists[file] = make_hist(file_roots[file]);
		const int n_entries = in_tree->GetEntries();
		cout << "Found "<< n_entries<<" entries"<<endl;

		for(unsigned int entry = 0; entry < n_entries; ++entry) {
			in_tree->GetEntry(entry);

			int muon_ids_pre_deg [500];   // ensure we've logged a momentum
			int muon_ids_scint1 [500];    // muons that have 'triggered'
			int electron_ids [500];       // don't double count
			double muon_mom_pre_deg[500]; // what we're actually going to plot
			// index bounds; don't want to loop over 500 zeros
			int n_seen_pre_deg = 0;  
			int n_seen_electrons = 0;

			for(unsigned int hit = 0; hit < branch.n_hits; ++hit) {
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
						} else {
							// this branch shouldn't execute
							cout << "We've not see this one "<< id <<endl;
							for(unsigned int i = 0; i < n_seen_pre_deg; ++i) {
								cout << muon_ids_pre_deg[i] << endl;
							}
						}
					}
				} else if (abs(pid) == 11) {
					// electron/positron stuff
					if ((counter == 1 || counter == 3) &&  first_step) {
						// first step in scint 1 or 2	
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
		cout << "File done" << endl;	
	}

	TCanvas* canvases [n_files];

	for(unsigned int hist = 0; hist < n_files; ++hist) {
		canvases[hist] = new TCanvas(file_roots[hist],file_roots[hist]);
		hists[hist]->Draw();
	}

	TString title = "Stopped muon momentum";
	TString save_location = "images/stopped_muon_momentum.svg";
	// 1002201 is the magic number for stats
	draw_pretty_hists(n_files,hists,title,file_roots, save_location,1002201);
}

void set_addresses(const in_branch& branch, const TTree* tree) {
	tree->SetBranchAddress("nhit",&branch.n_hits);
	tree->SetBranchAddress("trkid",&branch.id);
	tree->SetBranchAddress("pdgid", &branch.pdgid);
	tree->SetBranchAddress("counter", &branch.counter);
	tree->SetBranchAddress("parentid", &branch.parentid);
	tree->SetBranchAddress("first_step", &branch.first_step);
	tree->SetBranchAddress("px", &branch.px);
	tree->SetBranchAddress("py", &branch.py);
	tree->SetBranchAddress("pz", &branch.pz);
}

TH1F* make_hist(const TString file_root) {
	TString name = "Muon_momentum_with_"+file_root;
	TH1F* res = new TH1F(name,name, 50, 0, 200);
	res->GetXaxis()->SetTitle("Momentum (MeV/c)");
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