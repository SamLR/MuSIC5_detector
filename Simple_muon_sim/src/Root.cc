

#include "root.hh"

Root::Root(TString out_root_name) {
    make_root(out_root_name);
    g_nhit=0;
}

Root::~Root() {
    file_out->Write();
    file_out->Close();
    delete file_out;
}

void Root::write() {
    file_out->Write();
}

void Root::make_root(TString root_name) {
    file_out = new TFile(root_name,"recreate");
    
    tree_out = new TTree("t","t");
    
    tree_out->Branch("nhit",&g_nhit,"nhit/I");
//    // apparently bool_t but gods alone know
//    tree_out->Branch("first_step",first_step,"first_step[nhit]/O");
//    tree_out->Branch("last_step" ,last_step,"last_step[nhit]/O");
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
