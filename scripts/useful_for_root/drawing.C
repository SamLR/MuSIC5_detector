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
    const TString* leg_entries,
    const TString img_save_location ="",
    const int stats_opt=0, // 1002201 <- name, mean±er, rms±er & integral
    const TString options = "",
    const int colours [n_hists] = __useful_default_colour_selection
) {
    TCanvas* can = new TCanvas(title, title,1436,856); // these magic numbers make a maximised plot 
    const float x1 = (stats_opt) ? 0.60:0.70;
    const float x2 = (stats_opt) ? 0.80:0.90;
    const float y1 = 0.9 - 0.05*n_hists; 
    const float y2 = 0.90;

    if (stats_opt) can->SetRightMargin((1.0 - x2));
    TLegend* leg = new TLegend(x1, y1, x2, y2);

    for(unsigned int hist = 0; hist < n_hists; ++hist) {
        hist_array[hist]->SetLineWidth(2);// otherwise the lines are tiny
        hist_array[hist]->SetLineColor(colours[hist]); // mmmm colour!

        // specifically set the legend to use the current title        
        leg->AddEntry(hist_array[hist], leg_entries[hist]); 
        
        if (stats_opt){
            // if it's the first hist you just Draw(), otherwise Draw("sameS");
            // need the extra 's' at the end and as part of the word, indicates
            // to draw the stats boxes as well
            TString draw_opt = (hist==0) ? options : ("sameS"+options);
            hist_array[hist]->Draw(draw_opt);
            
            // position the stats boxes so that they're all visible
            // leave a 0.01 gap between the stats boxes
            // the +0.1 offset aligns with x-axis whilst 0.81 aligns with 
            // top of draw frame whilst maintaining a 0.01 gap
            const float y1_stat = (0.81/n_hists)*hist + 0.1;
            const float y2_stat = (0.81/n_hists)*(hist+1) + 0.09; 
            const float x1_stat = x2+0.01; 
            const float x2_stat = 0.99; 
            // tidy up the stats box
            edit_stats_box(can, hist_array[hist], stats_opt, x1_stat, x2_stat, y1_stat, y2_stat);
        } else {
            // if it's the first hist you just Draw(), otherwise Draw("SAME");
            TString draw_opt = (hist==0) ? options : ("SAME"+options);
            // make sure no stats boxes are drawn
            if (hist==0) hist_array[hist]->SetStats(false);
            hist_array[hist]->Draw(draw_opt);
        }
    }
    // hack to set the title of the histogram (yet have a different entry for 
    // the first hist in the legend, if desired)
    hist_array[0]->SetTitle(title);
    leg->SetFillColor(0); // don't want grey backgrounds
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
    const float y1 = 0.60, const float y2 = 0.90  
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


