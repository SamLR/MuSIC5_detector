
// Loops over all the enteries in a tree and for each executes a function

// this is the general shape of a 'cut function' 
// it is intended to receive an 'in' object (normally a tree's branch)
// and an out object (either another tree or a histogram)
// the bool is for verbose output if desired
typedef void (*entry_fptr)(const S&, const T, const bool);
// loop enteries will loop over all the events in a tree reading from 
// the in_obj_for_cut (normally a branch), applying the cut function and 
// writing to the out_obj_for_cut
// 
// an array (length n_funcs) of cut functions is assumed to be supplied with 
// a corresponding out object for each
template <class S, class T> 
void loop_entries(
    const TTree* in_tree,       // tree to loop over
    const S& in_obj_for_cut,    // input object for cut
    const T* out_obj_for_cut,   // an array of objects size n_func for data to go to
    const unsigned int n_funcs, // how many functions are in the array
    entry_fptr* cuts,         // array of function pointers for each loop
    const bool verbose,   // yay more info!
    const bool testing   // only run the first 1000 entries of a file
)  {

    unsigned int n_entries = -1;

    if (testing) {
        const unsigned int n_entries = 100;
        cout << endl;
        cout << "WARNING testing mode enabled! Only "<<n_entries<<" entries will be read" << endl;
        cout << endl;
    } else {
        n_entries = in_tree->GetEntries();    
    }

    if(verbose) cout << "Tree loaded. "<< n_entries << " entries found." << endl;

    const int tenth = n_entries/10;
    for(int entry = 0; entry < n_entries; ++entry) { 
        // loop over each entry and see if it passes the cuts
        if(entry%tenth==0) cout << "Entry: " << entry<< endl;
        in_tree->GetEntry(entry);
        for(unsigned int func = 0; func < n_funcs; ++func) {
                // Dark voodoo! loop over the array of function pointers
                // calling each function in turn
            (*cuts[func])(in_obj_for_cut, out_obj_for_cut[func], verbose);
        }

    }
    if(verbose) cout << "filling finished" << endl;
}
