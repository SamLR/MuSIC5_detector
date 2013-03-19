#include "root.hh"

Root::Root(char* in_root_name, char* out_root_name)
{
    nevents_g4bl = open_g4bl(in_root_name);
    printf("in_root_name %s has %lld entries\n",in_root_name,nevents_g4bl);
    make_root(out_root_name);
    g_nhit   = 0;
    mppc_hits = 0;
}

Root::~Root()
{
    file_out->Close();
    file_g4bl->Close();
    // Trees should get auto deleted
}

void Root::write()
{
    printf("\n\nPrinting the truth tree\n\n");
    truth_tree_out->Print();
    printf("\n\nPrinting the MPPC tree\n\n");
    mppc_tree_out->Print();
    file_out->cd();
    truth_tree_out->Write();
    mppc_tree_out->Write();
}

void Root::make_root(char* root_name)
{
    file_out = new TFile(root_name,"recreate");
    
    // Tree to store truth level data (only charged particles in
    // scintillators, target, degrader or just before degrader)
    truth_tree_out = new TTree("truth","truth");
    
    // Input information
    truth_tree_out->Branch("iev",        &g_iev,      "iev/I");
    truth_tree_out->Branch("in_EventID", &in_EventID, "in_EventID/I");
    truth_tree_out->Branch("in_TrackID", &in_TrackID, "in_TrackID/I");
    truth_tree_out->Branch("in_PDGid",   &in_PDGid,   "in_PDGid/I");
    // In position
    truth_tree_out->Branch("in_x", &in_x, "in_x/D");
    truth_tree_out->Branch("in_y", &in_y, "in_y/D");
    truth_tree_out->Branch("in_z", &in_z, "in_z/D");
    // In momentum
    truth_tree_out->Branch("in_Px", &in_Px, "in_Px/D");
    truth_tree_out->Branch("in_Py", &in_Py, "in_Py/D");
    truth_tree_out->Branch("in_Pz", &in_Pz, "in_Pz/D");
    // 'Real' start time
    truth_tree_out->Branch("in_tof", &in_tof, "in_tof/D");
    truth_tree_out->Branch("in_Weight", &in_Weight, "in_Weight/D");
    // Rotated start position (x & z only)
    truth_tree_out->Branch("in_x_new", &in_x_new, "in_x_new/D");
    truth_tree_out->Branch("in_z_new", &in_z_new, "in_z_new/D");
    // Rotate start momentum (x & z only)
    truth_tree_out->Branch("in_Px_new", &in_Px_new, "in_Px_new/D");
    truth_tree_out->Branch("in_Pz_new", &in_Pz_new, "in_Pz_new/D");
    
    // Number of hits in this event
    truth_tree_out->Branch("nhit",&g_nhit,"nhit/I");
    // Was it a first/last step (these are arrays so automatically converted to pointers)
    truth_tree_out->Branch("first_step", first_step, "first_step[nhit]/O"); // O = bool apparently
    truth_tree_out->Branch("last_step" , last_step,  "last_step[nhit]/O");
    // Process, counter, track, parent and PDG IDs
    truth_tree_out->Branch("procid",   procid,   "procid[nhit]/I");
    truth_tree_out->Branch("counter",  counter,  "counter[nhit]/I");
    truth_tree_out->Branch("trkid",    trkid,    "trkid[nhit]/I");
    truth_tree_out->Branch("parentid", parentid, "parentid[nhit]/I");
    truth_tree_out->Branch("pdgid",    pdgid,    "pdgid[nhit]/I");
    // Position
    truth_tree_out->Branch("x", x, "x[nhit]/D");
    truth_tree_out->Branch("y", y, "y[nhit]/D");
    truth_tree_out->Branch("z", z, "z[nhit]/D");
    // Momentum
    truth_tree_out->Branch("px", px, "px[nhit]/D");
    truth_tree_out->Branch("py", py, "py[nhit]/D");
    truth_tree_out->Branch("pz", pz, "pz[nhit]/D");
    // Kinetic energy
    truth_tree_out->Branch("kinetic", kinetic, "kinetic[nhit]/D");
    // Energy depositied in this volume
    truth_tree_out->Branch("edep", edep, "edep[nhit]/D");
    // Global time (i.e. since start of event, doesn't include 'in_tof')
    truth_tree_out->Branch("tof", tof, "tof[nhit]/D");
    
    // Tree to store the information on photon hits at the MPPC
    mppc_tree_out = new TTree("mppc","mppc");
    // number of photons, which mppc they hit and when (global time)
    mppc_tree_out->Branch("mppc_hits", &mppc_hits,  "mppc_hits/I");
    mppc_tree_out->Branch("mppc_x",     mppc_x,    "mppc_x[mppc_hits]/D");
    mppc_tree_out->Branch("mppc_y",     mppc_y,    "mppc_y[mppc_hits]/D");
    mppc_tree_out->Branch("mppc_z",     mppc_z,    "mppc_z[mppc_hits]/D");
    mppc_tree_out->Branch("mppc_time",  mppc_time, "mppc_time[mppc_hits]/D");
}
long long int Root::open_g4bl(const char* root_name)
{
    file_g4bl = new TFile(root_name);
    tree_g4bl = (TTree*)file_g4bl->Get("t");
    tree_g4bl->SetBranchAddress("EventID",&in_EventID);
    tree_g4bl->SetBranchAddress("TrackID",&in_TrackID);
    tree_g4bl->SetBranchAddress("PDGid",&in_PDGid);
    tree_g4bl->SetBranchAddress("x",&in_x);
    tree_g4bl->SetBranchAddress("y",&in_y);
    tree_g4bl->SetBranchAddress("z",&in_z);
    tree_g4bl->SetBranchAddress("Px",&in_Px);
    tree_g4bl->SetBranchAddress("Py",&in_Py);
    tree_g4bl->SetBranchAddress("Pz",&in_Pz);
    tree_g4bl->SetBranchAddress("tof",&in_tof);
    tree_g4bl->SetBranchAddress("Weight",&in_Weight);
    tree_g4bl->SetBranchAddress("x_new",&in_x_new);
    tree_g4bl->SetBranchAddress("z_new",&in_z_new);
    tree_g4bl->SetBranchAddress("Px_new",&in_Px_new);
    tree_g4bl->SetBranchAddress("Pz_new",&in_Pz_new);
    return tree_g4bl->GetEntries();
}

