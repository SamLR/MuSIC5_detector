#include "useful_for_root/drawing.C"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1F.h"

void beampipe_muon_momentum()
{
    TString filename = "../../output/final_900M/final_st_Copper_0.5mm_deg_Air_5mm.root";
    TFile* in_file = new TFile(filename, "READ");
    TTree* tree = (TTree*) in_file->Get("t");
    
    // Use the Tree Draw function to select and draw what we want
    
    // String that tells root to plot the total momentum
    TString plot_mom = "sqrt(in_Px*in_Px + in_Py*in_Py + in_Pz*in_Pz)";
    
    // What cuts to apply to the histograms
    TString cut_mu_plus  = "in_PDGid==-13";
    TString cut_mu_minus = "in_PDGid==13";
    TString cut_mu_all   = "abs(in_PDGid)==13";
    
    TString tmp = plot_mom +">>plus";
    
    tree->Draw(tmp, cut_mu_plus);
    TH1F* plus = (TH1F*) gPad->GetPrimitive("plus");
    
    tmp = plot_mom+">>minus";
    tree->Draw(tmp, cut_mu_minus);
    TH1F* minus = (TH1F*) gPad->GetPrimitive("minus");
    
    tmp=plot_mom+">>all";
    tree->Draw(tmp, cut_mu_all);
    TH1F* all = (TH1F*) gPad->GetPrimitive("all");
    all->GetXaxis()->SetTitle("Momentum (MeV/c)");
    all->GetYaxis()->SetTitle("Count");
    
    
    TH1F* array [] = {all, plus, minus};
    TString title = "Muon momentum at beampipe";
    TString leg_entries [] = {"All", "#mu+", "#mu-"};
    TString save_location = "images/muon_momentum_at_beam_pipe_exit.svg";
    
    
    draw_pretty_hists(3, array, title, leg_entries, save_location, 1002201);
}