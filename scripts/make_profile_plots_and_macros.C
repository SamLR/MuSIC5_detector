// 
// This file (originally) created 1D plots of the main
// parameters for each of the charged particle types
// in the MuSIC input G4BL file. It now additionally
// fits either a gaussian or 2 guassians to this data
// and produces macros for the music g4 simulation
// based on these fits.
// 
// The parameters are: position x and y  (before 
// rotation) and momentum (again: x, y and z 
// before rotation). Position in z is not fitted
// as this is fixed (at 3901.1799 mm).
//
// Because the all the particles are heavily boosted
// in z this is where most features are (in momentum)
// to reflect this rather than a single gaussian
// two are used.
//

#include <stdio.h>

#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TROOT.h"
#include "TF1.h"
#include "TH1F.h"
#include "TStyle.h"

void write_macro_for_pid(const int pid, TF1** func_array);
void find_particle(const int pid, char* buffer);

void make_profile_plots_and_macros() {
    // Sisplay the number of entries for each histogram
    gStyle->SetOptStat(10);
    // and display the fit values, errors & chi^211
    gStyle->SetOptFit(111);
    const int n_pids = 7;
    const int pids [n_pids] = {-211, -13, -11, 11, 13, 211, 2212};
    const int n_parameters = 10;
    const char* plots [n_parameters] = {"x",  "x_new",  "y",  "z",  "z_new",
                                        "Px", "Px_new", "Py", "Pz", "Pz_new"};
    const char img_root [] = "../../../images/g4bl_approximation/dbl_gaus_start_at_30p/position_and_momentum_by_axis_PID";
    
    TFile* file = new TFile("../../g4blout/from_hep_1Bn/g4bl_out_36_rotation_30435841_particles.root", "READ");
    TTree* tree = (TTree*) file->Get("t");
    
    TCanvas* canvases[n_pids];
    
    for(int pid = 0; pid < n_pids; ++pid)
    {
        char can_name[16];
        sprintf(can_name, "canvas %i", pids[pid]);
        
        printf("%s %i\n", can_name, pid);
        
        // magic numbers to full screen the canvas
        canvases[pid] = new TCanvas(can_name, can_name,1436,856);
        canvases[pid]->Divide(5, 2);
        
        char cut_str[16];
        sprintf(cut_str, "PDGid==%i", pids[pid]);
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
            const bool pz_fit = (strcmp("Pz",plots[param]) == 0);
            // const bool pz_fit = false; // if using a single gaus
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
        
        // create the macro 
        // write_macro_for_pid(pids[pid],func);
    }
}

void write_macro_for_pid(const int pid, TF1** func_array) {
    
    char file_name[32];
    sprintf(file_name, "../pid_macros/pid_%i.mac", pid);
    FILE* macro_file = fopen(file_name, "w");
    
    const char path_root [] = "/MuSIC_Detector/gun/set";
    const char* commands [] = {"Xmean",   "Xsigma",        // function 0
    "Ymean",   "Ysigma",        // function 1
    // The z values shouldn't get used
    "zDUMMY",  "zDUMMY",
    "PxMean",  "PxSigma",       // function 2
    "PyMean",  "PySigma",       // function 3
    "PzMean",  "PzSigma", 
    "PzMean2", "PzSigma2", "PzRatio"}; // function 4
    
    
    // Need to translate PID in to the correct gun command  
    char particle_name[8];
    find_particle(pid, particle_name);
    // Write a header
    fprintf(macro_file, "# This is a generated macro to create a distribution for %s\n", particle_name);
    // Disable use of the G4BL input file
    fprintf(macro_file, "/MuSIC_Detector/gun/g4blEnable false\n");
    // Now set the particle type
    fprintf(macro_file, "/gun/particle %s\n\n", particle_name);

    // Now set all the required parameters
    for(int f = 0; f < 6; ++f)
    {
        // Loop over all 
        TF1* func = func_array[f];
        if (!func) { 
            continue; // Skip z & Pz
        } else if (func->GetNpar()>3) {
            fprintf(macro_file, "# Scale factor1 %.2e, factor2 %.2e function integral %.2e\n", 
                func->GetParameter(0), 
                func->GetParameter(3), 
                func->Integral(-100.0,200.0)); 
                                
            fprintf(macro_file, "%s%s %.2e\n", 
                path_root, commands[2*f + 0], func->GetParameter(1));
            fprintf(macro_file, "%s%s %.2e\n", 
                path_root, commands[2*f + 1], func->GetParameter(2));
            fprintf(macro_file, "%s%s %.2e\n", 
                path_root, commands[2*f + 2], func->GetParameter(4));
            fprintf(macro_file, "%s%s %.2e\n", 
                path_root, commands[2*f + 3], func->GetParameter(5));
                                
            const double dbl_int = func->Integral(-100.0, 200.0);
            TF1* sub_func = new TF1("sub_func", "gaus(0)", -100.0, 200);
            sub_func->SetParameters(func->GetParameters());
            const double sub_int = sub_func->Integral(-100.0, 200.0);

            fprintf(macro_file, "%s%s %.2e\n\n", 
                path_root, commands[2*f + 4], sub_int/dbl_int);
            
        } else {
        
            fprintf(macro_file, "# Scale factor %.2e, function integral %.2e\n", 
                func->GetParameter(0), func->Integral(-100.0,200.0)); 
            fprintf(macro_file, "%s%s %.2e\n", 
                path_root, commands[2*f + 0], func->GetParameter(1));
            fprintf(macro_file, "%s%s %.2e\n\n", 
                path_root, commands[2*f + 1], func->GetParameter(2));
        } 
    }
}

void find_particle(const int pid, char* buffer) {
    switch (pid) {
        case -211: 
        sprintf(buffer, "pi-");
        return;
        case  -13: 
        sprintf(buffer, "mu+");
        return;
        case  -11: 
        sprintf(buffer, "e+");
        return;
        case   11: 
        sprintf(buffer, "e-");
        return;
        case   13: 
        sprintf(buffer, "mu-");
        return;
        case  211: 
        sprintf(buffer, "pi+");
        return;
        case 2212: 
        sprintf(buffer, "proton");
        return;
        default  : 
        sprintf(buffer, "UNKNOWN");
        return;
    }
}
