#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TCanvas.h"
#include "Math.h"

double calc_momentum(const double px, const double py, const double pz);
TH1D* get_mom_hist(const TString & name, const double x_min=0.0, const double x_max=200.0);
TH2D* get_pos_hists(const TString & name, const double min=-500.0, const double max=500.0);

void gen_profile_plots()
{
    const int n_pids = 7;
    const int pids [n_pids] = {-211, -13, -11, 11, 13, 211, 2212};
    
    TFile* file = new TFile("g4bl_out_36_rotation_30435841_particles.root", "READ");
    TTree* t = (TTree*) file->Get("t");
    const int n_entries = t->GetEntries();
    
    TH1D* momentum_hists[n_pids];
    // TH2D* position_hists[n_pids];
    // TH2D* heat_map_hists[n_pids];
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        TString name("Momentum for PID: ");
        name += pids[pid];
        momentum_hists[pid] = get_mom_hist(name);
        // 
        // TString name2("Positions for PID: ");
        // name2 += pids[pid];
        // position_hists[pid] = get_pos_hists(name2);
        // position_hists[pid]->GetZaxis()->SetTitle("Count");
        // 
        // TString name3("Heat map of momentum for PID: ");
        // name3 += pids[pid];
        // heat_map_hists[pid] = get_pos_hists(name3);
    }
    
    double px, py, pz, x, y, z;
    t->SetBranchAddress("Px", &px);
    t->SetBranchAddress("Py", &py);
    t->SetBranchAddress("Pz", &pz);
    t->SetBranchAddress("x",  &x);
    t->SetBranchAddress("y",  &y);
    t->SetBranchAddress("z",  &z);
    
    int pdgid;
    t->SetBranchAddress("PDGid", &pdgid);
        
    for(int entry = 0; entry < n_entries; ++entry)
    {
        t->GetEntry(entry);
        for(int pid = 0; pid < n_pids; ++pid)
        {
            if(pdgid == pids[pid])
            {
                momentum_hists[pid]->Fill(calc_momentum(px,py,pz));
                // position_hists[pid]->Fill(x, y);
                // heat_map_hists[pid]->Fill(x, y, calc_momentum(px,py,pz));
            }
        }
    }
    
    TCanvas* can1 = new TCanvas("Momentum", "Momentum");
    // TCanvas* can2 = new TCanvas("Position", "Position");
    // TCanvas* can3 = new TCanvas("Heat Map", "Heat Map");
    can1->Divide(4,2);
    // can2->Divide(4,2);
    // can3->Divide(4,2);
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        can1->cd(pid + 1);
        momentum_hists[pid]->Draw();
        // can2->cd(pid + 1);
        // position_hists[pid]->Draw("SURF2");
        // 
        // can3->cd(pid + 1);
        // heat_map_hists[pid]->Divide(position_hists[pid]);
        // heat_map_hists[pid]->Draw("COLZ");
    }
}
    
double calc_momentum(const double px, const double py, const double pz)
{
    return sqrt(px*px + py*py + pz*pz);
}


TH2D* get_pos_hists(const TString & name, const double min, const double max)
{
    // const int n_bins = static_cast<int>((max - min)/5.0);
    const int n_bins = 50;
    TH2D* hist = new TH2D(name, name, n_bins, min, max, n_bins, min, max);
    hist->GetXaxis()->SetTitle("X (mm)");
    hist->GetXaxis()->SetTitleOffset(1.5);
    hist->GetYaxis()->SetTitle("Y (mm)");
    hist->GetYaxis()->SetTitleOffset(1.9);
    hist->GetZaxis()->SetTitle("Momentum (MeV/c)");
    hist->GetZaxis()->SetTitleOffset(1.4);
    hist->SetContour(50);
    return hist;
}

TH1D* get_mom_hist(const TString & name, const double x_min, const double x_max)
{
    const int n_bins = static_cast<int>((x_max - x_min)/2.0);
    TH1D* hist = new TH1D(name, name, n_bins, x_min, x_max);
    hist->GetXaxis()->SetTitle("Momentum (MeV/c)");
    hist->GetYaxis()->SetTitle("Count");
    return hist;
}
