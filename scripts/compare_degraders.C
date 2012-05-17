// FIXME header info
// FIXME look at includes within root scipts, 
//      some of this should be turned into a library

#define MAX_HIT 500
struct branches_struct
{
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
TFile* init_file(const TString &filename, const bool& verbose=false)
{
    TFile* file = new TFile(filename, "READ");
    if (!file->IsOpen()) 
    { // if the file didn't open an object is still created but isOpen will 
    // throw an error. This is damn stupid
        exit(1); // if it didn't load something's wrong
    } else
    {
        if (verbose) cout << filename << " sucessfully opened"<< endl;
        return file;
    }
}

TTree* init_tree(const TFile* file, const TString& treename, const bool& verbose=false)
{
    TTree* tree = (TTree*) file->Get(treename);
    if(!tree)
    {
        if(verbose) cout << "ERROR: " << treename << " not found" << endl;
        exit(1); // if it didn't load something's wrong
    } else
    {
        if(verbose) cout << "Returning "<< treename << endl;
        return tree;
    }
}

void set_address(const branches_struct& branch, const TTree* tree)
{ // assign the addresses of the various brances
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

void test()
{
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

void process_canvas(TString fileroot, TCanvas* canvas)
{
    canvas = new TCanvas(fileroot, fileroot);
    canvas->cd();
}

void loop_entries(const TTree* tree,
                  const TH1F* histo,
                  const branches_struct& branch,
                  // func to fill the return histogram
                  void (*cut)(const branches_struct&, const int, const TH1F*), 
                  // want tons of stdout?
                  const bool& verbose=false) 
{ // return a histogram filled according to apply_cut
    
    const unsigned int n_entries = tree->GetEntries();    
    
    if(verbose) cout << "Tree loaded. "<< n_entries << " entries found." << endl;
    
    for(unsigned int entry = 0; entry < n_entries; ++entry)
    { // loop over each entry and see if it passes the cuts
        tree->GetEntry(entry);
        const unsigned int n_hits = branch.g_nhit;
        if(verbose && (entry%1000==0)) cout << "Entry: " << entry<< endl;
        for(unsigned int hit = 0; hit < n_hits; ++hit)
        {
            cut(branch, hit, histo);
        }
    }
    if(verbose) cout << "filling finished" << endl;
}

double length(const double& x, const double& y, const double& z)
{
    return sqrt(x*x + y*y + z*z);
}

void basic_cut(const branches_struct& branch, const int hit, const TH1F* hist)
{
    if (abs(branch.pdgid [hit]) == 13) 
    {
        const double momentum = length(branch.px[hit],branch.py[hit],branch.pz[hit]);
        hist->Fill(momentum);
    }
}

void compare_air()
{
    
}

void compare_degraders()
{
    // test();
    TString file_prefix = "../../output/";
    TString file_suffix = ".root";
    
    compare_air();
}