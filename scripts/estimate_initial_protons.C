/*
* This will attempt to estimate the number of initial
* particles simulated by G4Beamline and actually saved
* to root files
*  
* Created by: Sam Cook 19 October 2012
*/

void estimate_initial_protons() {
    const int n_files = 41;
    const int max_protons = 25000000;

    const TString file_prefix  = "initial_files/";
    const TString file_suffix  = ".root";
    const TString in_dir_name  = "VirtualDetector";
    const TString in_tree_name = "monitor6";

    int max_eventid_for_file [n_files]; 
    int n_protons [n_files];
    int running_total = 0;
    int prev_max_event_id = 0;
    float p_per_event_for_file [n_files]; // number of secondaries/primary i.e. the error

    for(unsigned int file_id = 0; file_id < n_files; ++file_id) {
        // load file, tree, branch
        TString file_name = get_file_name(file_id, file_prefix, file_suffix);
        TFile* in_file = new TFile(file_name, "READ");
        TDirectoryFile* in_dir = (TDirectoryFile*) in_file->Get(in_dir_name);
        TTree* in_tree = (TTree*) in_dir->Get(in_tree_name);

        float eventID_float = -1.0;
        in_tree->SetBranchAddress("EventID", &eventID_float);

        const int n_entries = in_tree->GetEntries();
        printf("\n\n%i entries found.\n", n_entries);

        in_tree->GetEntry(n_entries-1);
        int eventID = static_cast<int>(eventID_float);
        printf("EventID: %.0f\n", eventID);
        int protons_in_this_file = eventID - prev_max_event_id;
        max_eventid_for_file[file_id] = eventID;
        n_protons[file_id] = protons_in_this_file;
        p_per_event_for_file[file_id] = static_cast<float>(n_entries)/protons_in_this_file;
        running_total += protons_in_this_file;
        printf("Protons in this file: %i\n", protons_in_this_file);
        printf("Running total: %i\n", running_total);
        prev_max_event_id = eventID;
        in_file->Close();
        
    }

    printf("\n%5s %10s %9s %18s\n", "file", "last event", "n_protons", "secondaries/proton");
    for(unsigned int file_id = 0; file_id < n_files; ++file_id) {
        printf("%5i %10i %9i %.5f\n",file_id, max_eventid_for_file[file_id],
                                          n_protons[file_id],
                                          p_per_event_for_file[file_id]);
    }

    printf("\n total protons: %i\n", running_total);
}

TString get_file_name (int file_number, const TString file_prefix, const TString file_suffix){
    TString res = TString(file_prefix);
    res += file_number;
    res += file_suffix;
    return res;
}