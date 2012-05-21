// a selection of generally useful root functions

// returns a pointer to new file, basically a wrapper for TFile
TFile* init_file(const TString& file_name, // the file to open
    const& TString options="READ", // options: read, write, recreate etc
    const bool verbose=false // more info
){
    // this is a pretty basic wrapper for TFile
    TFile* file = new TFile(filename, options);
    if (!file->IsOpen()) { 
        exit(1); // if it didn't load something's wrong
    } else {
        if (verbose) cout << filename << " sucessfully opened"<< endl;
        return file;
    }
}

// returns a pointer to a new TTree, basically another wrapper
template <class T>
typedef void (*set_branch_address)(const T&, const TTree*);
TTree* init_tree(const TFile* file, // file to load from
    const TString& treename,        // tree to load
    const T& branch,                // where to load the branches
    set_branch_address address,     // function to load the branches
    const bool& verbose=false       // YUM more output!
) {
    TTree* tree = (TTree*) file->Get(treename);
    if(!tree) {
        if(verbose) cout << "ERROR: " << treename << " not found" << endl;
        exit(1); // if it didn't load something's wrong
    } else {
        if(verbose) cout << "Returning "<< treename << endl;
        (*address)(branch, tree);
        return tree;
    }
}

// calculates sqrt(x*x + y*y + z*z)
double length (const double& x, const double& y, const double& z){
    return sqrt(x*x + y*y + z*z);
}

// searches for target in the vector
template <class T>
bool is_in (const vector<T>& vec, const T& target){
    for(vector<T>::iterator iter = vec.begin(); iter < vec.end(); ++iter){
        if (*iter == target) return true;
    }
    return false;
}

// general for of the cut function (user supplies)
// S & T are some user defined objects (assumed to be a branch and some
// sort of output object eg a histogram) bool is passed the value of verbose
template <class S, class T>
typedef void (*cut_func_ptr)(const S& branch, const T* TObject, const bool);
void loop_entries(
    const TTree* in_tree,       // tree to loop over
    const S& branch,            // first object for cut (e.g. tree's branch struct)
    const T* object_for_cut,    // second object for cut (e.g hist, out_tree)
    const unsigned int n_funcs, // how many functions are in the array
    cut_func_ptr* cuts,         // array of function pointers for each loop
    const bool& verbose=false   // yay more info!
)  {
    const unsigned int n_entries = in_tree->GetEntries();    
    
    if(verbose) cout << "Tree loaded. "<< n_entries << " entries found." << endl;

    for(unsigned int entry = 0; entry < n_entries; ++entry) { 
        // loop over each entry and see if it passes the cuts
        if(verbose && (entry%1000==0)) cout << "Entry: " << entry<< endl;
        in_tree->GetEntry(entry);
        for(unsigned int func = 0; func < n_funcs; ++func) {
                // Dark voodoo! loop over the array of function pointers
                // calling each function in turn
            (*cuts[func])(branch, cut_to_object, verbose);
        }
            // cut(branch, hit, cut_to_object);

    }
    if(verbose) cout << "filling finished" << endl;
}

