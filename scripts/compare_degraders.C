// FIXME header info
// FIXME look at includes within root scipts, 
//      some of this should be turned into a library

#define MAX_HIT 500

// template <class T>
typedef void (*cut_func_ptr)(const in_branch_struct&, const int, const T*);

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

struct out_branch_struct{
    // structure to be saved for future analysis
    int event;
    int procid;
    int counter;
    int trkid;
    int parentid;
    int pdgid;
    double x;
    double y;
    double z;
    double px;
    double py;
    double pz;
    double edep;
    double tof;
};

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

TTree* init(const TString& filename,  const in_branch_struct& branch){
    // open the file, create and return a tree with assigned branches
    TFile* file = init_file(filename);
    TTree* tree = init_tree(file, 't');
    set_in_branch_address(branch, tree);
    return tree;
}

TH1F* init_hist(const TString& name, 
    const int& n_bins,
    const int& min_x, 
    const int& max_x, 
    const TString& xaxis_name, 
    const TString& yaxis_name,
    const int& linecolor
) {

    TH1F* hist = new TH1F(name, name, n_bins, min_x, max_x);
    hist->GetXaxis()->SetTitle(xaxis_name);
    hist->GetYaxis()->SetTitle(yaxis_name);
    hist->SetLineColor(linecolor);
    return hist;
}                

