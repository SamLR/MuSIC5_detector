#include "TFile.h" 
#include "TTree.h" 
#include "TCanvas.h"
#include "TString.h"
#include "TH1F.h"
#include "TVirtualPad.h"
#include "TROOT.h"
#include "TStyle.h"

void name_hist_and_range(const int pid, TVirtualPad* pad);
void plot_total_momentum_vs_pid_for_gen_and_g4bl() {
    // open g4bl file
    // make a canvas
    // split the canvas into 2x4
    // for each pad plot a pid (-211, -13, -11, 11, 13, 211, 2212)
    // 
    // make a canvas
    // split into 2x4
    // for each pid
    //   open pid file
    //   get pad and plot 
    
    gStyle->SetOptStat(2210);
    
    const int n_pids = 7;
    const int pids [n_pids] = {-211, -13, -11, 11, 13, 211, 2212};
    
    TFile* g4bl_file = new TFile("../../g4blout/from_hep_1Bn/g4bl_out_36_rotation_30435841_particles.root", "READ");
    TTree* tree = (TTree*) g4bl_file->Get("t");
    TCanvas* g4bl_can = new TCanvas("g4bl", "g4bl", 1436, 856);
    g4bl_can->Divide(4,2);
    
    TString g4bl_command("sqrt(Px_new*Px_new + Py*Py + Pz_new*Pz_new)");
    
    for(int pid = 0; pid < n_pids; ++pid) {
        TVirtualPad* pad = g4bl_can->cd(pid+1);
        TString cut("PDGid==");
        cut += pids[pid];
        tree->Draw(g4bl_command, cut);
        name_hist_and_range(pids[pid], pad);
    }
    g4bl_can->SaveAs("../../../images/g4bl_approximation/g4bl_total_momentums_by_pid.svg");
    g4bl_can->SaveAs("../../../images/g4bl_approximation/g4bl_total_momentums_by_pid.png");
    g4bl_file->Close();
    
    TCanvas* gen_can = new TCanvas("gen", "gen", 1436, 856);
    gen_can->Divide(4,2);
    
    TString gen_command("sqrt(in_Px_new*in_Px_new + in_Py*in_Py + in_Pz_new*in_Pz_new)");
    
    for(int pid = 0; pid < n_pids; ++pid) {
        TString filename("../pid_macros/root/pid_");
        filename += pids[pid];
        filename += "_events_100000.root";
        
        TFile* pid_file = new TFile(filename, "READ");
        tree = (TTree*) pid_file->Get("truth");
        
        TVirtualPad* pad = gen_can->cd(pid+1);

        tree->Draw(gen_command);
        
        pid_file->Close();
        name_hist_and_range(pids[pid], pad);
    }
    
    gen_can->SaveAs("../../../images/g4bl_approximation/generated_total_momentums_by_pid.svg");
    gen_can->SaveAs("../../../images/g4bl_approximation/generated_total_momentums_by_pid.png");
}
 
 
 
void name_hist_and_range(const int pid, TVirtualPad* pad) {
    TString title("PID = ");
    title += pid;
    TH1F* htemp = (TH1F*) pad->GetPrimitive("htemp");
    htemp->SetTitle(title);
    htemp->GetXaxis()->SetRange(0.0, 200.0);
    htemp->GetXaxis()->SetTitle("Momentum (MeV/c)");
    htemp->GetYaxis()->SetTitle("Count");
}