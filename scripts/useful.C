// a selection of generally useful root functions
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

// this is the general shape of a 'cut function' 
// it is intended to receive an 'in' object (normally a tree's branch)
// and an out object (either another tree or a histogram)
// the bool is for verbose output if desired
typedef void (*cut_func_ptr)(const S& in_obj, const T out_obj, const bool);
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
    cut_func_ptr* cuts,         // array of function pointers for each loop
    const bool& verbose=false   // yay more info!
)  {
    // cout << "WARNING cheat mode enabled! swap back to actual number of entries" << endl;
    // const unsigned int n_entries = 100;
    const unsigned int n_entries = in_tree->GetEntries();    

    if(verbose) cout << "Tree loaded. "<< n_entries << " entries found." << endl;

    for(unsigned int entry = 0; entry < n_entries; ++entry) { 
        // loop over each entry and see if it passes the cuts
        if(verbose && (entry%1000==0)) cout << "Entry: " << entry<< endl;
        in_tree->GetEntry(entry);
        for(unsigned int func = 0; func < n_funcs; ++func) {
                // Dark voodoo! loop over the array of function pointers
                // calling each function in turn
            (*cuts[func])(in_obj_for_cut, out_obj_for_cut[func], verbose);
        }

    }
    if(verbose) cout << "filling finished" << endl;
}


