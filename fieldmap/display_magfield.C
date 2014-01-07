#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TMath.h"

struct bfield2d{
    float x;
    float z;
    float bx;
    float bz;  
};
float get_mod(const float x, const float y, const float z);

void display_magfield(){
    FILE *pFile = fopen("fieldmap_solenoid.txt", "r");
    
    TFile *outFile = new TFile("magfield.root", "RECREATE");
    TTree *outTree = new TTree("bfield", "bfield");
    
    bfield2d myfield;
    outFile->cd();
    outTree->Branch("field", &myfield, "x/F:z:bx:bz");
    TH2F *bhist = new TH2F("Bfield", "Bfield", 25, 750,  1250,
                                               25, 3500, 4000);
    
    float y, by, junk;
    
    char dat_format [] = "%f %f %f %f %f %f";
    char line [150];
    int linecount = 0;
    int datacount = 0;
    while (fgets(line, 150, pFile) != NULL) {
        ++linecount;
        sscanf(line, dat_format, &myfield.x, &y, 
            &myfield.z, &myfield.bx, &by, &myfield.bz);
        
        if (linecount%500==0) {printf("%d done\n", linecount);}
        if (y > 0.1) {continue;} // take only the y = 0 slice
        ++datacount;
        float mod = get_mod(myfield.bx, by, myfield.bz);
        outTree->Fill();
        bhist->Fill(myfield.x, myfield.z, mod);
    }
    // bhist->Draw("SURF2");
    bhist->SetStats(false);
    bhist->Draw("COLZ");
    printf("%d\n", datacount);
    
    outFile->Write();
}

float get_mod(const float x, const float y, const float z) {
  return TMath::Sqrt(x*x + y*y + z*z);
}