// 
// Functions to use for cutting and processing a hit within a branch of
// a MuSIC out tree
//

#include "music_root.C"

// Function pointers that apply some sort of boolean test to the
// values of branch each hit, eg (pdgid==13), (counter==1) etc.
// They should return true if analysis of the loop should continue
typedef const bool (*hit_cut_fptr)(const in_branch_struct&, const int&);
// uses the branch to calculate a weight for filling a histogram
typedef const double (*weight_func)(const in_branch_struct&, const int&);
// other useful typedefs 
typedef vector<int> intvec;
typedef vector<double> dblvec;

//============================================================================

// These are looping functions (for passing indirectly to loop_entries)
// These _must_ have wrapper functions that comply with 'hit_looping_fptr'
// written for them to work, this is extra effort but does make these 
// more flexible without having exceptionally complex calls

// NOTE: it has been considered making void()(branch, hist, hit_cut_fptr)
// the standard for looping but this breaks flexibility (hit cuts are MuSIC
// specific) as well as not accounting for requiring multiple cuts 
// (i.e. one to select muons at scint 1, one to select electrons are scint 1 or 2)

// Sum the energy deposited by tracks passing some cut. 
void edep_sum(const in_branch_struct& branch, const TH1F* hist, hit_cut_fptr* cut){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    dblvec edeps;
    intvec seen_tracks;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        bool cut_pass = (*cut)(branch,hit);

        if (cut_pass) {
            // if it's new add it to the track & edep vectors
            // otherwise add its edep to the correct value
            const int trackID = branch.trkid[hit];
            const double edep = branch.edep[hit];
            bool seen_it = is_in(seen_tracks, trackID);

            if(seen_it){
                // find and increment the edep value
                // insert for loop
                for(unsigned int i = 0; i < seen_tracks.size(); ++i) {
                    if(seen_tracks[i] == trackID){
                        edeps[i] += edep;
                    }
                }
            } else {
                // new track add it to the vectors
                // trackID & edep should have same index
                seen_tracks.push_back(trackID);
                edeps.push_back(edep);
            }
        } 
    }

    // Now that all the various track's deposited energies have been read,
    // fill the histogram (want to plot the sum of the energy deposited in the
    // counter, not just the energy deposited in a step).
    if (edeps.size() == 0) return; // nothing to be filled
    dblvec::iterator iter = edeps.begin();

    for(iter; iter < edeps.end(); ++iter) hist->Fill( (*iter) ); 
}

// Plots the momentum of tracks that pass some cut
// NB this will plot the momentum for each track only once: the first time 
// that track is seen. 
void mom_at(const in_branch_struct& branch, const TH1F* hist, hit_cut_fptr* cut){
    const unsigned int n_hits = branch.g_nhit;
    bool verbose = false;
    if (n_hits == 0) return;

    // dblvec edeps;
    intvec seen_tracks;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        bool cut_pass = (*cut)(branch,hit);
        if (cut_pass) {

            // if it's new add it to the track & edep vectors
            // otherwise add its edep to the correct value
            const int trackID = branch.trkid[hit];
            bool seen_it = is_in(seen_tracks, trackID);

            if(!seen_it){
                // new particle, record it and then make sure we don't see it again
                seen_tracks.push_back(trackID);
                const double momentum = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                hist->Fill(momentum);
            } 
        } 
    }
}

// counts the incidence of each type of particle for hits that pass a cut
// particles are numbered according to pid_to_apid from cuts.C
void particle_type_counter(const in_branch_struct& branch, const TH1F* hist, 
    hit_cut_fptr* cut, const int max_particles=__n_pids
){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;
    
    intvec altpid_counts(max_particles);
    intvec seen_tracks;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        bool cut_pass = (*cut)(branch,hit);

        if (cut_pass) {
            // if it's new add it to the track & edep vectors
            // otherwise add its edep to the correct value
            const int trackID = branch.trkid[hit];
            bool seen_it = is_in(seen_tracks, trackID);

            if( !seen_it ){
                const int apid =  pid_to_apid(branch.pdgid[hit]);
                altpid_counts[apid] += 1;
            }
        } 
    }

    // Now that all the various track's deposited energies have been read,
    // fill the histogram (want to plot the sum of the energy deposited in the
    // counter, not just the energy deposited in a step).
    for(unsigned int pid = 0; pid < max_particles; ++pid) {
        // fill each bin with weight equal to the number of times seen
        hist->Fill(pid, altpid_counts[pid]); 
    }
}

