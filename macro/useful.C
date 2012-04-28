// copied from scan.C

#include "TFile.h"
#include "TTree.h"
#include <iostream>

#define MAX_HIT 100

struct branches_struct
{
    int g_iev;
    int in_event;
    int in_trackID;
    int in_pdg;
    double in_x; // use the new (AKA global) values for these
    double in_y; 
    double in_z;
    double in_px;
    double in_py;
    double in_pz;
    double in_tof;
    int g_nhit;
    int procid[MAX_HIT];
    int counter[MAX_HIT];
    int trkid[MAX_HIT];
    int parentid[MAX_HIT];
    int pdgid[MAX_HIT];
    double x[MAX_HIT];
    double y[MAX_HIT];
    double z[MAX_HIT];
    double px[MAX_HIT];
    double py[MAX_HIT];
    double pz[MAX_HIT];
    double edep[MAX_HIT];
    double tof[MAX_HIT];
};

char* counter_to_name(int id)
{
    switch (id)
    {
        case 1: return "sci1";
        case 2: return "target";
        case 3: return "sci2";
        case 4: return "degrader";
    }
}
char* pdgid_to_name(int id)
{
    switch (id)
    {
        case 11 : return "e-";
        case 12 : return "nu_e";
        case 13 : return "mu-";
        case 14 : return "nu_mu";

        case -11 : return "e+";
        case -12 : return "nu_e_bar";
        case -13 : return "mu+";
        case -14 : return "nu_mu_bar";
 
        case 22  : return "gamma";
        case 111 : return "pi0";
        case 211 : return "pi+";
        case -211: return "pi-";
        case 2212: return "p";
        case 2112: return "n";
        default  : return "others";
    }
}
char* procid_to_name(int id)
{
    switch (id)
    {
        case 1 :  return "msc";
        case 2 :  return "Transportation";
        case 3 :  return "eIoni";
        case 4 :  return "eBrem";
        case 5 :  return "CoulombScat";
        case 6 :  return "phot";
        case 7 :  return "compt";
        case 8 :  return "muMsc";
        case 9 :  return "muIoni";
        case 10:  return "Decay";
        case 11:  return "hIoni";
        case 12:  return "annihil";
        case 13:  return "conv";
        case 14:  return "ionIoni";
        case 15:  return "muMinusCaptureAtRest";
        case 16:  return "hadElastic";
        case 17:  return "PionPlusInelastic";
        case 18:  return "NeutronInelastic";
        case 19:  return "CHIPSNuclearCaptureAtRest";
        case 20:  return "PionMinusInelastic";
    }
}

void load(char* fname, TFile* file, TTree* tree, branches_struct &branch)
{
    file = new TFile(fname);
    if (!file)
    {
        cerr << "file " << fname<< " not found" << endl;
        return;
    }
    tree = (TTree*) file->Get("t");

    tree->SetBranchAddress("iev",&branch.g_iev);
    tree->SetBranchAddress("in_EventID",&branch.in_event);
    tree->SetBranchAddress("in_TrackID",&branch.in_trackID);
    tree->SetBranchAddress("in_PDGid",&branch.in_pdg);
    tree->SetBranchAddress("in_x",&branch.in_x);
    tree->SetBranchAddress("in_y",&branch.in_y);
    tree->SetBranchAddress("in_z",&branch.in_z); 
    tree->SetBranchAddress("in_Px",&branch.in_px); 
    tree->SetBranchAddress("in_Py",&branch.in_py); 
    tree->SetBranchAddress("in_Pz",&branch.in_pz); 
    tree->SetBranchAddress("in_tof",&branch.in_tof); 
     
    tree->SetBranchAddress("nhit",&branch.g_nhit);
    tree->SetBranchAddress("procid",branch.procid);
    tree->SetBranchAddress("counter",branch.counter);
    tree->SetBranchAddress("trkid",branch.trkid);
    tree->SetBranchAddress("parentid",branch.parentid);
    tree->SetBranchAddress("pdgid",branch.pdgid);
    tree->SetBranchAddress("x",branch.x);
    tree->SetBranchAddress("y",branch.y);
    tree->SetBranchAddress("z",branch.z);
    tree->SetBranchAddress("px",branch.px);
    tree->SetBranchAddress("py",branch.py);
    tree->SetBranchAddress("pz",branch.pz);
    tree->SetBranchAddress("edep",branch.edep);
    tree->SetBranchAddress("tof",branch.tof);
}

void scan (TTree* t, int iev, branches_struct &branch)
{
    int total = t->GetEntries();
    cout << total << endl;
    for (int i=0; i<total; i++) {
        t->GetEntry(i);

        if (iev==branch.g_iev || iev==0) {
            printf("%10s %12s %7s %25s %8s %16s %9s %9s %9s %9s %9s %9s %9s %9s\n",
                    "g_iev",
                    "g_iev_g4bl",
                    "g_nhit",
                    "particle[pid](trkid-prtid)",
                    "counter",
                    "process",
                    "x(mm)",
                    "y(mm)",
                    "z(mm)",
                    "Px(MeV/c)",
                    "Py(MeV/c)",
                    "Pz(MeV/c)",
                    "edep(MeV)",
                    "time(ns)");
            for (int j=0; j<branch.g_nhit; j++) {
                printf("%10d %12d %7d %10s[%4d] (%3d<-%3d) %8s %16s %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf\n",
                        branch.g_iev,
                        // branch.g_iev_g4bl,
                        branch.g_nhit,
                        pdgid_to_name(branch.pdgid[j]),
                        branch.pdgid[j],
                        branch.trkid[j],
                        branch.parentid[j],
                        counter_to_name(branch.counter[j]),
                        procid_to_name(branch.procid[j]),
                        branch.x[j],
                        branch.y[j],
                        branch.z[j],
                        branch.px[j],
                        branch.py[j],
                        branch.pz[j],
                        branch.edep[j],
                        branch.tof[j]);
            }
            if (iev>0) break;
        }
    }
}