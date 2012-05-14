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

double length(double x, double  y, double  z)
{
    return sqrt(x*x + y*y + z*z);
}

void quick_mom_plot()
{
    // TString file_name = "../../output/pc144_old_data/1mm_degrader.root";
    TString file_name = "../../output/out_original.root";
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
    cout << "Tree in file " << file_name<< " has "<< entries << endl;

    TString name = "muon_momentum";
    TString name2  = "charged_momentum";
    scint1_mom_muon     = new TH1F(name, name, 100, 0, 200);
    scint1_mom_muon_pos = new TH1F(name+"pos", name+"pos", 100, 0, 200);
    scint1_mom_muon_neg = new TH1F(name+"neg", name+"neg", 100, 0, 200);
    scint1_mom_charged  = new TH1F(name2, name2, 100, 0, 200);

    cout << "Lock \'n\' loaded " << entries << " entries."<< endl;

    for(int entry = 0; entry < entries; ++entry)
    { // loop over all entries
        tree->GetEntry(entry);
            // branch should now have all the data loaded to it
        // cout << branch.g_nhit << endl;
        for(int hit = 0; hit < branch.g_nhit; ++hit)
        { // loop over all the hits        
            if (*branch.counter == 1)
            { // particle in scint 1    
                int pid = *(branch.pdgid);
                if (abs(pid) == 11 || abs(pid) == 13 || abs(pid) == 211 || pid == 2212)
                { // charged particle
                    double momentum = length (branch.px[hit],branch.py[hit],branch.pz[hit]);
                    scint1_mom_charged->Fill(momentum);
                    if (pid == 13) // negative muon
                    {
                        scint1_mom_muon_neg->Fill(momentum);
                        scint1_mom_muon->Fill(momentum);
                    } else if (pid == -13) // positive muon
                    {   
                        scint1_mom_muon_pos->Fill(momentum);
                        scint1_mom_muon->Fill(momentum);
                    }
                }
            }
        }

    }
    TCanvas* c_charged = new TCanvas("charged", "charged");
    scint1_mom_charged->Draw();
    
    TCanvas* c_muon = new TCanvas("muon", "muon");
    scint1_mom_muon->Draw();
}            