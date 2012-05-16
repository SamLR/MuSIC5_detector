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

void x_y_average_mom()
{
    TString file_names [] = {"../out/no_degrader.root",
        // "../out/0.2mm_degrader.root",
        "../out/5mm_degrader.root",
        // "../out/1mm_degrader.root"};
        // "../out/10mm_degrader.root"};
        // "../out/test.root"
        "../../out.root"};

    TFile* files [] = {NULL, NULL, NULL};
    TTree* trees [] = {NULL, NULL, NULL};
    TH1F* scint1_mom_charged [] = {NULL, NULL, NULL};
    TH1F* scint1_mom_muon [] = {NULL, NULL, NULL};
    TH1F* scint1_mom_muon_pos [] = {NULL, NULL, NULL};
    TH1F* scint1_mom_muon_neg [] = {NULL, NULL, NULL};
    int entries [] = {0,0,0};
    branches_struct all_branches [3];

    for(int i = 0; i < 3; ++i)
    { // load everything
        files[i] = new TFile(file_names[i]);
        if (!files[i])
        {
            cerr << "file " << file_names[i]<< " not found" << endl;
            return;
        }

        trees[i] = (TTree*) files[i]->Get("t");

        branches_struct branch = all_branches[i];// alias

        trees[i]->SetBranchAddress("iev",&branch.g_iev);
        trees[i]->SetBranchAddress("in_EventID",&branch.in_event);
        trees[i]->SetBranchAddress("in_TrackID",&branch.in_trackID);
        trees[i]->SetBranchAddress("in_PDGid",&branch.in_pdg);
        trees[i]->SetBranchAddress("in_x",&branch.in_x);
        trees[i]->SetBranchAddress("in_y",&branch.in_y);
        trees[i]->SetBranchAddress("in_z",&branch.in_z); 
        trees[i]->SetBranchAddress("in_Px",&branch.in_px); 
        trees[i]->SetBranchAddress("in_Py",&branch.in_py); 
        trees[i]->SetBranchAddress("in_Pz",&branch.in_pz); 
        trees[i]->SetBranchAddress("in_tof",&branch.in_tof); 

        trees[i]->SetBranchAddress("nhit",&branch.g_nhit);
        trees[i]->SetBranchAddress("procid",branch.procid);
        trees[i]->SetBranchAddress("counter",branch.counter);
        trees[i]->SetBranchAddress("trkid",branch.trkid);
        trees[i]->SetBranchAddress("parentid",branch.parentid);
        trees[i]->SetBranchAddress("pdgid",branch.pdgid);
        trees[i]->SetBranchAddress("x",branch.x);
        trees[i]->SetBranchAddress("y",branch.y);
        trees[i]->SetBranchAddress("z",branch.z);
        trees[i]->SetBranchAddress("px",branch.px);
        trees[i]->SetBranchAddress("py",branch.py);
        trees[i]->SetBranchAddress("pz",branch.pz);
        trees[i]->SetBranchAddress("edep",branch.edep);
        trees[i]->SetBranchAddress("tof",branch.tof);

        entries[i] = trees[i]->GetEntries();
        cout << "Tree in file " << file_names[i]<< " has "<< entries[i] << endl;

        TString name = "muon_momentum";
        name += i;
        TString name2  = "charged_momentum";
        name2 += i;
        scint1_mom_muon[i] = new TH1F(name, name, 100, 0, 200);
        scint1_mom_muon_pos[i] = new TH1F(name+"pos", name+"pos", 100, 0, 200);
        scint1_mom_muon_neg[i] = new TH1F(name+"neg", name+"neg", 100, 0, 200);
        scint1_mom_charged[i] = new TH1F(name2, name2, 100, 0, 200);
    }
    cout << "Lock \'n\' load" << endl;

    for(int i = 0; i < 3; ++i)
    {
        TTree* current_tree = trees[i];
        branches_struct branch = all_branches[i];
        for(int entry = 0; entry < entries[i]; ++entry)
        { // loop over all entries
            current_tree->GetEntry(entry);
            // branch should now have all the data loaded to it
            for(int hit = 0; hit < branch.g_nhit; ++hit)
            { // loop over all the hits
                if (branch.counter == 1);
                { // particle in scint 1
                    int pid = branch.pdgid[hit];
                    if (abs(pid) == 11 || abs(pid) == 13 || abs(pid) == 211 || pid == 2212)
                    { // charged particle
                        double momentum = length (branch.px[hit],branch.py[hit],branch.pz[hit]);
                        scint1_mom_charged[i]->Fill(momentum);
                        if (pid == 13) // negative muon
                        {
                            scint1_mom_muon_neg[i]->Fill(momentum);
                            scint1_mom_muon[i]->Fill(momentum);
                        } else if (pid == -13) // positive muon
                        {   
                            scint1_mom_muon_pos[i]->Fill(momentum);
                            scint1_mom_muon[i]->Fill(momentum);
                        }

                    }
                }
            }
        }
    }
    
    TCanvas* c_charged = new TCanvas("charged", "charged");
    TCanvas* c_muon = new TCanvas("muon", "muon");
    int pref_colours [] = {1, 2, 4}; // better colours for plots
    for(int i = 0; i < 3; ++i)
    {
        scint1_mom_charged[i]->SetLineColor(pref_colours[i]);
        scint1_mom_muon[i]->SetLineColor(pref_colours[i]);
        c_charged->cd();
        if (i == 0)
        {
            scint1_mom_charged[i]->Draw();
            c_muon->cd();
            scint1_mom_muon[i]->Draw();
        } else
        {
            scint1_mom_charged[i]->Draw("SAME");
            c_muon->cd();
            scint1_mom_muon[i]->Draw("SAME");
        }
    }
    

}