// applies two cuts and records momentum which passes cut1 (the first time) 
// for tracks that subsequentially also pass cut2 (first time only)
// if testing is true this will print any subsequent times cut2 is passed
void record_momentum_at_cut1_if_also_cut2(const in_branch_struct& branch, 
    const TH1F* hist, hit_cut_fptr* cut1, hit_cut_fptr* cut2, const bool testing=false
){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;
    
    intvec cut1_tracks;
    intvec cut2_tracks;
    dblvec cut1_momentum;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        const bool cut1_pass = (*cut1)(branch,hit);
        const bool cut2_pass = (*cut2)(branch,hit);
        const unsigned int trackID = branch.trkid[hit];
        
        if (cut1_pass) {
            bool seen_it = is_in<int>(cut1_tracks, trackID);
            if( !seen_it ){
                // new track record it!
                cut1_tracks.push_back(trackID);
                double momentum = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
                cut1_momentum.push_back(momentum);
            }
        } else if (cut2_pass) {
            bool seen_it = is_in<int>(cut2_tracks, trackID);
            if( !seen_it ){
                // new track record it!
                cut2_tracks.push_back(trackID);
                // go find its partner from cut1
                for(unsigned int i = 0; i < cut1_tracks.size(); ++i) {
                    if(cut1_tracks[i] == trackID){
                        hist->Fill(cut1_momentum[i]);
                    }
                }
            } else if (testing) {
                cout << "same track has passed cut2 twice" << endl;
            }
        }
    }
}

// record the initial momentum of a particle that passes cut1 and plot it 
// against the energy depositied by a matching track that passes cut2
void momentum_vs_edep_at(const in_branch_struct& branch, 
    const TH2F* hist, hit_cut_fptr* cut1, hit_cut_fptr* cut2, const bool testing=false
){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    int current_index = 0; // keeps track of the last filled position
    int seen_tracks [n_hits];
    double edep_array [n_hits]; // keeps the running total of edep 
    double mom_array [n_hits]; // stores the initial momentum

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        const bool cut1_pass = (*cut1)(branch,hit);
        const bool cut2_pass = (*cut2)(branch,hit);
        const int trackID = branch.trkid[hit];
        bool seen_it = false;
        
        if ( (!cut1_pass) && (!cut2_pass) )  continue;
        
        // create a pointer to the array that loops whilst it's less than the 
        // maximum currently filled position
        for(int* track = seen_tracks; track < &(seen_tracks[current_index]); ++track) {
            if ( cut2_pass && (*track) == trackID) {
                // it's passed the second cut and we've already seen it
                seen_it = true;
                
                // the difference of two pointers is the number of elements
                // of the given type (i.e. int) that would fit between them
                int index = (track - seen_tracks); 
                edep_array[index] += branch.edep[hit];
            } else if ( (*track) == trackID ) {
                // we've already seen it
                seen_it = true;
            }
        }
        // if it's not been found add it 
        if (cut1_pass && !seen_it) {
                // new track record it!
            seen_tracks[current_index]    = trackID;
            double momentum = length (branch.px[hit], branch.py[hit], branch.pz[hit]);
            mom_array[current_index]  = momentum;
            edep_array[current_index] = branch.edep[hit];  
            ++current_index;
        } 
    }
    for(unsigned int i = 0; i < current_index; ++i) {
        hist->Fill(mom_array[i], edep_array[i]);
    }
}

