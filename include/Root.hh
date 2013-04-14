#ifndef ROOT_H
#define ROOT_H 1

#include "TFile.h"
#include "TTree.h"

class Root
{
public:
    Root(char* in_root_name, char* out_root_name);
    ~Root();
    long long int nevents_g4bl;
    void write();
    void init_root();
    void make_root(char* root_name);
    void create_output(char* root_name);
    long long int open_g4bl(const char* root_name);
    
    bool g4bl_file_enabled;
    char in_root_file_name [200];
    char out_root_file_name[200];
    
    // Input ROOT file from G4beamline, also saved to output in truth data
    TFile* file_g4bl;
    TTree* tree_g4bl;
    int g_iev;      // Unique event ID essentially the row for the entry
    int in_EventID; // Event ID (not unique, can have multiple particles from 1 event)
    int in_TrackID; // Track ID
    int in_PDGid;   // PDG ID
    double in_x;    // Position
    double in_y;
    double in_z; 
    double in_Px;     // Momentum
    double in_Py;
    double in_Pz;
    double in_tof;    // Time since start of G4BL event
    double in_Weight; // Event weighting
    double in_x_new;  // Rotated position (i.e. 36 deg rotation)
    double in_z_new;
    double in_Px_new; // Rotated momentum
    double in_Pz_new;
    
  #define MAX_HIT 10000 // Max array size  
    /* Output ROOT file to be created */
    TFile* file_out;
    // Truth level data
    TTree* truth_tree_out;
    int g_nhit;
    bool last_step  [MAX_HIT]; // flags for first/last step in a volume
    bool first_step [MAX_HIT];
    int procid   [MAX_HIT];    // Process ID (see SteppingAction.cc)
    int counter  [MAX_HIT];    // Counter ID (see SteppingAction.cc)
    int trkid    [MAX_HIT];    // Track ID
    int parentid [MAX_HIT];    // Parent track ID
    int pdgid    [MAX_HIT];    // PDG ID
    double x [MAX_HIT];        // Position
    double y [MAX_HIT];
    double z [MAX_HIT];
    double px [MAX_HIT];       // Momentum
    double py [MAX_HIT];
    double pz [MAX_HIT];
    double kinetic [MAX_HIT];  // Current kinetic energy
    double edep    [MAX_HIT];  // Energy deposited in the current step
    double tof     [MAX_HIT];  // Global time since start of event (not incl 'in_tof')
    
    // MPPC data
    TTree* mppc_tree_out;
    int    mppc_hits;            // How many hits in this entry
    double mppc_x    [MAX_HIT]; // Position of the hit (will need conversion to ID)
    double mppc_y    [MAX_HIT];
    double mppc_z    [MAX_HIT];
    double mppc_time [MAX_HIT]; // When the hit happens

private:
    Root(); // Stop default constructor being used DONT IMPLEMENT THIS!
    Root(Root&); // Stop copy constructor
};

#endif

