// 
// Script to check that the generated profiles
// for the various particles in music
// look like the profiles from g4bl
//

#include "TTree.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"

void pid_macro_profile(){
    // Sisplay the number of entries for each histogram
    gStyle->SetOptStat(2210);
    const int n_pids = 7;
    const int pids [n_pids] = {-211, -13, -11, 11, 13, 211, 2212};
    const int n_parameters = 10;
    const char* plots [n_parameters] = {"in_x", "in_x_new", "in_y", "in_z", "in_z_new", 
        "in_Px", "in_Px_new", "in_Py", "in_Pz", "in_Pz_new"};
    
    const char file_root [] = "../pid_macros" ;
    const char img_root [] = "../../../images/g4bl_approximation/single_gaus_start_at_30p/generated_position_and_momentum_by_axis_PID";
    
    TCanvas* canvases[n_pids];
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        char file_name [32];
        sprintf(file_name,"%s/%i_10k.root",file_root,pids[pid]);
        TFile* file = new TFile(file_name, "READ");
        TTree* tree = (TTree*) file->Get("truth");
        
        printf("File: %s loaded, %lld entries found.\n", file_name, tree->GetEntries());
        
        char can_name[16];
        sprintf(can_name, "canvas %i", pids[pid]);
        
        printf("%s %i\n", can_name, pid);
        
        // magic numbers to full screen the canvas
        canvases[pid] = new TCanvas(can_name, can_name,1436,856);
        canvases[pid]->Divide(5, 2);
        
        char cut_str[20];
        sprintf(cut_str, "in_PDGid==%i", pids[pid]);
        
        for(int param = 0; param < n_parameters; ++param)
        {
            printf("\t %s %s\n", cut_str, plots[param]);
            canvases[pid]->cd(param+1);    
            tree->Draw(plots[param], cut_str);
            // the z position is fixed, don't bother fitting it
        }
        

        
        char img_name [85];
        // Save as a PNG
        sprintf(img_name,"%s_%i.png",img_root, pids[pid]);
        canvases[pid]->SaveAs(img_name);
        // Save the image as a SVG
        sprintf(img_name,"%s_%i.svg",img_root, pids[pid]);
        canvases[pid]->SaveAs(img_name);
        
        file->Close();
    }
}