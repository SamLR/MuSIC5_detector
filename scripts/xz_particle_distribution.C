/* 
 * A quick macro to give a top-down view of the beam (i.e. the x-z plane). 
 * This is to ensure that the 36 rotation required to covert from beam-pipe
 * aligned co-ordinates (i.e. those used by G4BL) to pion production target
 * aligned co-ordinates (i.e. those used by the magnetic field maps).
 */

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"

TH2F* xz_particle_distribution_inner(
     const TString filename, const TString treename, TFile* in_file,
          const TString x_name="x_new", const int bins_x=200, 
          const float x_min=500,  const float x_max=4000,
          const TString z_name="z_new", const int bins_z=200, 
          const float z_min=1500, const float z_max=4500);
          // const TString x_name="x", const int bins_x=200, 
          // const float x_min=-2500,  const float x_max=2500,
          // const TString z_name="z", const int bins_z=200, 
          // const float z_min=3800, const float z_max=4000);


void xz_particle_distribution(){
    // standalone wrapper, allows workings to be re-used
    const TString in_filename1 = "../../g4blout/from_hep/out_36_rotate.root";
    const TString in_filename2 = "../../g4blout/monitor6_By+0.04T_cor.root";
    const TString in_treename = "t";
    TFile* in_file1 = 0;
    TFile* in_file2 = 0;
    TH2F* hist1 = xz_particle_distribution_inner(in_filename1, in_treename, in_file1);
    TH2F* hist2 = xz_particle_distribution_inner(in_filename2, in_treename, in_file2);
    TCanvas* c1 = new TCanvas("side_by_side","side_by_side",1436,856);
    c1->Divide(2,1);
    c1->cd(1);
    hist1->Draw("ColZ");
    c1->cd(2);
    hist2->Draw("ColZ");
}

TH2F* xz_particle_distribution_inner(
        const TString filename, const TString treename, TFile* in_file,
        const TString x_name, const int bins_x, const float x_min, const float x_max,
        const TString z_name, const int bins_z, const float z_min, const float z_max ){
    // this assumes that the xy & positional branches are called 'x_new' and 'z_new'
    in_file = new TFile(filename, "READ");
    TTree* in_tree = (TTree*) in_file->Get(treename);
    double x, z;
    in_tree->SetBranchAddress(x_name, &x);
    in_tree->SetBranchAddress(z_name, &z);
    const int n_entries = in_tree->GetEntries();
    TString h_name = filename + "_xz_positions";
    TH2F* hist = new TH2F(h_name, h_name, bins_x, x_min, x_max, bins_z, z_min, z_max);
    
    for(int entry = 0; entry < n_entries; ++entry) {
        in_tree->GetEntry(entry);
        hist->Fill(x,z);
    }
    return hist;
}