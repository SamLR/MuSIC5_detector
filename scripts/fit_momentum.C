#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include "TCanvas.h"
#include "Math.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"


double calc_momentum(const double px, const double py, const double pz);
TH1D* get_mom_hist(const TString & name, const double x_min=0.0, const double x_max=200.0);
TF1* get_fit_function(const int pid);
TF1* get_pos_param_func(const TString &name, const TString &func_str);

void fit_momentum()
{
    gStyle->SetOptStat(10);
    gStyle->SetOptFit(111);
    const int n_pids = 7;
    const int pids [n_pids] = {-211, -13, -11, 11, 13, 211, 2212};
    
    TFile* file = new TFile("g4bl_out_36_rotation_30435841_particles.root", "READ");
    TTree* t = (TTree*) file->Get("t");
    const int n_entries = t->GetEntries();
    
    TH1D* momentum_hists[n_pids];
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        TString name("Momentum for PID: ");
        name += pids[pid];
        momentum_hists[pid] = get_mom_hist(name);
    }
    
    double px, py, pz;
    t->SetBranchAddress("Px", &px);
    t->SetBranchAddress("Py", &py);
    t->SetBranchAddress("Pz", &pz);
    
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
            }
        }
    }
    
    TF1* funcs[n_pids];
    
    TCanvas* can1 = new TCanvas("Momentum", "Momentum");
    can1->Divide(4,2);
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        can1->cd(pid + 1);
        momentum_hists[pid]->Draw();
        funcs[pid] = get_fit_function( pids[pid] );
        momentum_hists[pid]->Fit(funcs[pid]);
    }
}
    
double calc_momentum(const double px, const double py, const double pz)
{
    return sqrt(px*px + py*py + pz*pz);
}

TH1D* get_mom_hist(const TString & name, const double x_min, const double x_max)
{
    const int n_bins = static_cast<int>((x_max - x_min));
    TH1D* hist = new TH1D(name, name, n_bins, x_min, x_max);
    hist->GetXaxis()->SetTitle("Momentum (MeV/c)");
    hist->GetYaxis()->SetTitle("Count");
    return hist;
}

TF1* get_fit_function(const int pid)
{
    TString name("Func_PID_");
    name += pid;
    
    TString func_str = (pid==-211) ? TString("gaus(0)+gaus(3)+gaus(6)+gaus(9)") 
                                   : TString("gaus(0)+gaus(3)");
    TF1* func = get_pos_param_func(name, func_str);

    switch(pid)
    {
        case -211: 
        func->SetParameter(1,  60);
        func->SetParameter(4,  90);
        func->SetParameter(7, 120);
        func->SetParameter(10,150);
        break;
        
        case -13:
        func->SetParameter(1, 30);
        func->SetParameter(4, 60);
        break;
        
        case -11:
        func->SetParameter(1, 5);
        func->SetParameter(4, 30);
        break;
        
        case 11:
        func->SetParameter(1, 5);
        func->SetParameter(4, 30);
        break;
        
        case 13:
        func->SetParameter(1, 30);
        func->SetParameter(4, 60);
        break;
        
        case 211:
        func->SetParameter(1,  70);
        func->SetParameter(4, 120);
        break;
        
        case 2212:
        func->SetParameter(1, 70);
        func->SetParameter(4, 150);
        break;
    }
    return func;
}

TF1* get_pos_param_func(const TString &name, const TString &func_str)
{ 
    TF1* func = new TF1(name, func_str,0,200);
    for(int par = 0; par < func->GetNpar(); ++par)
    {
        // Force all paramters to be positive
        func->SetParLimits(par, 0.0, 100000.0);
        // Set a default
        func->SetParameter(par, 10.0);
    }
    return func;
}
