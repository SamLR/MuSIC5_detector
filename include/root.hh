#ifndef ROOT_H
#define ROOT_H 1

#include "TFile.h"
#include "TTree.h"

class Root
{
   public:
      Root(char* in_root_name, char* out_root_name);
      int nevents_g4bl;
      void write();
      void make_root(char* root_name);
      int open_g4bl(const char* root_name);

      /* Input ROOT file from G4beamline */
      TFile* file_g4bl;
      TTree* tree_g4bl;
      int g_iev;
      int in_EventID;
      int in_TrackID;
      int in_PDGid;
      double in_x;
      double in_y;
      double in_z;
      double in_Px;
      double in_Py;
      double in_Pz;
      double in_tof;
      double in_Weight;
      double in_x_new;
      double in_z_new;
      double in_Px_new;
      double in_Pz_new;

      /* Output ROOT file to be created */
      TFile* file_out;
      TTree* tree_out;
      int g_nhit;
#define MAX_HIT 500
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

