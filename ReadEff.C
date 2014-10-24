#include<iostream>
#include"TEfficiency.h"
#include"TH1.h"
#include"TH2.h"
#include"TF1.h"
#include"TF2.h"
#include"TFile.h"
#include"TString.h"
#include"TLegend.h"
#include"TCanvas.h"

enum eElectron{
  All,
  Barrel,
  Endcap,
  gNEL
};

Int_t Color[gNEL] = {
  kRed,
  kBlue,
  kGreen
};

const TString sElectron[gNEL] = {
  "All",
  "Barrel",
  "Endcap"
};

void ReadEff(){

  TEfficiency* hEta;
  TEfficiency* hEtaPt;
  TEfficiency* hPt[gNEL];

  TFile* file = TFile::Open("../results/DY_CSA14.root");
  file->GetObject("hEta",hEta);
  file->GetObject("hEtaPt",hEtaPt);
  for (Int_t k = 0; k<gNEL; k++){
    file->GetObject("hPt"+sElectron[k],hPt[k]);
  }


  TCanvas* c1 = new TCanvas();
  TF1* fun2 = new TF1("fun2","[0]*x*x+[1]",-2.5,2.5);
  hEta->SetTitle(";#eta;P");
  hEta->Draw();
  hEta->Fit(fun2);

  TCanvas* c2 = new TCanvas();
  //c2-&gt;Divide(2,1);
  TF2* fun = new TF2("fun","-[0]*x*x*x*x/(x-5)+[1]*y*x*x*x*x+[2]",0,2.5,0,200);


  
  TH1* passed = hEtaPt->GetCopyPassedHisto();
  TH1* total = hEtaPt->GetCopyTotalHisto();
  passed->Divide(total);
  //c2-&gt;cd(1);
  passed->Draw("surf2");
  // c2-&gt;cd(2);

  // passed-&gt;Fit(fun,"R0");
  passed -> GetXaxis() ->SetTitle("#eta");
  passed -> GetYaxis() ->SetTitle("p_{T} [GeV]");
  passed -> GetZaxis() ->SetTitle("P");

  //  fun-&gt;Draw("surf2");


  TCanvas* c3 = new TCanvas();

  TF1* fun3[3];
  for (Int_t k = 0; k<3; k++){
    TString num;
    num.Form("%d",k);
    fun3[k] = new TF1("fun"+num,"[1]*x+[0]",0,200);
  }

  TLegend* leyenda = new TLegend(0.145115,0.681435,0.316092,0.850211,"");
  for (Int_t k = 2; k > -1; k--){
    leyenda->AddEntry(hPt[k],sElectron[k]);
    hPt[k]->SetLineColor(Color[k]);
    fun3[k]->SetLineColor(Color[k]);
    fun3[k]->SetParName(0,"a_{0}");
    fun3[k]->SetParName(1,"a_{1}");
    hPt[k]->Fit(fun3[k],"0");

    if (k==2){
      hPt[k]->SetTitle(";P_{T}[GeV];P");
      //  hPt[k] -&gt; SetStats(0);
      hPt[k]->Draw();
    }
    else hPt[k]->Draw("same");
  }
  leyenda->Draw("same");
 
 
}

