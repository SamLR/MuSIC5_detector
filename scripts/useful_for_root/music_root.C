
#include "looping.C" // looping functions and some other hand stuff
#include "inits.C" // initialise trees, files and histograms

// only really useful with the MuSIC simulation but they may as well be somewhere
struct in_branch_struct {
    // tree structure as stored by MuSIC simulation
    // note later elements are arrays max (filled) length given by g_nhit
    int g_iev;
    int in_event;
    int in_trackID;
    int in_pdg;
    double in_x; 
    double in_y; 
    double in_z;
    double in_px;
    double in_py;
    double in_pz;
    double in_tof;
    int g_nhit;
    bool last_step[500];
    bool first_step[500];
    int procid[500];
    int counter[500];
    int trkid[500];
    int parentid[500];
    int pdgid[500];
    double x[500];
    double y[500];
    double z[500];
    double px[500];
    double py[500];
    double pz[500];
    double edep[500];
    double tof[500];
};

// enumeration of the counters
enum counter_names { eCN_scint1  = 1,    // scintillator 1
    eCN_target                   = 2,    // stopping target
    eCN_scint2                   = 3,    // scintillator 3
    eCN_degrader                 = 4    // degrader
};

enum pid_names { ePID_pi_minus = -211, // will add neutrinos etc later
    ePID_muon_plus             =  -13,
    ePID_positron              =  -11,
    ePID_electron              =   11, 
    ePID_muon                  =   13,
    ePID_pion                  =  211,
    ePID_proton                = 2212
};

enum process_names { ePN_msc      = 1 , // multiple scattering
    ePN_Transportation            = 2 , // basic transportation
    ePN_eIoni                     = 3 , // electron ionisation
    ePN_eBrem                     = 4 , // electron Bremsstralung
    ePN_CoulombScat               = 5 , // Coulomb scattering
    ePN_phot                      = 6 , // photo electric effect
    ePN_compt                     = 7 , // Compton scattering
    ePN_muMsc                     = 8 , // muon multiple scattering
    ePN_muIoni                    = 9 , // µ ionisation 
    ePN_Decay                     = 10, // decay
    ePN_hIoni                     = 11, // hadron ionisation
    ePN_annihil                   = 12, // annihilation 
    ePN_conv                      = 13, // conversion (eg gamma->e-e+)
    ePN_ionIoni                   = 14, // ion ionisation
    ePN_muMinusCaptureAtRest      = 15, // nuclear capture of µ-
    ePN_hadElastic                = 16, // hadronic elastic scattering
    ePN_PionPlusInelastic         = 17, // π+ in-elastic scattering
    ePN_NeutronInelastic          = 18, // neturon in-elastic scattering
    ePN_CHIPSNuclearCaptureAtRest = 19, // CHIPS nuclear capture
    ePN_PionMinusInelastic        = 20  // π- in-elastic scattering
};


void set_in_branch_address(const in_branch_struct& branch, const TTree* tree) { 
    // set the branches for MuSIC tree (called 't')
    // uses the struct given above
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
    tree->SetBranchAddress("last_step",&branch.last_step);
    tree->SetBranchAddress("first_step",&branch.first_step);
    tree->SetBranchAddress("procid",&branch.procid);
    tree->SetBranchAddress("counter",&branch.counter);
    tree->SetBranchAddress("trkid",&branch.trkid);
    tree->SetBranchAddress("parentid",&branch.parentid);
    tree->SetBranchAddress("pdgid",&branch.pdgid);
    tree->SetBranchAddress("x",&branch.x);
    tree->SetBranchAddress("y",&branch.y);
    tree->SetBranchAddress("z",&branch.z);
    tree->SetBranchAddress("px",&branch.px);
    tree->SetBranchAddress("py",&branch.py);
    tree->SetBranchAddress("pz",&branch.pz);
    tree->SetBranchAddress("edep",&branch.edep);
    tree->SetBranchAddress("tof",&branch.tof);
}

//==============================================================================
// calculates sqrt(x*x + y*y + z*z)
double length (const double& x, const double& y, const double& z){
    return sqrt(x*x + y*y + z*z);
}

