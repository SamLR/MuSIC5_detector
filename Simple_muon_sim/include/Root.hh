#ifndef ROOT_H
#define ROOT_H 1

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

class Root
{
public:
    Root(TString out_root_name);
    ~Root();
    
    
    void make_root(TString root_name);
    void write();
    
    /* Output ROOT file to be created */
    TFile* file_out;
    TTree* tree_out;
    int g_nhit;
#define MAX_HIT 500
//    bool last_step[MAX_HIT];  // flags for first/last step in a volume
//    bool first_step[MAX_HIT];
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
    double kinetic[MAX_HIT];
    double edep[MAX_HIT];
    double tof[MAX_HIT];
};

#endif

