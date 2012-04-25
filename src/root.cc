#include "root.hh"

Root::Root(char* in_root_name, char* out_root_name)
{
   nevents_g4bl = open_g4bl(in_root_name);
   printf("in_root_name %s has %d entries\n",in_root_name,nevents_g4bl);
   make_root(out_root_name);
   g_nhit=0;
}
void Root::write()
{
   tree_out->Print();
   file_out->cd();
   tree_out->Write();
}
void Root::make_root(char* root_name)
{
   file_out = new TFile(root_name,"recreate");

   tree_out = new TTree("t","t");

   tree_out->Branch("iev",&g_iev,"iev/I");
   tree_out->Branch("in_EventID",&in_EventID,"in_EventID/I");
   tree_out->Branch("in_TrackID",&in_TrackID,"in_TrackID/I");
   tree_out->Branch("in_PDGid",&in_PDGid,"in_PDGid/I");
   tree_out->Branch("in_x",&in_x,"in_x/D");
   tree_out->Branch("in_y",&in_y,"in_y/D");
   tree_out->Branch("in_z",&in_z,"in_z/D");
   tree_out->Branch("in_Px",&in_Px,"in_Px/D");
   tree_out->Branch("in_Py",&in_Py,"in_Py/D");
   tree_out->Branch("in_Pz",&in_Pz,"in_Pz/D");
   tree_out->Branch("in_tof",&in_tof,"in_tof/D");
   tree_out->Branch("in_Weight",&in_Weight,"in_Weight/D");
   tree_out->Branch("in_x_new",&in_x_new,"in_x_new/D");
   tree_out->Branch("in_z_new",&in_z_new,"in_z_new/D");
   tree_out->Branch("in_Px_new",&in_Px_new,"in_Px_new/D");
   tree_out->Branch("in_Pz_new",&in_Pz_new,"in_Pz_new/D");

   tree_out->Branch("nhit",&g_nhit,"nhit/I");
   tree_out->Branch("procid",procid,"procid[nhit]/I");
   tree_out->Branch("counter",counter,"counter[nhit]/I");
   tree_out->Branch("trkid",trkid,"trkid[nhit]/I");
   tree_out->Branch("parentid",parentid,"parentid[nhit]/I");
   tree_out->Branch("pdgid",pdgid,"pdgid[nhit]/I");
   tree_out->Branch("x",x,"x[nhit]/D");
   tree_out->Branch("y",y,"y[nhit]/D");
   tree_out->Branch("z",z,"z[nhit]/D");
   tree_out->Branch("px",px,"px[nhit]/D");
   tree_out->Branch("py",py,"py[nhit]/D");
   tree_out->Branch("pz",pz,"pz[nhit]/D");
   tree_out->Branch("kinetic",kinetic,"kinetic[nhit]/D");
   tree_out->Branch("edep",edep,"edep[nhit]/D");
   tree_out->Branch("tof",tof,"tof[nhit]/D");
}
int Root::open_g4bl(const char* root_name)
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

