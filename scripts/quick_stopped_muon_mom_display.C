#include "TFile.h"
#include "TH1F.h"
#include "useful_for_root/drawing.C"

void quick_stopped_muon_mom_display() {
    const int n_hists = 4;
    TFile* hist_file = new TFile("stopped_muon_momenta_saved_hists.root", "READ");
    TString hist_prefix = "Muon_momentum_with_";
    TString hist_roots [n_hists] = { "5mm_Air",
        "0.5mm_Aluminium",
        "1mm_Aluminium",
        "5mm_Aluminium"};
        
    TH1F* hist_array [n_hists];
    printf("Get the histograms!\n");
    for(int h_id = 0; h_id < n_hists; ++h_id) {
        TString hist_name = hist_prefix + hist_roots[h_id];
        hist_array[h_id] = (TH1F*) hist_file->Get(hist_name);
        hist_array[h_id]->GetYaxis()->SetTitleOffset(1.3);
        hist_array[h_id]->GetXaxis()->SetTitleOffset(1.2);
    }
    printf("Histograms gotten!\n");
    

    TString title("Stopped muon momentum");
    
    TString save_location1("images/stopped_muon_momentum.eps");
    TString save_location2("images/stopped_muon_momentum.svg");
    
    // same as above but with spaces! shiny! 
    TString file_roots_tstrings[n_hists] = {
        TString("5mm Air"),
        TString("0.5mm Aluminium"),
        TString("1mm Aluminium"),
        TString("5mm Aluminium")
    };
    
    TCanvas* can = draw_pretty_hists(n_hists, hist_array, title, file_roots_tstrings, save_location1, 1002201);
    can->SaveAs(save_location2);
}