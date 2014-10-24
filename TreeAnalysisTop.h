#ifndef TREEANALYSIS_TOP_H
#define TREEANALYSIS_TOP_H 1

#include "PAFAnalysis.h"
#include "TCounterUI.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TTree.h"
#include "TString.h"
#include "TRandom3.h"

#include <fstream>
#include <iostream>
#include <vector>

enum eChannel{
  ElEl,
  ElMu,
  MuMu,
  gNCHAN
};

const TString sChannel[gNCHAN] =  {
  "ElEl",
  "ElMu",
  "MuMu"
};

enum eElectron{
  All,
  Barrel,
  Endcap,
  gNEL
};

const TString sElectron[gNEL] = {
  "All",
  "Barrel",
  "Endcap"
};


class lepton {
 public:
  lepton(){};
  lepton(TLorentzVector vec, int ch, int ty, int ind){
    p = vec;
    charge = ch;
    type = ty;
    index = ind;
  };
  TLorentzVector p;
  int charge;
  int type; // -1(unknown), 0(mu), 1(ele)                                             
  int index;
  int MCindex; // index in the collection  T_Gen_FinalElec_*
  bool ChargeMiss; // false: no charge miss
  
  //Get methods
  TLorentzVector Get4Momentum(){ return p;}
  Int_t GetCharge(){return charge;}
  Double_t GetPt() { return p.Pt();}
  Double_t GetEta() { return p.Eta();}
  Int_t GetIndex() {return index;}
  bool GetChargeMiss(){ return ChargeMiss;}
  Int_t GetMCIndex(){ return MCindex;}
  
  //Set methods
  void SetMCindex(int i) { MCindex = i; return;} 
  void SetChargeMiss(bool miss){ ChargeMiss = miss; return;} 
};


class TreeAnalysisTop: public PAFAnalysis {
 public:
  TreeAnalysisTop(TTree *tree=0);
  virtual ~TreeAnalysisTop() {};
  
  virtual void Initialise();
  virtual void InsideLoop();
  virtual void GetParameters();
  virtual void SetDataMembersAtTermination();
  virtual void Summary();

  
  //Analysis functions
  virtual void SetLeptons();
  virtual Double_t fGetInvMass();
  virtual void CleanDataMembers();
  virtual bool MatchLeptonsWithMCTruth();
  virtual void CompareCharges();
 private:
 
  ////////////////
  // Histograms //
  ////////////////
  TH1F* fHDummy;
  TH1F* hDeltaPtGood;
  TH1F* hDeltaPtBad;
  TEfficiency* hPt[gNEL];
  TEfficiency* hEta;
  TEfficiency* hEtaPt;
  

  Int_t counter;
  eChannel Channel;
  eElectron Electrons;

  /////////////////////////////////////
  // Datamembers that must be cleaned//
  /////////////////////////////////////
  std::vector<int> TightElectrons;
  std::vector<int> TightMuons;
  std::vector<lepton> SelectedLeptons;
  std::vector<int> MisMeasuredEl;
  ////////////////////////
  /// Input parameters ///
  ////////////////////////
  TString gSampleName;
  TString gfileSuffix;
  Float_t gWeight;
  Float_t gLumiForPU;
  Float_t gTotalLumi;
  Int_t   gSysSource;
  Int_t   gSysDirection;
  Bool_t  gDoSystStudies;
  Bool_t  gDoTLRatios;
  Bool_t  gIsData;
  Bool_t  gUseCSVM;

  //////////////////////
  // Counting methods //
  //////////////////////

  Int_t getNTightElectrons();
  Int_t getNTightMuons();


  //////////////////////////////
  // Lepton selection methods //
  //////////////////////////////
  bool IsGoodMuon(unsigned int,float ptcut= 10.);
  bool IsVetoMuon(unsigned int);
  bool IsLooseMuon(unsigned int);
  bool IsTightMuon(unsigned int);
  float getMuonIso(int);
  bool IsGoodElectron(unsigned int);
  bool IsLooseElectron(unsigned int);  
  bool IsTightElectron(Int_t);
  bool ElPassesTrigger(Int_t);
  float getElecIso(int);
  float getEACorrection(float);
  bool IsBarrelElectron(Int_t);
  Int_t GetNBarrelElectrons();
  std::vector<lepton> SortLeptonsByPt(std::vector<lepton>&);

  ClassDef(TreeAnalysisTop,0);
};
#endif

