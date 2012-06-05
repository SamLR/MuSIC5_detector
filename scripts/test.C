// 
// 
// void func(TH1* h, int i){
//     if (i==1){
//         h->Fill(1,60);
//     } else if(i==2) {
//         h->Fill(1,50,60);
//     } else if(i==3){        
//         h->Fill(40, 90,70);
//     }
// }
// 
// void test(){
// 
//     TH1F* h1 = new TH1F("1D", "1D", 100, 0, 100);
//     TH2F* h2 = new TH2F("2D", "2D", 100, 0, 100, 100, 0, 100);
//     TH3F* h3 = new TH3F("3D", "3D", 100, 0, 100, 100, 0, 100, 100, 0, 100);
//     
//     func(h1,1);
//     func(h2,2);
//     func(h3,3);
//     
//     TCanvas* c1 = new TCanvas("c1","c1");
//     h1->Draw();
//     TCanvas* c2 = new TCanvas("c2","c2");
//     h2->Draw();
//     TCanvas* c3 = new TCanvas("c3","c3");
//     h3->Draw();
// }

// int func(int&& n){
//     cout << "rvalue "<< n << endl;
//     return n;
// }
// 
// int func(int& n){
//     cout << "lvalue "<< n << endl;
//     return n;
// }
// 
// void test(){
//     int n = 1;
//     func(n);
//     func(func(9));
// }