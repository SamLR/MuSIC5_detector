// A macro which display the different numbers of particles at a counter
// 

#include <iostream>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "useful_for_root/drawing.C"

struct in_branch 
{
	int n_hits;
	int pdgid[500];
    int counter[500];
    bool first_step[500];
};

enum pdg_id 
{
    PDG_PI_MINUS = -211,
    PDG_MU_PLUS  = -13,
    PDG_E_PLUS   = -11,
    PDG_E_MINUS  =  11,
    PDG_MU_MINUS =  13,
    PDG_PI_PLUS  =  211,
    PDG_PROTON   =  2212,
    PDG_ERROR    =  0
};

enum particle_bin 
{
    BIN_PI_MINUS = 1,
    BIN_MU_PLUS  = 2,
    BIN_E_PLUS   = 3,
    BIN_E_MINUS  = 4,
    BIN_MU_MINUS = 5,
    BIN_PI_PLUS  = 6,
    BIN_PROTON   = 7,
    BIN_ERROR    = -1,
};

TH1F* get_in_pid_hist(const TString filename, const TString histname);
TH1F* get_pid_count_hist(const TString filename, const TString histname, const int counter);
TH1F* make_hist (TString name);
void set_addresses (TTree* tree, in_branch& branch);
TString get_particle_name (const int id);
particle_bin get_bin_number (const pdg_id id);
pdg_id get_pdg_id (const particle_bin id);

void quick_pid_count()
{
    // Create a pair of histograms comparing the particle types at the end 
    // of the beampipe (i.e. the G4BL input) and just prior to the degrader
    // (i.e. the "actual" simulation input).
    const TString file_prefix = "../../output/final_900M/final_st_Copper_0.5mm_deg_";
    const TString file_suffix = ".root";
    
    const TString beampipe_dat = "Air_5mm";
    TString air_filename = file_prefix + beampipe_dat + file_suffix;
    TH1F* beampipe_c5 = get_pid_count_hist(air_filename, "pre-deg_pids", 5);
    TH1F* g4bl_initial = get_in_pid_hist(air_filename, "g4bl_pids");
    
    TH1F* beampipe_hists [] = {beampipe_c5, g4bl_initial};
        
    TString beampipe_legs [] = {"Particle counts pre-degrader", "Particle counts from G4BL"};
    TString beampipe_save_loc = "images/pid_counts_at_beampipe.svg";
    TString beampipe_title = "PID counts at beampipe";
    draw_pretty_hists(2, beampipe_hists, beampipe_title, 
                      beampipe_legs, beampipe_save_loc, 1002201);
                
    // Create a set of histograms that compare the number of particles at
    // the stopping target depending on degrader.
    const int n_files = 4;
    const TString file_roots [n_files] = {"Air_5mm",
                               "Aluminium_0.5mm",
                               "Aluminium_1mm",
                               "Aluminium_5mm"};
                
    TH1F* st_hists [n_files];
    for(int file_number = 0; file_number < n_files; ++file_number)
    {
        TString filename = file_prefix + file_roots[file_number] + file_suffix;
        cout << "Starting file: "<< filename << endl;
        st_hists[file_number] = get_pid_count_hist(filename, file_roots[file_number], 2);
    }
                
    TString title = "Particle counts at the target for varying degraders";
    TString save_location = "images/pid_counts_at_target.svg";
    draw_pretty_hists(n_files, st_hists, title, file_roots, save_location, 1002201);
                
                
    TString save_location2 = "images/pid_counts_at_target.svg";
    draw_pretty_hists(n_files, st_hists, title, file_roots, save_location2, 1002201);
                
    // Print beamline information
    printf ("%9s | %8s | %8s\n","Particle","5","in");
    for(int bin = 1; bin <= 7; ++bin)
    {
        printf("%9s | %8.0f | %8.0f\n", get_particle_name(bin).Data(), 
                beampipe_hists[0]->GetBinContent(bin), 
                beampipe_hists[1]->GetBinContent(bin) );
    }
}

TH1F* get_in_pid_hist(const TString filename, const TString histname)
{ // Get numbers for each particle from G4BL (i.e. using in_PDGid) 
    TH1F* hist = make_hist(histname);
    TFile* file = new TFile(filename, "READ");
    TTree* tree = (TTree*) file->Get("t");
    
    for(int bin = 1; bin <= 7; ++bin)
    {
        TString search = TString("in_PDGid==");
        search += ((int)get_pdg_id((particle_bin)bin));
        hist->SetBinContent(bin, tree->GetEntries(search));
    }
    file->Close();
    return hist;
}

