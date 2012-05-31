// a selection of generally useful root functions

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
typedef void (*cut_func_ptr)(const S& in_obj, const T out_obj, const bool);
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

TH1F* init_1Dhist (const TString& name, // histogram name & title
    const int& n_bins, const int& x_low, const int& x_hi, // number of bins, min & max x
    const TString& xtitle,const TString& ytitle // axis titles
) {
    TH1F* hist = new TH1F(name, name, n_bins, x_low, x_hi); // fresh blood!
    hist->GetXaxis()->SetTitle(xtitle);
    hist->GetYaxis()->SetTitle(ytitle);
    return hist;
}

TH2F* init_2Dhist (const TString& name, // histogram name & title
    const int& n_x_bins, const int& x_low, const int& x_hi, // number of bins, min & max x
    const int& n_y_bins, const int& y_low, const int& y_hi, // number of bins, min & may y
    const TString& xtitle, const TString& ytitle // axis titles
) {
    TH2F* hist = new TH2F(name, name, n_x_bins, x_low, x_hi, n_y_bins, y_low, y_hi); // fresh blood!
    hist->GetXaxis()->SetTitle(xtitle);
    hist->GetYaxis()->SetTitle(ytitle);
    return hist;
}

TH3F* init_3Dhist (const TString& name, // histogram name & title
    const int& n_x_bins, const int& x_low, const int& x_hi, // number of bins, min & max x
    const int& n_y_bins, const int& y_low, const int& y_hi, // number of bins, min & may y
    const int& n_z_bins, const int& z_low, const int& z_hi, // number of bins, min & maz z
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

void draw_pretty_two_hists(const TH1F* baseHist, 
    const TH1F* frontHist, 
    const TString title,
    const TString baseTitle,
    const TString frontTitle,
    const TString img_save_location,
    const TString options = "",
    // these defaults will arrange the stats & legend in the top left corner
    // stats box should show: name, mean, RMS & integral (incl errors)
    const int& stats_opt = 1002201, 
    const float& frontX1 = 0.65, const float& frontX2 = 0.9 ,
    const float& frontY1 = 0.75, const float& frontY2 = 0.9 ,
    const float& backX1 = 0.65, const float& backX2 = 0.9 ,
    const float& backY1 = 0.59, const float& backY2 = 0.74,
    const float& legX1 = 0.44, const float& legX2 = 0.64 ,
    const float& legY1 = 0.75, const float& legY2 = 0.9
) {
    TCanvas* can = new TCanvas(title, title);
    baseHist->SetTitle(title);
    baseHist->Draw(options);
    frontHist->SetLineColor(2);
    frontHist->Draw((options + "SAMES"));

    can->Update(); // allows access to the stats boxes (THE FUCK?!)
    edit_stats_box(backHist, stats_opt, backX1, backX2, backY1, backY2);
    edit_stats_box(frontHist, stats_opt, frontX1, frontX2, frontY1, frontY2);
    
        // create a legend for extra info
    TLegend* leg = new TLegend(legX1, legY1, legX2, legY2);
    leg->AddEntry(frontHist, frontTitle);
    leg->AddEntry(baseHist, baseTitle); // add the two histrograms to the legend
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


void create_boxy_legend(const TH1* hist, const TString legend_header, const int n_boxes, 
    const TString const* box_names, const int box_colors[n_boxes],
    const float x1 = 0.15, const float x2 = 0.45, 
    const float y1 = 0.60, const float y2 = 0.90,  
){
    TLegend* leg = new TLegend(x1,y1,x2,y2);
    leg->SetHeader(legend_header);
    leg->SetFillColor(0);
    TBox* boxes[n_boxes] = new TBox*[n_boxes]; //allocate the memory
    for(unsigned int i = 0; i < n_boxes; ++i) {
        boxes[i] = new TBox(0.0, 0.0, 0.0, 0.0);
        boxes[i]->SetFillColor(box_colors[i]);
        leg->AddEntry(boxes[i], box_names[i],"F");
    }
    leg->Draw();
}

void edit_stats_box(const TCanvas* can, const TH2F* hist, const int options=1002201,
    const double x1 = 0.55, const double x2 = 0.70, 
    const double y1 = 0.10, const double y2 = 0.37
){
    can->Update(); // required to get access to the stats box
    TPaveStats* stats_box = (TPaveStats*) hist->FindObject("stats");

    stats_box->SetOptStat(options);
    stats_box->SetX1NDC(x1);
    stats_box->SetX2NDC(x2);
    stats_box->SetY1NDC(y1);
    stats_box->SetY2NDC(y2);
    
}


