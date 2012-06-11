// returns a pointer to new file, basically a wrapper for TFile
TFile* init_file(const TString& file_name, // the file to open
    const TString& options="READ", // options: read, write, recreate etc
    const bool verbose=false // more info
){
    // this is a pretty basic wrapper for TFile
    cout << file_name << endl;
    TFile* file = new TFile(file_name, options);
    if (!file->IsOpen()) { 
        exit(1); // if it didn't load something's wrong
    } else {
        if (verbose) cout << file_name << " sucessfully opened"<< endl;
        return file;
    }
}

// returns a pointer to a new TTree, basically another wrapper
typedef void (*set_branch_address)(const T&, const TTree*);
template <class T> 
TTree* init_tree (
    const TFile* file, // file to load from
    const TString& treename,        // tree to load
    const T& branch,                // where to load the branches
    set_branch_address func,
    const bool& verbose=false       // YUM more output!
) {
    TTree* tree = (TTree*) file->Get(treename);
    if(!tree) {
        if(verbose) cout << "ERROR: " << treename << " not found" << endl;
        exit(1); // if it didn't load something's wrong
    } else {
        if(verbose) cout << "Returning "<< treename << endl;
        (*func)(branch, tree);
        return tree;
    }
}

TH1F* init_1Dhist (const TString& name, // histogram name & title
    const int& n_bins, const double& x_low, const double& x_hi, // number of bins, min & max x
    const TString& xtitle,const TString& ytitle // axis titles
) {
    TH1F* hist = new TH1F(name, name, n_bins, x_low, x_hi); // fresh blood!
    hist->GetXaxis()->SetTitle(xtitle);
    hist->GetYaxis()->SetTitle(ytitle);
    return hist;
}

TH2F* init_2Dhist (const TString& name, // histogram name & title
    const int& n_x_bins, const double& x_low, const double& x_hi, // number of bins, min & max x
    const int& n_y_bins, const double& y_low, const double& y_hi, // number of bins, min & max y
    const TString& xtitle, const TString& ytitle // axis titles
) {
    TH2F* hist = new TH2F(name, name, n_x_bins, x_low, x_hi, n_y_bins, y_low, y_hi); // fresh blood!
    hist->GetXaxis()->SetTitle(xtitle);
    hist->GetYaxis()->SetTitle(ytitle);
    return hist;
}

TH3F* init_3Dhist (const TString& name, // histogram name & title
    const int& n_x_bins, const double& x_low, const double& x_hi, // number of bins, min & max x
    const int& n_y_bins, const double& y_low, const double& y_hi, // number of bins, min & may y
    const int& n_z_bins, const double& z_low, const double& z_hi, // number of bins, min & maz z
    const TString& xtitle, const TString& ytitle, const TString& ztitle // axis titles
) {
    TH3F* hist = new TH3F(name, name, n_x_bins, x_low, x_hi, 
        n_y_bins, y_low, y_hi,
        n_z_bins, z_low, z_hi); // fresh blood!
    hist->GetXaxis()->SetTitle(xtitle);
    hist->GetYaxis()->SetTitle(ytitle);
    hist->GetZaxis()->SetTitle(ztitle);
    return hist;
}

template <class T>
T* init_hist(const TString& name, const int* bins, const double* mins, const double* maxs,
    const TString *axis_titles, const int& hist_dimension
){
    switch(hist_dimension){
        case 1:
        return init_1Dhist(name, bins[0], mins[0], maxs[0], axis_titles[0], axis_titles[1]);
        
        case 2:
        return init_2Dhist(name, bins[0], mins[0], maxs[0],
            bins[1], mins[1], maxs[1], axis_titles[0], axis_titles[1]);
            
        case 3:
        return  init_3Dhist(name, bins[0], mins[0], maxs[0],
            bins[1], mins[1], maxs[1], bins[2], mins[2], maxs[2], 
            axis_titles[0], axis_titles[1], axis_titles[2]);
        default:
        cout << "ERROR incorrect number of dimensions (range 1->3) "<< hist_dimension << endl;
        return NULL;
    }
}