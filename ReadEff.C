{

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


  TEfficiency* hEta;
  TEfficiency* hEtaPt;
  TEfficiency* hPt[gNEL];

  TFile* file = TFile::Open("results/DY_CSA14.root");
  file->GetObject("hEta",hEta);
  file->GetObject("hEtaPt",hEtaPt);
  for (Int_t k = 0; k<gNEL; k++){
    file->GetObject("hPt"+sElectron[k],hPt[k]);
  }

  TCanvas* c1 = new TCanvas();
  hEta->Draw();
  
  TCanvas* c2 = new TCanvas();
  hEtaPt->Draw("colz TEXT e");

  TCanvas* c3 = new TCanvas();
  TLegend* leyenda = new TLegend(0.145115,0.681435,0.316092,0.850211,"");
  for (Int_t k = 0; k<gNEL; k++){
    leyenda->AddEntry(hPt[k],sElectron[k]);
    hPt[k]->SetLineColor(Color[k]);
    if (k==0){
      hPt[k]->SetTitle(";P_{T}[GeV];P");
      hPt[k]->Draw();
    }
    else hPt[k]->Draw("same");
  }
  leyenda->Draw("same");
  
}