TH1F* get_pid_count_hist(const TString filename, const TString histname, const int counter) 
{
    // counter 5, look at the numbers at the beam pipe
    // const int counter = 5; 
    // TString filename = "../../output/final_900M/final_st_Copper_0.5mm_deg_Air_5mm.root";
    TH1F* hist = make_hist(histname);
    TFile* file = new TFile(filename, "READ");
    TTree* tree = (TTree*) file->Get("t");
    
    in_branch branch; 
    set_addresses(tree, branch);
    
    const int n_entries = tree->GetEntries();
    // const int n_entries = 100;
    for (int entry = 0; entry < n_entries; ++entry)
    { // Loop over all the entries
        tree->GetEntry(entry);
        for (int hit = 0; hit < branch.n_hits; ++hit)
        { // Loop over the hits and fill the histograms
            
            // Skip anything that's not at the correct counter or the first step
            if ( branch.counter[hit] != counter) continue;
            if (!branch.first_step)              continue; // don't double count
            // Translate the PDG id into a bin number (needs cast)
            const int bin_num = get_bin_number((pdg_id) branch.pdgid[hit]);
            // get_bin_number returns -1 if not a charged particle
            if (bin_num == BIN_ERROR) continue;
            hist->Fill(bin_num);
        }
    }
    
    file->Close();
    return hist;
}

TH1F* make_hist (TString name) 
{ // Make a PID histogram 

    // Use a .5 offset so that bin number == bin id 
    // i.e. bin number in 'Fill(bin)' and 'SetBinLabel(bin, label)' are
    //      the same
    TH1F* hist = new TH1F(name,name,7, 0.5, 7.5);
    hist->GetXaxis()->SetTitle("Particle");
    hist->GetYaxis()->SetTitle("Count");
    for(int bin = 1; bin <= 7; ++bin)
    { // Set the bin labels
        hist->GetXaxis()->SetBinLabel(bin, get_particle_name(bin));
    }
    return hist;
}

void set_addresses (TTree* tree, in_branch& branch) 
{ // Set the addresses for the branches we're interested in 
    tree->SetBranchAddress("nhit",       &branch.n_hits);
    tree->SetBranchAddress("pdgid",      &branch.pdgid);
    tree->SetBranchAddress("counter",    &branch.counter);
    tree->SetBranchAddress("first_step", &branch.first_step);
}

TString get_particle_name (const int id)
{ // Get a string of the particle name 
    
    // If the id isn't a particle_bin id then convert it to one
    // (assuming that it *is* a pdg id)
    const int id_i = (1 <= id && id <= 7) ? id : get_bin_number((pdg_id) id);
    
    switch (id_i) 
    {
        case BIN_PI_MINUS : return TString ("#pi-");
        case BIN_MU_PLUS  : return TString ("#mu+");
        case BIN_E_PLUS   : return TString ("e+");
        case BIN_E_MINUS  : return TString ("e-");
        case BIN_MU_MINUS : return TString ("#mu-");
        case BIN_PI_PLUS  : return TString ("#pi+");
        case BIN_PROTON   : return TString ("p");
        default           : return TString ("ERROR");
    }
}

particle_bin get_bin_number (const pdg_id id) 
{ // Convert PDG_id to a bin_id 
    switch (id) 
    {
        case PDG_PI_MINUS : return BIN_PI_MINUS;
        case PDG_MU_PLUS  : return BIN_MU_PLUS;
        case PDG_E_PLUS   : return BIN_E_PLUS;
        case PDG_E_MINUS  : return BIN_E_MINUS;
        case PDG_MU_MINUS : return BIN_MU_MINUS;
        case PDG_PI_PLUS  : return BIN_PI_PLUS;
        case PDG_PROTON   : return BIN_PROTON;
        case PDG_ERROR    : return BIN_ERROR;
        default           : return BIN_ERROR;
    }
}

pdg_id get_pdg_id (const particle_bin id)
{ // Convert from a bin to a particle id 
    switch (id)
    {
        case BIN_PI_MINUS : return PDG_PI_MINUS;
        case BIN_MU_PLUS  : return PDG_MU_PLUS;
        case BIN_E_PLUS   : return PDG_E_PLUS;
        case BIN_E_MINUS  : return PDG_E_MINUS;
        case BIN_MU_MINUS : return PDG_MU_MINUS;
        case BIN_PI_PLUS  : return PDG_PI_PLUS;
        case BIN_PROTON   : return PDG_PROTON;
        case BIN_ERROR    : return PDG_ERROR;
        default           : return PDG_ERROR;
    }
}