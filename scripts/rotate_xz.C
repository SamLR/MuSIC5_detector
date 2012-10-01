#include <iostream>

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TMath.h"

struct in_branch{
    float x;        
    float y;        
    float z;        
    float Px;       
    float Py;       
    float Pz;       
    float t;        
    float PDGid;    
    float EventID;  
    float TrackID;  
    float ParentID; 
    float Weight;   
};

struct out_branch{
    int EventID;    
    int TrackID;    
    int PDGid;      
    double x;       
    double y;       
    double z;       
    double Px;      
    double Py;      
    double Pz;      
    double tof;     
    double Weight;  
    double x_new;   
    double z_new;   
    double Px_new;  
    double Pz_new;  
};

void set_in_branch_addresses  (TTree* tree, const in_branch& branch);
void set_out_branch_addresses (TTree* tree, const out_branch& branch);
void process_in_branch_to_out(const in_branch& in, out_branch& out, const double* cos_sin_x);

void rotate_xz() {
    const double theta_deg = 36.0;

    const TString in_filename  = "out.root";
    const TString out_filename = "out_36_rotate.root";
    const TString in_dir_name  = "VirtualDetector";
    const TString in_treename  = "monitor6";
    const TString out_treename = "t";

    TFile* in_file = new TFile(in_filename, "READ");
    TDirectoryFile* in_dir = (TDirectoryFile*) in_file->Get(in_dir_name);
    
    TTree* in_tree = (TTree*) in_dir->Get(in_treename);
    in_branch in_data;
    set_in_branch_addresses(in_tree, in_data);

    TFile* out_file = new TFile(out_filename, "RECREATE");
    TTree* out_tree = new TTree("t", "t");
    out_branch out_data;
    set_out_branch_addresses(out_tree, out_data);

    const double theta_rad = theta_deg*TMath::DegToRad();
    const double cos_sin_x [2] = {cos(theta_rad), sin(theta_rad)};

    const int n_entries_in = in_tree->GetEntries();
    for(int in_entry = 0; in_entry < n_entries_in; ++in_entry) {
        in_tree->GetEntry(in_entry);
        process_in_branch_to_out(in_data, out_data, cos_sin_x);
        out_tree->Fill();
    }
    out_file->Write();
    out_file->Close();
}

void process_in_branch_to_out(const in_branch& in, out_branch& out, const double* cos_sin_x){
    out.EventID = in.EventID;    
    out.TrackID = in.TrackID;    
    out.PDGid = in.PDGid;      
    out.x = in.x;       
    out.y = in.y;       
    out.z = in.z;       
    out.Px = in.Px;      
    out.Py = in.Py;      
    out.Pz = in.Pz;      
    out.tof = in.t;     
    out.Weight = in.Weight;  

    // x = x cos + z sin
    // z = z cos - x sin
    const double x_new = in.x * cos_sin_x[0] + in.z * cos_sin_x[1];
    const double z_new = in.z * cos_sin_x[0] - in.x * cos_sin_x[1];
    const double Px_new = in.Px * cos_sin_x[0] + in.Pz * cos_sin_x[1];
    const double Pz_new = in.Pz * cos_sin_x[0] - in.Px * cos_sin_x[1];

    out.x_new = x_new;   
    out.z_new = z_new;   
    out.Px_new = Px_new;  
    out.Pz_new = Pz_new;
}

void set_in_branch_addresses  (TTree* tree, in_branch& branch) {
    tree->SetBranchAddress("x", &branch.x);
    tree->SetBranchAddress("y", &branch.y);
    tree->SetBranchAddress("z", &branch.z);
    tree->SetBranchAddress("Px", &branch.Px);
    tree->SetBranchAddress("Py", &branch.Py);
    tree->SetBranchAddress("Pz", &branch.Pz);
    tree->SetBranchAddress("t", &branch.t);
    tree->SetBranchAddress("PDGid", &branch.PDGid);
    tree->SetBranchAddress("EventID", &branch.EventID);
    tree->SetBranchAddress("TrackID", &branch.TrackID);
    tree->SetBranchAddress("ParentID", &branch.ParentID);
    tree->SetBranchAddress("Weight", &branch.Weight);
}

void set_out_branch_addresses (TTree* tree, out_branch& branch) {
    tree->Branch("EventID", &branch.EventID, "EventID/I");   
    tree->Branch("TrackID", &branch.TrackID, "TrackID/I");   
    tree->Branch("PDGid", &branch.PDGid, "PDGid/I");   
    tree->Branch("x", &branch.x, "x/D");
    tree->Branch("y", &branch.y, "y/D");
    tree->Branch("z", &branch.z, "z/D");
    tree->Branch("Px", &branch.Px, "Px/D");
    tree->Branch("Py", &branch.Py, "Py/D");
    tree->Branch("Pz", &branch.Pz, "Pz/D");
    tree->Branch("tof", &branch.tof, "tof/D");
    tree->Branch("Weight", &branch.Weight, "Weight/D");
    tree->Branch("x_new", &branch.x_new, "x_new/D");
    tree->Branch("z_new", &branch.z_new, "z_new/D");
    tree->Branch("Px_new", &branch.Px_new, "Px_new/D");
    tree->Branch("Pz_new", &branch.Pz_new, "Pz_new/D");
}





