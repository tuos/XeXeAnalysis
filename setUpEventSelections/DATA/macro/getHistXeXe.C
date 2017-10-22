#include <iomanip>
void getHistXeXe(){

  // Setup
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptFit(1011);

  // Open File
  TFile *infile = new TFile("./flowXeXe_setupTest.root");

  const int nCent = 9;
  TH1D *hNtrk[nCent];
for(int i=0; i<nCent; i++){
  hNtrk[i] = (TH1D*)infile->Get(Form("demo/htrackdistribution_icent%d",i));
}

  TCanvas *can=new TCanvas("can","can",10,10,980,780);
  can->Divide(3,3);
  gPad->SetLogy();
  //gStyle->SetOptStat(0);
  for(int i=0; i<nCent; i++){
    can->cd(i+1);
    hNtrk[i]->Draw();
  } 
  can->SaveAs("plot_ntrkCent_dist.png");

  TH1D* vzHist;
  TH1D* hCentralityBin;
  TH1D* hHFCentralityObject; 
  TH1D* hTrackCentralityObject; 
  TH1D* generalTracksHist;
  TH2D* hetaVSphi;  
  vzHist = (TH1D*)infile->Get(Form("demo/vzhist"));
  hCentralityBin = (TH1D*)infile->Get(Form("demo/hcentralitybin"));
  hHFCentralityObject = (TH1D*)infile->Get(Form("demo/hhfcentralityobject"));
  hTrackCentralityObject = (TH1D*)infile->Get(Form("demo/htrackcentralityobject"));
  generalTracksHist = (TH1D*)infile->Get(Form("demo/generaltrackshist"));
  hetaVSphi = (TH2D*)infile->Get(Form("demo/hetavsphi"));

  TCanvas *can2=new TCanvas("can2","can2",10,10,880,580);
  can2->Divide(3,2);
  can2->cd(1);
  vzHist->Draw();
  can2->cd(2);
  hCentralityBin->Draw();
  can2->cd(3);
  gPad->SetLogy();
  hHFCentralityObject->Draw();
  can2->cd(4);
  gPad->SetLogy();
  hTrackCentralityObject->Draw();
  can2->cd(5);
  gPad->SetLogy();
  generalTracksHist->Draw();
  can2->cd(6);
  hetaVSphi->Draw("colz");
  can2->SaveAs("plot_diff_dist.png");


}