// loop over hits, check if some cut is passed; if so fill the histogram
// with XY position and the result of the weight function (e.g. the particles'
// momentum, PID...) 
void xy_sum_at(const in_branch_struct& branch, 
    const TH2F* hist, hit_cut_fptr* cut, weight_func* get_weight, const bool testing=false
){
    const unsigned int n_hits = branch.g_nhit;

    if (n_hits == 0) return;

    for(unsigned int hit = 0; hit < n_hits; ++hit) {
        const bool cut_pass = (*cut)(branch,hit);      
                                            
        if(cut_pass){
            // if it passes the cut: plot it!
            const double weight = (*get_weight)(branch, hit);
            const double x = branch.x[hit];
            const double y = branch.y[hit];
            hist->Fill(x, y, weight);
        }
    }
}      

//==============================================================================
// Weight functions

// get the momentum of the particle
const double get_momentum(const in_branch_struct& branch, const int& hit){
    return length (branch.px[hit], branch.py[hit], branch.pz[hit]);
}
// hack for counting particles
const double get_one(const in_branch_struct& branch, const int& hit){
    return 1.0;
}
// get the PID
const double get_pid(const in_branch_struct& branch, const int& hit){
    return static_cast<double>(branch.pdgid[hit]);
}
//==============================================================================
// cut for muons decaying in the ST
bool mu_decaying_in_ST(const in_branch_struct& branch, const int& hit) {
    return ((branch.procid[hit]==ePN_Decay) 
        && (branch.counter[hit]==eCN_target) 
        && (abs(branch.pdgid[hit])==ePID_muon));
}

//==============================================================================
// cuts for momentum plots: charged particles at degrader & scint 1 first step only 
// first step µ+'s in degrader (counter==4)

// any charged particle in scint1 or degrader
bool first_scint1_charged(const in_branch_struct& branch, const int& hit){
    const int pid = branch.pdgid[hit];
    bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );
    return (charged && branch.first_step[hit]  && (branch.counter[hit]==1));
}
bool first_deg_charged(const in_branch_struct& branch, const int& hit){
    const int pid = branch.pdgid[hit];
    bool charged = ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );
    return (charged && branch.first_step[hit] && (branch.counter[hit]==4));
}

// muon first step in degrader or scint1
bool first_scint1_muon(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (abs(branch.pdgid[hit]) == 13) && (branch.counter[hit]==1));
}
bool first_deg_muon(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (abs(branch.pdgid[hit]) == 13) && (branch.counter[hit]==4));
}
// pion, first step in scint1 or degrader
bool first_scint1_pion(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (abs(branch.pdgid[hit]) == 211) && (branch.counter[hit]==1));
}
bool first_deg_pion(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (abs(branch.pdgid[hit]) == 211) && (branch.counter[hit]==4));
}
// e first step in scint1 or degrader
bool first_scint1_e(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (abs(branch.pdgid[hit]) == 11) && (branch.counter[hit]==1));
}
bool first_deg_e(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (abs(branch.pdgid[hit]) == 11) && (branch.counter[hit]==4));
}
// first step p's in scintillator & degrader
bool first_scint1_proton(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 2212) && (branch.counter[hit]==1));
}
bool first_deg_proton(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 2212) && (branch.counter[hit]==4));
}

// as above but with charge splitting
bool first_deg_mu_plus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == -13) && (branch.counter[hit]==4));
}
// first step µ+'s in scintillator (counter==1)
bool first_scint1_mu_plus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == -13) && (branch.counter[hit]==1));
}
// first step µ-'s in degrader (counter==4)
bool first_deg_mu_minus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 13) && (branch.counter[hit]==4));
}
// first step µ-'s in scintillator (counter==1)
bool first_scint1_mu_minus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 13) && (branch.counter[hit]==1));
}

