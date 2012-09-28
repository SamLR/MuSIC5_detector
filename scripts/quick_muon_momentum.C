#include "useful_for_root/drawing.C"

struct in_branch {
	int n_hits;
	int pdgid[500];
    int counter[500];
	bool first_step[500];
	double px[500];
	double py[500];
	double pz[500];
};

void set_addresses(const in_branch& mom, const TTree* tree);
TH1F* make_hist(const TString* file_root);
const double get_momentum(const in_branch& branch, const int hit);
TString get_counter_name(const int counter);

void quick_muon_momentum() {
	// pick the counter to measure the muon momentum at
	// using counter 5 (step prior to muons hitting the degrader)
	// will remove the 'first_step' requirement
	const int counter = 5;
	const TString counter_name = get_counter_name(counter);
	cout << "Generating muon momentums at counter: "<< counter_name << " id: "<< counter << endl;
	const int n_files = 6;
	const TString file_roots [n_files] = {
		"Air_5mm",
		"Aluminium_0.5mm",
		"Aluminium_1mm",
		"Aluminium_5mm",
		"Aluminium_8mm",
		"Aluminium_12mm"
	};
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
			for(unsigned int hit = 0; hit < branch.n_hits; ++hit) {
				if(branch.counter[hit]==counter &&  
					(branch.first_step[hit] || counter==5) && 
					abs(branch.pdgid[hit])==13){
					// if in the target, for the first time and a muon....
					
					hists[file]->Fill(get_momentum(branch, hit));
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
	TString title = "Muon momentum for different degraders at the "+counter_name;
	TString save_location = "../../../images/final_analysis/muon_momentum_at_"+counter_name+".svg";
	draw_pretty_hists(n_files,hists,title,file_roots, save_location,1002201);
}

void set_addresses(const in_branch& branch, const TTree* tree) {
	tree->SetBranchAddress("nhit",&branch.n_hits);
	tree->SetBranchAddress("pdgid", &branch.pdgid);
    tree->SetBranchAddress("counter", &branch.counter);
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

TString get_counter_name(const int counter) {
	switch (counter) {
		case 1: return TString("scint_1");
		case 2: return TString("target");
		case 3: return TString("scint_2");
		case 4: return TString("degrader");
		case 5: return TString("beam_pipe_exit");
		default: return TString("Something when horrible wrong, a dog, perhaps.");
	}
}
