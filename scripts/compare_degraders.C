// FIXME header info
// FIXME look at includes within root scipts, 
//      some of this should be turned into a library

#define MAX_HIT 500
struct branches_struct {
    int g_iev;
    int in_event;
    int in_trackID;
    int in_pdg;
    double in_x; // use the new (AKA global) values for these
    double in_y; 
    double in_z;
    double in_px;
    double in_py;
    double in_pz;
    double in_tof;
    int g_nhit;
    int procid[MAX_HIT];
    int counter[MAX_HIT];
    int trkid[MAX_HIT];
    int parentid[MAX_HIT];
    int pdgid[MAX_HIT];
    double x[MAX_HIT];
    double y[MAX_HIT];
    double z[MAX_HIT];
    double px[MAX_HIT];
    double py[MAX_HIT];
    double pz[MAX_HIT];
    double edep[MAX_HIT];
    double tof[MAX_HIT];
};

// FIXME Move init file & tree to a header? 
// FIXME these should probably return smart pointers
TFile* init_file(const TString &filename, const bool& verbose=false) {
    TFile* file = new TFile(filename, "READ");
    if (!file->IsOpen()) { 
        exit(1); // if it didn't load something's wrong
    } else {
        if (verbose) cout << filename << " sucessfully opened"<< endl;
        return file;
    }
}

TTree* init_tree(const TFile* file, const TString& treename, const bool& verbose=false) {
    TTree* tree = (TTree*) file->Get(treename);
    if(!tree) {
        if(verbose) cout << "ERROR: " << treename << " not found" << endl;
        exit(1); // if it didn't load something's wrong
    } else {
        if(verbose) cout << "Returning "<< treename << endl;
        return tree;
    }
}

TH1F* init_hist(const TString& name, 
    const int& n_bins,
    const int& min_x, 
    const int& max_x, 
    const TString& xaxis_name, 
    const TString& yaxis_name,
const int& linecolor) {

    TH1F* hist = new TH1F(name, name, n_bins, min_x, max_x);
    hist->GetXaxis()->SetTitle(xaxis_name);
    hist->GetYaxis()->SetTitle(yaxis_name);
    hist->SetLineColor(linecolor);
    return hist;
}                


