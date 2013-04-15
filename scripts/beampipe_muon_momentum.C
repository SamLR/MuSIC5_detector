#include "useful_for_root/drawing.C"
#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1F.h"

void beampipe_muon_momentum()
{
    const double mu_per_uA = 1.0e-6/(9.0e8*1.602e-19); // muons per amp for our initial 900M
    // Use the g4bl file
    TString filename = "../../g4blout/from_hep_1Bn/out_36_rotate.root";
    TFile* in_file = new TFile(filename, "READ");
    TTree* tree = (TTree*) in_file->Get("t");
    
    // Use the Tree Draw function to select and draw what we want
    
    // String that tells root to plot the total momentum
    TString plot_mom = "sqrt(Px*Px + Py*Py + Pz*Pz)";
    
    // What cuts to apply to the histograms
    TString cut_mu_plus  = "PDGid==-13";
    TString cut_mu_minus = "PDGid==13";
    TString cut_mu_all   = "abs(PDGid)==13";
    
    TString tmp = plot_mom +">>plus";
    
    tree->Draw(tmp, cut_mu_plus);
    TH1F* plus = (TH1F*) gPad->GetPrimitive("plus");
    plus->Scale(mu_per_uA);
    
    tmp = plot_mom+">>minus";
    tree->Draw(tmp, cut_mu_minus);
    TH1F* minus = (TH1F*) gPad->GetPrimitive("minus");
    minus->Scale(mu_per_uA);
    
    tmp=plot_mom+">>all";
    tree->Draw(tmp, cut_mu_all);
    TH1F* all = (TH1F*) gPad->GetPrimitive("all");
    all->GetXaxis()->SetTitle("Momentum (MeV/c)");
    all->GetYaxis()->SetTitle("Muon Rate (muons/#muA)");
    all->Scale(mu_per_uA);
    
    TH1F* array [] = {all, plus, minus};
    TString title = "Muon momentum at beampipe";
    TString leg_entries [] = {"All", "Muon(+)", "Muons(-)"};
    TString save_location = "images/muon_momentum_at_beam_pipe_exit.";
    
    
    TCanvas* can = draw_pretty_hists(3, array, title, leg_entries, save_location+"svg", 1002201);
    can->SaveAs(save_location+"png");
    
}