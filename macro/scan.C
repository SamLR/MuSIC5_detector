// Usage:
// root[0] .L scan.C
// root[1] scan("path/to/rootfile",10); // to see 10th event.
//
char* counter_to_name(int id)
{
    if (id==1) return "sci1";
    if (id==2) return "target";
    if (id==3) return "sci2";
}
char* pdgid_to_name(int id)
{
    if (id==11)        { return "e-"; }
    else if (id==12)   { return "nu_e"; }
    else if (id==13)   { return "mu-"; }
    else if (id==14)   { return "nu_mu"; }

    else if (id==-11)  { return "e+"; }
    else if (id==-12)  { return "nu_e_bar"; }
    else if (id==-13)  { return "mu+"; }
    else if (id==-14)  { return "nu_mu_bar"; }
 
    else if (id==22)   { return "gamma"; }
    else if (id==111)  { return "pi0"; }
    else if (id==211)  { return "pi+"; }
    else if (id==-211) { return "pi-"; }
    else if (id==2212) { return "p"; }
    else if (id==2112) { return "n"; }
    else { return "others"; }
}
char* procid_to_name(int id)
{
    if (id==1 )  return "msc";
    if (id==2 )  return "Transportation";
    if (id==3 )  return "eIoni";
    if (id==4 )  return "eBrem";
    if (id==5 )  return "CoulombScat";
    if (id==6 )  return "phot";
    if (id==7 )  return "compt";
    if (id==8 )  return "muMsc";
    if (id==9 )  return "muIoni";
    if (id==10)  return "Decay";
    if (id==11)  return "hIoni";
    if (id==12)  return "annihil";
    if (id==13)  return "conv";
    if (id==14)  return "ionIoni";
    if (id==15)  return "muMinusCaptureAtRest";
    if (id==16)  return "hadElastic";
    if (id==17)  return "PionPlusInelastic";
    if (id==18)  return "NeutronInelastic";
    if (id==19)  return "CHIPSNuclearCaptureAtRest";
    if (id==20)  return "PionMinusInelastic";

}
void scan(char* root, int iev)
{
    TFile *f = new TFile(root);
    TTree *t = f->Get("t");

    int g_iev;
    int g_iev_g4bl;
    int g_nhit;
#define MAX_HIT 100
    int procid[MAX_HIT];
    int counter[MAX_HIT];
    int trkid[MAX_HIT];
    int parentid[MAX_HIT];
    int pdgid[MAX_HIT];
    double x[MAX_HIT];
    double y[MAX_HIT];
    double z[MAX_HIT];
    double px[MAX_HIT];
    double py[MAX_HIT];
    double pz[MAX_HIT];
    double edep[MAX_HIT];
    double tof[MAX_HIT];
    t->SetBranchAddress("iev",&g_iev);
    t->SetBranchAddress("iev_g4bl",&g_iev_g4bl);
    t->SetBranchAddress("nhit",&g_nhit);
    t->SetBranchAddress("procid",procid);
    t->SetBranchAddress("counter",counter);
    t->SetBranchAddress("trkid",trkid);
    t->SetBranchAddress("parentid",parentid);
    t->SetBranchAddress("pdgid",pdgid);
    t->SetBranchAddress("x",x);
    t->SetBranchAddress("y",y);
    t->SetBranchAddress("z",z);
    t->SetBranchAddress("px",px);
    t->SetBranchAddress("py",py);
    t->SetBranchAddress("pz",pz);
    t->SetBranchAddress("edep",edep);
    t->SetBranchAddress("tof",tof);


    int total = t->GetEntries();
    for (int i=0; i<total; i++) {
        t->GetEntry(i);

        if (iev==g_iev || iev==0) {
            printf("%10s %12s %7s %25s %8s %16s %9s %9s %9s %9s %9s %9s %9s %9s\n",
                    "g_iev",
                    "g_iev_g4bl",
                    "g_nhit",
                    "particle[pid](trkid-prtid)",
                    "counter",
                    "process",
                    "x(mm)",
                    "y(mm)",
                    "z(mm)",
                    "Px(MeV/c)",
                    "Py(MeV/c)",
                    "Pz(MeV/c)",
                    "edep(MeV)",
                    "time(ns)");
            for (int j=0; j<g_nhit; j++) {
                printf("%10d %12d %7d %10s[%4d] (%3d<-%3d) %8s %16s %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf\n",
                        g_iev,
                        g_iev_g4bl,
                        g_nhit,
                        pdgid_to_name(pdgid[j]),
                        pdgid[j],
                        trkid[j],
                        parentid[j],
                        counter_to_name(counter[j]),
                        procid_to_name(procid[j]),
                        x[j],
                        y[j],
                        z[j],
                        px[j],
                        py[j],
                        pz[j],
                        edep[j],
                        tof[j]);
            }
            if (iev>0) break;
        }
    }
    delete f;
}