void set_in_branch_address(const in_branch_struct& branch, const TTree* tree) { // assign the addresses of the various brances
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

void set_out_branch_address(const out_branch_struct& branch, const TTree* tree){
    tree->Branch("event_id",&branch.event,"event/I"); // int
    tree->Branch("process_id",&branch.procid,"process_id/I"); 
    tree->Branch("hit_volume",&branch.counter, "hit_volume/I");
    tree->Branch("track_id",&branch.trkid,"track_id/I");
    tree->Branch("parent_id",&branch.parentid, "parent_id/I");
    tree->Branch("pid",&branch.pdgid, "pid/I");
    tree->Branch("pos_x",&branch.x,"pos_x/D"); // double
    tree->Branch("pos_y",&branch.y,"pos_y/D");
    tree->Branch("pos_z",&branch.z,"pos_z/D");
    tree->Branch("mom_x",&branch.px, "mom_x/D");
    tree->Branch("mom_y",&branch.py, "mom_y/D");
    tree->Branch("mom_z",&branch.pz, "mom_z/D");
    tree->Branch("deposited_energy",&branch.edep, "deposited_energy/D");
    tree->Branch("time_of_flight",&branch.tof, "time_of_flight/D");
}

void test() {
    TFile* file = init_file("../../output/run_Air_0.2mm.root", true); // this should pass
// TFile* file = init_file("../../output/MONKEY.root", true); // this should fail
    TTree* tree = init_tree(file, "t", true); // pass
// TTree* tree = init_tree(file, "B", true); // fail
    in_branch_struct branch;

    set_in_branch_address(branch, tree);
    cout << "tree loaded with " << tree->GetEntries() << " entries" <<endl;
// prove something's been loaded
    TH1F* hist = new TH1F("out", "out", 200, 0, 200);
    loop_entries(tree, hist, branch, &basic_cut, true);
    hist->Draw();
}

template <class T>
void loop_entries(const TTree* in_tree, // where the data's loaded
    // const TH1F* cut_to_object, // object to be passed to cut
    const T* cut_to_object, // object to be passed to cut
    const in_branch_struct& branch,
        // the cut fuction uses the data in the branch at hit to fill the TObject
    // void (*cut)(const in_branch_struct&, const int, const TH1F*),
    const unsigned int n_funcs,
    cut_func_ptr* cuts,
        // want tons of stdout?
    const bool& verbose=false
)  {
        // return a histogram filled according to apply_cut

    const unsigned int n_entries = in_tree->GetEntries();    
    // const unsigned int n_entries = 100;

    if(verbose) cout << "Tree loaded. "<< n_entries << " entries found." << endl;

    for(unsigned int entry = 0; entry < n_entries; ++entry) { 
        // loop over each entry and see if it passes the cuts
        if(verbose && (entry%1000==0)) cout << "Entry: " << entry<< endl;
        in_tree->GetEntry(entry);
        const unsigned int n_hits = branch.g_nhit;
        if( n_hits==0 ) continue; // skip entries with no events
        for(unsigned int hit = 0; hit < n_hits; ++hit) {
            for(unsigned int func = 0; func < n_funcs; ++func) {
                // Dark voodoo! loop over the array of function pointers
                // calling each function in turn
                (*cuts[func])(branch, hit, cut_to_object);
            }
            // cut(branch, hit, cut_to_object);
        }
    }
    if(verbose) cout << "filling finished" << endl;
}

double length(const double& x, const double& y, const double& z) {
    return sqrt(x*x + y*y + z*z);
}

// DONT USE BASIC CUT! 
void basic_cut(const in_branch_struct& branch, const int hit, const TH1F* hist) {
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

void smart_cut_muons(const in_branch_struct& branch, const int hit, const TH1F* hist) {
    static vector<int> seen_tracks;
    if(hit == 0) {
        seen_tracks.clear();
    } // new event so reset this

    if(abs(branch.pdgid[hit]) != 13) return; // not a muon? return

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
    TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
    legend->SetHeader("thickness of 'air' degrader");

    cout << "Begin" << endl;

    for(int i = 0; i < n_files; ++i) {
        TString filename = prefix + file_list[i] + suffix; // make the full location
        // 
        // // initialise things
        // TFile* file  = init_file(filename);
        // TTree* tree = init_tree(file, "t");

        in_branch_struct branch;
        // set_in_branch_address(branch, tree);

        TTree* tree = init(filename, branch);

        cout << "Tree: `t` from " << file_list[i] << " now loaded"<<endl;
        cout << "Expect " << tree->GetEntries("(abs(pdgid)==13)&&counter==1") << " muons." << endl;
        // make the histogram
        const int n_bins = 50, minx = 0, maxx = 200;
        const TString xtitle = "Momentum(MeV)";
        const TString ytitle = "count";
        TString title="Comparison of muon momentum with Air degrader";
        histograms[i] = 
            init_hist(title, n_bins, minx, maxx, xtitle, ytitle, colours[i]);
        loop_entries(tree, histograms[i], branch, &smart_cut_muons, true);

        legend->AddEntry(histograms[i], file_list[i]);
        histograms[i]->Draw(draw_opts[i]); 
    }
    legend->Draw();
}

void check_tracks(const TString& prefix, const TString& suffix) {
    TString filename = prefix + "run_Air_5mm" + suffix;
    TFile* file = init_file(filename); 
    TTree* tree = init_tree(file, "t");
    in_branch_struct branch;
    set_in_branch_address(branch,tree);

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

TH1F* gen_hist(const TString& filename, const TString& title, 
    const TFile* save_location, cut_func_ptr cut
) {
    in_branch_struct branch;
    TTree* tree = init(filename, branch); //initialise the tree for air
    TString title_xaxis = "Momentum (MeV)";
    TString title_yaxis = "Count";
    save_location->cd();
    TH1F* hist = init_hist(title, 50, 0, 200, title_xaxis, title_yaxis, 1);
    cut_func_ptr* cuts [] = {cut};
    loop_entries<TH1F>(tree, hist, branch, 1, cuts, true);
    return hist;
}

TTree* gen_tree(const TFile* save_to, const TString& in_filename, 
    const TString out_tree_name, const TString& out_tree_description,
    const unsigned int n_funcs,
    cut_func_ptr* cuts
){
    // generate trees according to some cut 
    in_branch_struct in_branch;
    TTree* in_tree = init(in_filename, in_branch);
    save_to->cd();
    TTree* out_tree = new TTree(out_tree_name, out_tree_description);
    // add origin info to the tree
    out_tree->GetUserInfo()->Add(new TObjString(in_filename));
    loop_entries<TTree>(in_tree, out_tree, in_branch, n_funcs, cuts, true);
    return in_tree;
}

void tree_cutter(const in_branch_struct& branch, const int hit, const TTree* tree){
    static out_branch_struct out_branch;
    static bool initialised = false;
    if(!initialised){
        set_out_branch_address(out_branch, tree);
        initialised=true;
    }
    // select only muons in counters 1 or 2
    if(abs(branch.pdgid[hit]) != 13)  return; // not muon
    if(!(branch.counter[hit]==1 || branch.counter[hit]==3)) return; // not in scint
    
    out_branch.event   = branch.in_event;
    out_branch.procid  = branch.procid[hit];
    out_branch.counter = branch.counter[hit];
    out_branch.trkid   = branch.trkid[hit];
    out_branch.parentid= branch.parentid[hit];
    out_branch.pdgid   = branch.pdgid[hit];
    out_branch.x       = branch.x[hit];
    out_branch.y       = branch.y[hit];
    out_branch.z       = branch.z[hit];
    out_branch.px      = branch.px[hit];
    out_branch.py      = branch.py[hit];
    out_branch.pz      = branch.pz[hit];
    out_branch.edep    = branch.edep[hit];
    out_branch.tof     = branch.tof[hit];
    
    tree->Fill();
}

void draw_comparison_hists(TCanvas* can, 
    const TH1F* titleHist, const TH1F* backHist,
    const TString img_save_location
){
    TString title = titleHist->GetTitle();
    can = new TCanvas(title, title);
    backHist->Draw();
    titleHist->SetLineColor(2);
    titleHist->Draw("SAME");
        // create a legend for extra info
    TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
    leg->SetHeader(title);
    leg->AddEntry(titleHist); // add the two histrograms to the legend
    leg->AddEntry(backHist);
    leg->SetFillColor(0);
    leg->Draw();

    can->SaveAs(img_save_location);
}

void compare_degraders() {
// test();

// FIXME this is starting to break the rule of 7 things

    cout << "THIS IS DEPRICATED DONT USE" << endl;
    return;
    
    TString image_dir = "../../../images/";
    TString prefix = "../../output/";
    TString suffix = ".root";
    TString save_name = prefix+"cut_data"+suffix;
    TFile* save_to = new TFile(save_name, "RECREATE");
    // open a file with no degrader (only air) and create a hist of muon momentum
    TString air_file = "run_Air_5mm"; // file to use as the base line
    cout << "Air file: " << air_file << endl;
    // TH1F* air_hist = gen_hist( (prefix + air_file + suffix), air_file, save_to,&smart_cut_muons);
    // air_hist->SetTitle("No degrader");
    
    cut_func_ptr *cuts [] = {&tree_cutter};
    TTree* air_tree = gen_tree(save_to, (prefix + air_file + suffix),"air", 
        "muons at scint 1 & 2", 1, cuts);
    
    // now generate the comparisons
    TString degrader_files [] = {"run_Aluminium_0.2mm",
        "run_Aluminium_10mm",
        "run_Aluminium_1mm",
        "run_Aluminium_2mm",
        "run_Aluminium_5mm",
        "run_Polystyrene_0.2mm",
        "run_Polystyrene_10mm",
        "run_Polystyrene_1mm",
        "run_Polystyrene_2mm",
        "run_Polystyrene_5mm"};
    
    const unsigned int n_files = 10; // hack just to test first 2
    // TH1F* hists [n_files];
    // TCanvas* canvases [n_files];
    TTree* trees [n_files];
    
    for(unsigned int file = 0; file < n_files; ++file) {
        // generate the histogram
        cout <<endl << "File: " << degrader_files[file] << endl;
        TString filename = prefix + degrader_files[file] + suffix;
        // hists[file] = gen_hist(filename, // which file is being opened
        //     degrader_files[file], // file root for use as title etc
        //     save_to, // where to save the histogram
        //     &smart_cut_muons); // which cut function to use
        // // draw on a new canvas with the air histogram for comparison
        // TString img_file = image_dir+degrader_files[file]+".eps";
        // draw_comparison_hists(canvases[file], hists[file], air_hist, img_file);
        trees[file] = gen_tree(save_to, filename, degrader_files[file], 
            "muons at scint 1 & 2", 1, cuts);
        
    }    
    save_to->Write();
    
}







