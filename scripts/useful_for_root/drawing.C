void edit_stats_box(const TCanvas* can, const TH1* hist, const int options=1002201,
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
    can->Update();
}

void draw_pretty_two_hists(const TH1* backHist, 
    const TH1* frontHist, 
    const TString title,
    const TString backTitle,
    const TString frontTitle,
    // if no location is given nothing is saved
    const TString img_save_location = "",
    const TString options = "",
    // these defaults will arrange the stats & legend in the top left corner
    // stats box should show: name, mean, RMS & integral (incl errors)
    // const int& stats_opt = 1002201, 
    const int stats_opt = 1, 
    const double frontX1 = 0.65, const double frontX2 = 0.9 ,
    const double frontY1 = 0.75, const double frontY2 = 0.9 ,
    const double backX1 = 0.65,  const double backX2 = 0.9 ,
    const double backY1 = 0.59,  const double backY2 = 0.74,
    const double legX1 = 0.44,   const double legX2 = 0.64 ,
    const double legY1 = 0.75,   const double legY2 = 0.9
) {
    cout << title << endl;
    TCanvas* can = new TCanvas(title, title);
    backHist->SetTitle(title);
    backHist->Draw(options);
    // can->Update();
    edit_stats_box(can, backHist,  stats_opt, backX1,  backX2,  backY1,  backY2);
    
    frontHist->SetLineColor(2);
    frontHist->Draw((options + "SAMES"));
    // can->Update();
    edit_stats_box(can, frontHist, stats_opt, frontX1, frontX2, frontY1, frontY2);
    
        // create a legend for extra info
    TLegend* leg = new TLegend(legX1, legY1, legX2, legY2);
    leg->AddEntry(frontHist, frontTitle);
    leg->AddEntry(backHist, backTitle); // add the two histrograms to the legend
    leg->SetFillColor(0);
    leg->Draw();

    if(img_save_location!="") {
        cout << "Saving image to " << img_save_location << endl;
        can->SaveAs(img_save_location);
    }
}

// the stupidly long variable name is to try and stop namespace collisions @ global
const int __useful_default_colour_selection [] =  {1, 2, 4, 3, 6, 7, 8, 9};
void draw_pretty_hists(const int& n_hists,
    const TH1** hist_array, 
    const TString title,
    const TString img_save_location ="",
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

    if (img_save_location != "") can->SaveAs(img_save_location);
}

void draw_pretty_hist(const TH1* hist, 
    const TString title,
    const TString img_save_location ="",
    const TString options = ""
) {
    TCanvas* can = new TCanvas(title, title);
    hist->SetTitle(title);
    hist->Draw(options);
    if (img_save_location != "" )can->SaveAs(img_save_location);
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