void set_address(const branches_struct& branch, const TTree* tree) { // assign the addresses of the various brances
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

void test() {
    TFile* file = init_file("../../output/run_Air_0.2mm.root", true); // this should pass
// TFile* file = init_file("../../output/MONKEY.root", true); // this should fail
    TTree* tree = init_tree(file, "t", true); // pass
// TTree* tree = init_tree(file, "B", true); // fail
    branches_struct branch;

    set_address(branch, tree);
    cout << "tree loaded with " << tree->GetEntries() << " entries" <<endl;
// prove something's been loaded
    TH1F* hist = new TH1F("out", "out", 200, 0, 200);
    loop_entries(tree, hist, branch, &basic_cut, true);
    hist->Draw();
}

void process_canvas(TString fileroot, TCanvas* canvas) {
    canvas = new TCanvas(fileroot, fileroot);
    canvas->cd();
}

void loop_entries(const TTree* tree,
    const TH1F* histo,
    const branches_struct& branch,
              // func to fill the return histogram
    void (*cut)(const branches_struct&, const int, const TH1F*), 
              // want tons of stdout?
const bool& verbose=false)  { 
            // return a histogram filled according to apply_cut

    const unsigned int n_entries = tree->GetEntries();    

    if(verbose) cout << "Tree loaded. "<< n_entries << " entries found." << endl;

    for(unsigned int entry = 0; entry < n_entries; ++entry) { // loop over each entry and see if it passes the cuts
        tree->GetEntry(entry);
        const unsigned int n_hits = branch.g_nhit;
        if(verbose && (entry%1000==0)) cout << "Entry: " << entry<< endl;
        for(unsigned int hit = 0; hit < n_hits; ++hit) {
            cut(branch, hit, histo);
        }
    }
    if(verbose) cout << "filling finished" << endl;
}

double length(const double& x, const double& y, const double& z) {
    return sqrt(x*x + y*y + z*z);
}


// DONT USE BASIC CUT! 
void basic_cut(const branches_struct& branch, const int hit, const TH1F* hist) {
    cout << "You probably shouldn't be using this" << endl;
    if (abs(branch.pdgid [hit]) == 13) {
        const double momentum = length(branch.px[hit],branch.py[hit],branch.pz[hit]);
        hist->Fill(momentum);
    }
}

template <class T>
bool is_in(const vector<T> vec, const T& target){
    for(vector<T>::iterator iter = vec.begin(); iter < vec.end(); ++iter){
        if (*iter == target) return true;
    }
    return false;
}

void smart_cut_muons(const branches_struct& branch, const int hit, const TH1F* hist) {
    static vector<int> seen_tracks;
    if(hit == 0) seen_tracks.clear(); // new event so reset this
    
    if(abs(branch.pdgid[hit] != 13)) return; // not a muon? return
    
    if(branch.counter[hit] != 1) return; // not in scint 1? return
    
    if(is_in(seen_tracks, branch.trkid[hit])) return; // seen it already
    
    // fill the histogram with the momentum
    hist->Fill(length(branch.px[hit],branch.py[hit],branch.pz[hit]));
    // note that we've now seen this track
    seen_tracks.push_back(branch.trkid[hit]);
}
// FIXME This should probably all be re done using some sort of class structure
void compare_air(const TString& prefix, const TString& suffix) {
    TString file_list [] = {"run_Air_0.2mm",
        "run_Air_10mm",
        "run_Air_5mm"};
    TString draw_opts [] = {"", "SAME", "SAME"};
    const int colours [] = {1, 2, 4};

    const int n_files = 3;
    TH1F* histograms [] = {NULL, NULL, NULL}; 

    cout << "Begin" << endl;

    for(int i = 0; i < n_files; ++i) {
        
        TString title = file_list[i]; // alias
        TString filename = prefix + title + suffix; // make the full location
        
        // initialise things
        TFile* file  = init_file(filename);
        TTree* tree = init_tree(file, "t");
        
        branches_struct branch;
        set_address(branch, tree);
        
        cout << "Tree: `t` from " << title << " now loaded"<<endl;
        // make the histogram
        const int n_bins = 200, minx = 0, maxx = 200;
        const TString xtitle = "Momentum(MeV)";
        const TString ytitle = "count";
        histograms[i] = 
            init_hist(title, n_bins, minx, maxx, xtitle, ytitle, colours[i]);
        loop_entries(tree, histograms[i], branch, &smart_cut_muons, true);
        // loop_entries(tree, histograms[i], branch, &basic_cut, true);
        
        histograms[i]->Draw(draw_opts[i]); 
    }

}

void check_tracks(const TString& prefix, const TString& suffix) {
    TString filename = prefix + "run_Air_5mm" + suffix;
    TFile* file = init_file(filename); 
    TTree* tree = init_tree(file, "t");
    branches_struct branch;
    set_address(branch,tree);
    
    // map<int, int> track_counts; 
    unsigned int n_entries = tree->GetEntries();
    unsigned int n_muons = 0;
    for(unsigned int entry = 0; entry < n_entries; ++entry){
        tree->GetEntry(entry);
        vector<int> seen_tracks;
        vector<int> uniq_tracks;
        if(branch.g_nhit==0) continue;
        for(unsigned int hit = 0; hit < branch.g_nhit; ++hit){
            if( abs(branch.pdgid[hit])!=13 ) continue;
            if( branch.counter[hit]!=1 ) continue;
            bool seen = false;
            seen_tracks.push_back(branch.trkid[hit]);
            for(unsigned int i = 0; i < uniq_tracks.size(); ++i) {
                if( branch.trkid[hit] == uniq_tracks[i]) seen=true;
            }
            if(seen) continue;
            
            ++n_muons;
            uniq_tracks.push_back(branch.trkid[hit]);
        }
        
        for(unsigned int i = 0; i < seen_tracks.size(); ++i) {
            cout << "seen track:" << seen_tracks[i] << endl;
        }
        cout << "*********" << endl;
    }
    cout << "unique muon tracks: " << n_muons << endl;
    cout << "GetEntries() result:" << tree->GetEntries("(pdgid==13||pdgid==-13)&&counter==1") << endl;
}

void compare_degraders() {
// test();
    TString file_prefix = "../../output/";
    TString file_suffix = ".root";
    // check_tracks(file_prefix, file_suffix);
    compare_air(file_prefix, file_suffix);
}