// a selection of generally useful root functions

// returns a pointer to new file, basically a wrapper for TFile
TFile* init_file(const TString& file_name, // the file to open
    const TString& options="READ", // options: read, write, recreate etc
    const bool verbose=false // more info
){
    // this is a pretty basic wrapper for TFile
    TFile* file = new TFile(file_name, options);
    if (!file->IsOpen()) { 
        exit(1); // if it didn't load something's wrong
    } else {
        if (verbose) cout << filename << " sucessfully opened"<< endl;
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
    // void (*set_branch_address)(const T&, const TTree*), // function to set branches
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

// calculates sqrt(x*x + y*y + z*z)
double length (const double& x, const double& y, const double& z){
    return sqrt(x*x + y*y + z*z);
}

// searches for target in the vector
typedef bool (*comparison)(const T&, const T&);
template <class T> bool is_in (const vector<T>& vec, const T& target, comparison func){
    for(vector<T>::iterator iter = vec.begin(); iter < vec.end(); ++iter){
        if ((*func<T>)(*iter,target)) return true;
    }
    return false;
}

// overload for basic equals test
template <class T> bool is_in (const vector<T>& vec, const T& target){
    for(vector<T>::iterator iter = vec.begin(); iter < vec.end(); ++iter){
        if ( (*iter)==target ) return true;
    }
    return false;
}

// general for of the cut function (user supplies)
// S & T are some user defined objects (assumed to be a branch and some
// sort of output object eg a histogram) bool is passed the value of verbose
typedef void (*cut_func_ptr)(const S& in_obj, const T* out_obj, const bool);
template <class S, class T> 
void loop_entries(
    const TTree* in_tree,       // tree to loop over
    const S& in_obj_for_cut,    // input object for cut
    const T** out_obj_for_cut,  // array of pointers to out objects
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
            (*cuts[func])(in_obj_for_cut, out_obj_for_cut[func], verbose);
        }

    }
    if(verbose) cout << "filling finished" << endl;
}

TH1F* init_hist (const TString& name, // histogram name & title
    const int& n_bins, const int& x_low, const int& x_hi, // number of bins, min & max x
    const TString& xtitle,const TString& ytitle // axis titles
) {
    TH1F* hist = new TH1F(name, name, n_bins, x_low, x_hi); // fresh blood!
    hist->GetXaxis()->SetTitle(xtitle);
    hist->GetYaxis()->SetTitle(ytitle);
    return hist;
}

void draw_pretty_two_hists(const TH1F* baseHist, 
    const TH1F* frontHist, 
    const TString title,
    const TString baseTitle,
    const TString frontTitle,
    const TString img_save_location,
    const TString options = ""
) {
    TCanvas* can = new TCanvas(title, title);
    baseHist->SetTitle(title);
    baseHist->Draw(options);
    frontHist->SetLineColor(2);
    frontHist->Draw((options + "SAMES"));
    
    can->Update(); // allows access to the stats boxes (THE FUCK?!)
    TPaveStats* base_st  = (TPaveStats*)  baseHist->FindObject("stats");
    base_st->SetOptStat(1002201); // stats: integral, mean + er, RMS + er, name
    base_st->SetX1NDC(0.65);
    base_st->SetX2NDC(0.9);
    base_st->SetY1NDC(0.59);
    base_st->SetY2NDC(0.74);
    TPaveStats* front_st = (TPaveStats*) frontHist->FindObject("stats");
    front_st->SetOptStat(1002200);
    front_st->SetX1NDC(0.65);
    front_st->SetX2NDC(0.9);
    front_st->SetY1NDC(0.75);
    front_st->SetY2NDC(0.9);
    
        // create a legend for extra info
    TLegend* leg = new TLegend(0.44,0.75,0.64,0.9);
    leg->AddEntry(baseHist, baseTitle); // add the two histrograms to the legend
    leg->AddEntry(frontHist, frontTitle);
    leg->SetFillColor(0);
    leg->Draw();

    can->SaveAs(img_save_location);
}

// the stupidly long variable name is to try and stop namespace collisions @ global
const int __useful_default_colour_selection [] =  {1, 2, 4, 3, 6, 7, 8, 9};
void draw_pretty_hists(const int& n_hists,
    const TH1** hist_array, 
    const TString title,
    const TString img_save_location,
    const int colours* = __useful_default_colour_selection,
    const TString options = ""
) {
    TCanvas* can = new TCanvas(title, title);
    TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
    leg->SetHeader(title);    
    
    for(unsigned int hist = 0; hist < n_hists; ++hist) {
        hist_array[hist]->SetLineColor(colours[hist]);
        TString draw_opt = (hist==0) ? options : (options+ " SAME");
        hist_array[hist]->Draw(draw_opt);
        leg->AddEntry(hist_array[hist]);
    }

    leg->SetFillColor(0);
    leg->Draw();

    can->SaveAs(img_save_location);
}
