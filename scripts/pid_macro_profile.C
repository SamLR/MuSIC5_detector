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
#include "TF1.h"
#include "TH1F.h"

void pid_macro_profile(){
    // Sisplay the number of entries for each histogram
    gStyle->SetOptStat(10);
    // display the fit results, these should be the (roughly) the same as the g4bl fits
    gStyle->SetOptFit(111);
    const int n_pids = 7;
    const int pids [n_pids] = {-211, -13, -11, 11, 13, 211, 2212};
    const int n_parameters = 10;
    const char* plots [n_parameters] = {"in_x", "in_x_new", "in_y", "in_z", "in_z_new", 
        "in_Px", "in_Px_new", "in_Py", "in_Pz", "in_Pz_new"};
    
    const char file_root [] = "../pid_macros" ;
    const char img_root [] = "../../../images/g4bl_approximation/dbl_gaus_start_at_30p/generated_position_and_momentum_by_axis_PID";
    
    TCanvas* canvases[n_pids];
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        char file_name [32];
        sprintf(file_name,"%s/%i_10000.root",file_root,pids[pid]);
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
        TF1* func [n_parameters];
        
        for(int param = 0; param < n_parameters; ++param)
        {
            printf("\t %s %s\n", cut_str, plots[param]);
            canvases[pid]->cd(param+1);    
            tree->Draw(plots[param], cut_str);

            // Get the generated histogram for fitting
            TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
            
            // Pz tends to be a bit more varied, use 2 gaussians to reflect this
            // Also apparently fit functions _have_ to be TStrings to work
            const bool pz_fit = (strcmp("in_Pz",plots[param]) == 0);
            TString fit_func = (pz_fit) ? TString("gaus(0)+gaus(3)") : TString("gaus(0)");
            // TString fit_func("gaus(0)");
            printf("%s \n ", fit_func.Data());
            char fit_name[16];
            sprintf(fit_name,"f_%i_%s", pid, plots[param]);
            // momenta have a range of [-100, 200], positions ±500
            const double min = (pz_fit) ? -100.0 : -500.0;
            const double max = (pz_fit) ?  200.0 :  500.0;
            
            // Create the function
            func[param] = new TF1(fit_name, fit_func, min, max);
            // Make sure the function's parameters have a good start point & bounds
            for(int p = 0; p < func[param]->GetNpar(); ++p)
            {
                if(p == 0 || p == 3)
                {   // Constrain the scale to 'reasonable' positive values
                    func[param]->SetParLimits(p, 10, 1000000.0);
                    // Set a reasonable start point
                    func[param]->SetParameter(p, 50.0);
                } else if (pz_fit && ( p == 1 || p == 4 ))
                {   // Pz is special again, make sure it's means are positive
                    func[param]->SetParLimits(p, 0, max);
                    // Reasonable starts (make sure the means split)
                    // func[param]->SetParameter(p, p*10.0);
                    func[param]->SetParameter(p, p*30.0);
                } else if (p == 1) 
                {   // Ensure the mean is within the fit bounds
                    func[param]->SetParLimits(p, min, max);
                    // 0 is a reasonable default
                } else if (p == 2 || p == 5)
                {   // Constrain the sigma values by positive bounds
                    func[param]->SetParLimits(p, 0, max);
                    // Set a reasonable start point
                    func[param]->SetParameter(p, 10.0);
                }
            }
            htemp->Fit(func[param]);
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