//  first step π+'s in degrader (counter==4)
bool first_deg_pion_plus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 211) && (branch.counter[hit]==4));
}
// first step π+'s in scintillator (counter==1)
bool first_scint1_pion_plus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 211) && (branch.counter[hit]==1));
}
// first step π-'s in degrader (counter==4)
bool first_deg_pion_minus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == -211) && (branch.counter[hit]==4));
}
// first step π-'s in scintillator (counter==1)
bool first_scint1_pion_minus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == -211) && (branch.counter[hit]==1));
}

//  first step e+'s in degrader (counter==4)
bool first_deg_e_plus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == -11) && (branch.counter[hit]==4));
}
// first step e+'s in scintillator (counter==1)
bool first_scint1_e_plus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == -11) && (branch.counter[hit]==1));
}
// first step e-'s in degrader (counter==4)
bool first_deg_e_minus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 11) && (branch.counter[hit]==4));
}
// first step e-'s in scintillator (counter==1)
bool first_scint1_e_minus(const in_branch_struct& branch, const int& hit){
    return (branch.first_step[hit] && (branch.pdgid[hit] == 11) && (branch.counter[hit]==1));
}


//============================================================================
// Cuts for edep, muons only 
bool first_in_scint1(const in_branch_struct& branch, const int& hit){
    return ( branch.first_step[hit] && ( branch.counter[hit]==1 ));
}
// muons at the degrader
bool muon_deg(const in_branch_struct& branch, const int& hit){
    return ( (branch.counter[hit]==4) && ( abs(branch.pdgid[hit])==13 ));
}
// µ+ @ degrader
bool mu_plus_deg(const in_branch_struct& branch, const int& hit){
    return ( (branch.counter[hit]==4) && ( branch.pdgid[hit]==-13 ));
}
// µ- @ degrader
bool mu_minus_deg(const in_branch_struct& branch, const int& hit){
    return ( (branch.counter[hit]==4) && ( branch.pdgid[hit]==13 ));
}
// muons @ degrader, no parent
bool muon_deg_no_parent(const in_branch_struct& branch, const int& hit){
    return ( (branch.counter[hit]==4) && ( abs(branch.pdgid[hit])==13 ) && (branch.parentid[hit] == 0));
}
// muons @ scint1, no parent
bool muon_scint1_no_parent(const in_branch_struct& branch, const int& hit){
    return ( (branch.counter[hit]==1) && ( abs(branch.pdgid[hit])==13 ) && (branch.parentid[hit] == 0));
}

//==============================================================================
// Old style checks, most of these need re-writing/scrapping
// check only for muons
bool muon_only_check(const in_branch_struct& branch, const int& hit){
    // muons only
    return (abs(branch.pdgid[hit])==13); 
}
// check for positive muons
bool mu_plus_only_check(const in_branch_struct& branch, const int& hit){
    // muons only
    return (branch.pdgid[hit]==-13); 
}
// negative muons
bool mu_minus_only_check(const in_branch_struct& branch, const int& hit){
    // muons only
    return (branch.pdgid[hit]==13); 
}
// muons that are not daughter particles (i.e. parents only)
bool parent_muon_only_check(const in_branch_struct& branch, const int& hit){
    // muons only
    return (abs(branch.pdgid[hit])==13 && branch.parentid[hit]==0); 
}
// parent muons taking their first step in a volume
bool first_muon_only_check_no_parent(const in_branch_struct& branch, const int& hit){
    // muons only
    return (abs(branch.pdgid[hit])==13 && (branch.first_step[hit]) && branch.parentid[hit]==0); 
}
// muons or electrons (i.e. muons and their charged daughter particles)
bool stopped_muon_pid_check(const in_branch_struct& branch, const int& hit){
    // muons or their decay daughters (electrons)
    return  (abs(branch.pdgid[hit])==13 || abs(branch.pdgid[hit])==11); 
}
// any charged particle (pions, muons, electrons and protons; positive or negative)
bool charged_check(const in_branch_struct& branch, const int& hit){
    // all charged particles
    const int pid = branch.pdgid[hit];
    return ( abs(pid)==11 || abs(pid)==13 || abs(pid)==211 || pid==2212 );
}

