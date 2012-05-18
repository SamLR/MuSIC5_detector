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

template <class T>
bool is_in(const vector<T> vec, const T& target)
{
    for(vector<T>::iterator iter = vec.begin(); iter < vec.end(); ++iter)
    {
        if (*iter == target) return true;
    }
    return false;
}

double length(double x, double  y, double  z)
{
    return sqrt(x*x + y*y + z*z);
}

void test_is_in()
{
    vector<int> d;
    d.push_back(10);
    d.push_back(9);
    d.push_back(2);
    d.push_back(7);
    d.push_back(1);
    cout << is_in(d, 1) << " " << is_in(d,100) << endl;
}

void quick_mom_plot()
{
    // test_is_in();
    // TString file_name = "../../output/pc144_old_data/1mm_degrader.root";
    // TString file_name = "../../output/out.root";
    TString file_name = "../../output/run_Air_0.2mm.root";

    cout << "will try to open "<<file_name << endl;
    TFile* files = NULL;
    TTree* tree = NULL;
    TH1F* scint1_mom_charged  = NULL;
    TH1F* scint1_mom_muon     = NULL;
    TH1F* scint1_mom_muon_pos = NULL;
    TH1F* scint1_mom_muon_neg = NULL;
    int entries = 0;
    branches_struct branch;

    files = new TFile(file_name);
    if (!files)
    {
        cerr << "file " << file_name<< " not found" << endl;
        return;
    }

    tree = (TTree*) files->Get("t");

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

    entries = tree->GetEntries();
    cout << "File " << file_name<< " has "<< entries << " entries" << endl;
    TString muon_pick = "abs(pdgid)==13&&counter==1";
    cout << "Muons: " << tree->GetEntries(muon_pick) << " at scint 1" << endl;
    TString charged_pick = "(abs(pdgid)==13||abs(pdgid)==11||abs(pdgid)==211||pdgid==2212)&&counter==1";
    cout << "Charged particles: " << tree->GetEntries(charged_pick)<< " at scint 1" << endl;

    TString name = "muon_momentum";
    TString name2  = "charged_momentum";
    scint1_mom_muon     = new TH1F(name, name, 100, 0, 200);
    scint1_mom_muon_pos = new TH1F(name+"pos", name+"pos", 100, 0, 200);
    scint1_mom_muon_neg = new TH1F(name+"neg", name+"neg", 100, 0, 200);
    scint1_mom_charged  = new TH1F(name2, name2, 100, 0, 200);

    cout << "Lock \'n\' loaded " << endl;
    

    unsigned int charged_count = 0;
    unsigned int individuals = 0;
    unsigned int n_muons = 0;
    unsigned int multihit = 0;

    for(int entry = 0; entry < entries; ++entry)
    { // loop over all entries
        tree->GetEntry(entry);
              // branch should now have all the data loaded to it
        vector <int> seen_trackIDs; // skip list
        for(int hit = 0; hit < branch.g_nhit; ++hit)
        {
              // skip conditions: not in scint 1, not charged, seen it already
            ++individuals;
            int counter = branch.counter[hit];
            if (counter != 1) continue;

            int pid = branch.pdgid[hit];
            if ( !(abs(pid) == 11 || abs(pid) == 13 || 
                  abs(pid) == 211 || pid == 2212)  )
                continue;

            int trackID = branch.trkid[hit];
            if (is_in(seen_trackIDs, trackID)) continue;

            double momentum = length (branch.px[hit],branch.py[hit],branch.pz[hit]);
            scint1_mom_charged->Fill(momentum);
            
            if(seen_trackIDs > 0){
                ++multihit; // see how many entries have multiple hits
            }

            if (abs(pid) == 13) {
                scint1_mom_muon->Fill(momentum);
                ++n_muons;
            }
            seen_trackIDs.push_back(trackID);
            ++charged_count;
        }    

    }

    cout << "Total tracks: "<< individuals << " counted "<< 
          charged_count << " skipped: " << (individuals - charged_count) << endl;
    cout << "Events with more than 1 chaged particle in scint1 " << multihit << endl;
    cout << "Number of multihits should be the difference of 'charged 
        particles at scint 1' and total counted tracks" << endl;
    TCanvas* c_charged = new TCanvas("charged", "charged");
    scint1_mom_charged->Draw();

    TCanvas* c_muon = new TCanvas("muon", "muon");
    scint1_mom_muon->Draw();
}            