// overload for basic equals test
template <class T> 
bool is_in (const vector<T>& vec, const T& target){
    for(vector<T>::iterator iter = vec.begin(); iter < vec.end(); ++iter){
        if ( (*iter)==target ) return true;
    }
    return false;
}
//==============================================================================
// useful hack to select charged pids and group in a sane range (i.e not -211 -> 2212)
const unsigned int& pid_to_apid(const int& inpid){
    // convert a PID to one in a restricted range (for z plotting)
    // ( e==11 || µ==13 || π==211 || p==2212 );
    switch (inpid){
        case ePID_positron  : return 1; 
        case ePID_electron  : return 2; 
        case ePID_muon_plus : return 3; 
        case ePID_muon      : return 4; 
        case ePID_pion      : return 5; 
        case ePID_pi_minus  : return 6; 
        case ePID_proton    : return 7; 
        default: return 0;// shouldn't get called
    }
}
const TString get_name_from_apid(const int& apid){
    switch(apid){
        case 1:  return TString("positron"); 
        case 2:  return TString("electron");  
        case 3:  return TString("muon_plus");  
        case 4:  return TString("muon_minus"); 
        case 5:  return TString("pion_plus");  
        case 6:  return TString("pion_minus"); 
        case 7:  return TString("proton");     
        case 0:  return TString("unknown");   
        default: return TString("unknown");  
    }
}
const TString get_name_from_pid(const int& pid){
    const int& tmp = pid_to_apid(pid);
    return get_name_from_alt_pid(tmp);
}

//==============================================================================

// these are defaults for the following über function.
// leading '_'s to avoid namespace collisions, at global because I'm lazy
// assume 3 functions
const int    _axis_bins_default [3*3] = {50,  50,  50 ,50,  50,  50 ,50,  50,  50 };
const double _axis_mins_default [3*3] = {0,   0,   0  ,0,   0,   0  ,0,   0,   0  };
const double _axis_maxs_default [3*3] = {200, 200, 200,200, 200, 200,200, 200, 200};

const TString _axis_titles_default [3*3] = {"Momentum (MeV/c)", "Count", "Z", 
    "Momentum (MeV/c)", "Count", "Z", "Momentum (MeV/c)", "Count", "Z"};

// templated to allow use by (in theory) any object
// in practice only pass in histograms
// 

template <class H>
void fill_hists(const unsigned int& n_files,
    const TString& file_prefix,
    const TString& save_file_name,
    const TString* file_roots,
    const TString* func_name_roots,
    const unsigned int& n_funcs,
    const H* hists[n_files][n_funcs],
    entry_fptr* cuts,
    const int hist_dimension,
    const TString& file_suffix=".root",
    const TString& img_suffix=".eps",

    const TString** axis_titles = _axis_titles_default,
    const int    axis_bins [n_funcs*3] = _axis_bins_default, 
    const double axis_mins [n_funcs*3] = _axis_mins_default,
    const double axis_maxs [n_funcs*3] = _axis_maxs_default,
    const bool& verbose = false,
    const bool& testing = false
){
    // root file to save things in
    if (!testing){
        TString out_file_name = file_prefix + save_file_name + file_suffix;
        TFile* out_file = init_file(out_file_name, "RECREATE");
    }
    // branch to address to
    in_branch_struct branch;

    for(unsigned int file_no = 0; file_no < n_files; ++file_no) {
        H** hist_set = hists[file_no]; // alias to the set of hists we want
        cout << endl<< "Starting file "<< file_roots[file_no] << endl;

        // munge the filename
        TString resolved_filename = file_prefix + file_roots[file_no] + file_suffix;

        // open up and initialise things
        TFile* in_file = init_file(resolved_filename, "READ", verbose);
        TTree* in_tree = init_tree<in_branch_struct>(in_file, "t", branch, &set_in_branch_address, verbose);

        // create the histograms then fill them in the loop
        if (!testing) out_file->cd(); // make sure they're added to the save file

        for(unsigned int func = 0; func < n_funcs; ++func) {
            // initialise the histograms
            TString name = func_name_roots[func] +"_" + file_roots[file_no];
            if(verbose) cout << "Making histogram " << name << endl;
            // hist_set[func] = init_1Dhist(name, n_bins, x_min, x_max, xtitle, ytitle);
            hist_set[func] = init_hist<H>(name, &(axis_bins[func*3]), 
                &(axis_mins[func*3]), &(axis_maxs[func*3]), &(axis_titles[func*3]), hist_dimension);
        }
        // loop-de-loop              
        loop_entries<in_branch_struct, H*>(in_tree, branch, hist_set, n_funcs, cuts, verbose, testing);
    }
    // cout << "out_file not enabled. Uncomment it, please" << endl;
    if (!testing) out_file->Write();
    cout << "It... is done." << endl;
}
