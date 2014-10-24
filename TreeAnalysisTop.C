#include "TreeAnalysisTop.h"
#include <fstream>
#include <iostream>
#include <math.h>
#define Z_MASS 91.2

#if !defined(__CINT__)
ClassImp(TreeAnalysisTop);
#endif


TreeAnalysisTop::TreeAnalysisTop(TTree* tree) : PAFAnalysis(tree) {}
//------------------------------------------------------------------------------
// Initialise
//------------------------------------------------------------------------------
void TreeAnalysisTop::Initialise() {
  cout << "Initializing" << endl; 
  GetParameters();

  const double EtaBins[8] = {-2.5,-2.,-1.5,-1.,1.,1.5,2.,2.5};
  const double EtaBins2[5] = {0.,1.,1.5,2.,2.5};
  const double PtBins[3] = {0,100,200};

  fHDummy = CreateH1F("fHDummy","",1,0,1);
  fHDummy->TH1::SetDefaultSumw2();
  hDeltaPtGood = CreateH1F("hDeltaPtGood","",40,-10,10);
  hDeltaPtBad = CreateH1F("hDeltaPtBad","",40,-10,10);
  
  hEta = new TEfficiency("hEta","hEta",7,EtaBins);
  hEtaPt = new TEfficiency("hEtaPt","hEtaPt",4,EtaBins2,2,PtBins);
  fOutput->Add(hEta);
  fOutput->Add(hEtaPt);
  
  for (Int_t el = 0; el<gNEL; el++){
    hPt[el]  = new TEfficiency("hPt"+sElectron[el],"hPt"+sElectron[el],2,PtBins);   
    fOutput->Add(hPt[el]);
  }
  counter = 0;

}

void TreeAnalysisTop::InsideLoop(){
  cout << "Dealing with event " << counter << endl; 
  counter++;
  
  CleanDataMembers();
  fHDummy->Fill(0.5);
  
  Int_t nMuons     = getNTightMuons();
  Int_t nElectrons = getNTightElectrons();
  
  SetLeptons();
  
 
#ifndef __ISGUN
  if (nMuons+nElectrons != 2) return;
  if (nMuons == 0) Channel = ElEl;
  if (nMuons == 1) Channel = ElMu;
  if (nMuons == 2) Channel = MuMu;

  if (Channel != ElEl) return;

  if (fabs(fGetInvMass() - Z_MASS) > 20.) return;
  if (!MatchLeptonsWithMCTruth()) return;
  
#endif

#ifdef __ISGUN
  if (nMuons > 0) return;
  if (nElectrons !=1) return;
#endif 


  CompareCharges();

  for (unsigned int el = 0; el<SelectedLeptons.size(); el++){
    bool ChargeMiss = SelectedLeptons[el].GetChargeMiss();
    Int_t index = SelectedLeptons[el].GetIndex();
    Int_t MC_index = SelectedLeptons[el].GetMCIndex();

    hEta->Fill(ChargeMiss,T_Elec_Eta->at(index));
    if (IsBarrelElectron(index)) hPt[Barrel]->Fill(ChargeMiss,SelectedLeptons[el].GetPt());
    else hPt[Endcap]->Fill(ChargeMiss,SelectedLeptons[el].GetPt());
    hPt[All]->Fill(ChargeMiss,SelectedLeptons[el].GetPt());
    hEtaPt->Fill(ChargeMiss,fabs(T_Elec_Eta->at(index)),SelectedLeptons[el].GetPt());
#ifndef __ISGUN
#ifdef __ISSUMM12
    TLorentzVector GenElec(T_Gen_Elec_Px->at(MC_index),
			   T_Gen_Elec_Py->at(MC_index),
			   T_Gen_Elec_Pz->at(MC_index),
			   T_Gen_Elec_Energy->at(MC_index));
#endif


#ifdef __ISCSA14
    TLorentzVector GenElec(T_Gen_FinalElec_Px->at(MC_index),
			   T_Gen_FinalElec_Py->at(MC_index),
			   T_Gen_FinalElec_Pz->at(MC_index),
			   T_Gen_FinalElec_Energy->at(MC_index));
    
#endif
    Double_t DeltaPt = SelectedLeptons[el].GetPt() - GenElec.Pt();
    if (ChargeMiss) hDeltaPtBad->Fill(DeltaPt);
    else hDeltaPtGood -> Fill(DeltaPt);
#endif
  }
  cout << "Dealt with event" << endl;
  return;
}
//------------------------------------------------------------------------------
// GetParameters
//------------------------------------------------------------------------------
void TreeAnalysisTop::GetParameters()
{
  
  gSampleName   = GetInputParameters()->TheNamedString("sampleName");
  
  GetInputParameters()->TheNamedBool ("IsData",        gIsData);
  GetInputParameters()->TheNamedFloat("weight",        gWeight); // cross section / events in the sample
  GetInputParameters()->TheNamedFloat("LumiForPU",     gLumiForPU);
  GetInputParameters()->TheNamedFloat("TotalLumi",     gTotalLumi);
  GetInputParameters()->TheNamedBool ("DoSystStudies", gDoSystStudies);
  GetInputParameters()->TheNamedBool ("DoFR"         , gDoTLRatios);
  GetInputParameters()->TheNamedBool ("UseCSVM",       gUseCSVM);
  //  GetInputParameters()->TheNamedInt("SystDirection", gSysDirection);
}

//------------------------------------------------------------------------------
// Summary
//------------------------------------------------------------------------------
void TreeAnalysisTop::Summary(){
  
  //  AnalysisTree = ((TTree*) FindOutput("AnalysisTree"));
  //  cout << "---------------------------------------------------" <<endl;
  //  cout << "Number of entries in the Tree= " << AnalysisTree->GetEntries() <<endl;  
  //  cout << "---------------------------------------------------" <<endl;
}

void TreeAnalysisTop::SetDataMembersAtTermination(){
  GetParameters();

}

void TreeAnalysisTop::SetLeptons(){
  for (unsigned int el = 0; el<TightElectrons.size(); el++){

    TLorentzVector Elec(T_Elec_Px->at(TightElectrons[el]),
			T_Elec_Py->at(TightElectrons[el]),
			T_Elec_Pz->at(TightElectrons[el]),
			T_Elec_Energy->at(TightElectrons[el]));
    Int_t charge = T_Elec_Charge->at(TightElectrons[el]);
    Int_t type = 1;
    Int_t index = TightElectrons[el]; 
    lepton lept(Elec,charge,type,index);
    SelectedLeptons.push_back(lept);    
  }

  for (unsigned int mu = 0; mu<TightMuons.size(); mu++){

    TLorentzVector Mu(T_Muon_Px->at(TightMuons[mu]),
		      T_Muon_Py->at(TightMuons[mu]),
		      T_Muon_Pz->at(TightMuons[mu]),
		      T_Muon_Energy->at(TightMuons[mu]));
    Int_t charge = T_Muon_Charge->at(TightMuons[mu]);
    Int_t type = 0;
    Int_t index = TightMuons[mu]; 
    lepton lept(Mu,charge,type,index);
    SelectedLeptons.push_back(lept);    
  }

  
  return;
  
}

Double_t TreeAnalysisTop::fGetInvMass(){
  if (SelectedLeptons.size() != 2){ cout << "ERROR: Number of selected leptons != 2" << endl; return -999;}
  return (SelectedLeptons[0].Get4Momentum()+SelectedLeptons[1].Get4Momentum()).M();

}


bool TreeAnalysisTop::IsGoodMuon(unsigned int iMuon, float ptcut){
  if (T_Muon_Pt->at(iMuon) < ptcut)      return false;
  if (TMath::Abs(T_Muon_Eta->at(iMuon)) > 2.4) return false;
  
  return true;
}
bool TreeAnalysisTop::IsVetoMuon(unsigned int iMuon){
  if (T_Muon_Pt->at(iMuon) < 20)               return false;
  if (TMath::Abs(T_Muon_Eta->at(iMuon)) > 2.4) return false;
  
  if (T_Muon_IsGlobalMuon->at(iMuon) == 0 && T_Muon_IsTrackerMuonArbitrated->at(iMuon) == 0) return false;
  
  float relIso = (T_Muon_chargedHadronIsoR04->at(iMuon) + max(0.0 , T_Muon_neutralHadronIsoR04->at(iMuon) + T_Muon_photonIsoR04->at(iMuon)- 0.5*T_Muon_sumPUPtR04->at(iMuon)))/T_Muon_Pt->at(iMuon);
  if (relIso > 0.20) return false;

  return true;
}
bool TreeAnalysisTop::IsLooseMuon(unsigned int iMuon){
  if (!IsGoodMuon(iMuon)) return false;
  
  // POG Tight Muons definition				       
  if (!T_Muon_IsGlobalMuon->at(iMuon))                       return false;
  if (T_Muon_NormChi2GTrk->at(iMuon) >= 10.)                 return false;
  if (T_Muon_NValidHitsGTrk->at(iMuon) < 1)                  return false;
  //this is still not the exact same def.		       
  if (T_Muon_NumOfMatchedStations->at(iMuon) <= 1)           return false; 
  //							       
  if (TMath::Abs(T_Muon_IPwrtAveBSInTrack->at(iMuon)) >= 0.2)      return false; 
  if (TMath::Abs(T_Muon_vz->at(iMuon) - T_Vertex_z->at(0)) >= 0.5) return false;
  if (T_Muon_NValidPixelHitsInTrk->at(iMuon) == 0)           return false;
  if (T_Muon_NLayers->at(iMuon) <= 5)                        return false;
  
  float relIso = (T_Muon_chargedHadronIsoR04->at(iMuon) + max(0.0 , T_Muon_neutralHadronIsoR04->at(iMuon) + T_Muon_photonIsoR04->at(iMuon)- 0.5*T_Muon_sumPUPtR04->at(iMuon)))/T_Muon_Pt->at(iMuon);
  
  if (relIso > 1.0) return false;
  
  return true;
}
bool TreeAnalysisTop::IsTightMuon(unsigned int iMuon){
  if (!IsLooseMuon(iMuon)) return false;
  
  float relIso = getMuonIso(iMuon);
  
  if (relIso > 0.12) return false;
  
  return true;
}

Int_t TreeAnalysisTop::getNTightMuons(){
  for (unsigned int i = 0; i<T_Muon_Pt->size(); i++){
    if (IsTightMuon(i)) TightMuons.push_back(i);
  }
  return TightMuons.size();
}
float TreeAnalysisTop::getMuonIso(int iMuon){
  if (iMuon < 0) return 9999.;
  if (iMuon >= (int)T_Muon_chargedHadronIsoR04->size()) return 9999.;

  return (T_Muon_chargedHadronIsoR04->at(iMuon) + max(0.0 , T_Muon_neutralHadronIsoR04->at(iMuon) + T_Muon_photonIsoR04->at(iMuon)- 0.5*T_Muon_sumPUPtR04->at(iMuon)))/T_Muon_Pt->at(iMuon);
}
//------------------------------------------------------------------------------
// Electron Selectors
//------------------------------------------------------------------------------
//SANTIint TreeAnalysisTop::GetSelectedElecInd()
//SANTI{
//SANTI  if (nSelElec > 0) {
//SANTI    cout << "[ERROR] GetSelectedElec(): This function has already been called" << endl;
//SANTI    cout << "[ERROR] GetSelectedElec(): Cleaning previous information        " << endl;
//SANTI    S_Elec.clear();
//SANTI  }
//SANTI  
//SANTI  // Loop over all electrons and keep loose electrons
//SANTI  for (UInt_t i=0; i<T_Elec_Energy->size(); i++) {
//SANTI    if (!IsLooseElectron(i)) continue;
//SANTI    S_Elec.push_back(i);
//SANTI  }
//SANTI  
//SANTI  return S_Elec.size();
//SANTI}
int  TreeAnalysisTop::getNTightElectrons(){
 
  for (UInt_t i=0; i<T_Elec_Energy->size(); i++) {
    if (IsTightElectron(i)) TightElectrons.push_back(i);
  }
  return TightElectrons.size();
}

bool TreeAnalysisTop::IsTightElectron(Int_t iElec){
  
  TLorentzVector Elec(T_Elec_Px->at(iElec),T_Elec_Py->at(iElec),T_Elec_Pz->at(iElec),T_Elec_Energy->at(iElec));
  if (!IsGoodElectron(iElec)) return false;
  if (Elec.Pt() < 10.) return false;
  if (fabs(Elec.Eta()) > 2.5) return false;
  if (T_Elec_nLost->at(iElec) > 0) return false;
  
  return true;
}

bool TreeAnalysisTop::IsGoodElectron(unsigned int iElec){
  TLorentzVector Elec(T_Elec_Px->at(iElec),
		      T_Elec_Py->at(iElec),
		      T_Elec_Pz->at(iElec),
		      T_Elec_Energy->at(iElec));

  if (!ElPassesTrigger(iElec)) return false;
  

  if (fabs(T_Elec_SC_Eta->at(iElec)) < 1.442){
    
    if (fabs(T_Elec_deltaEtaIn->at(iElec)) > 0.004) return false;   
    if (fabs(T_Elec_deltaPhiIn->at(iElec)) > 0.06)  return false;
    if (T_Elec_sigmaIetaIeta->at(iElec) > 0.01) return false; 
    if (T_Elec_HtoE->at(iElec) > 0.12) return false;
    if (fabs(T_Elec_IPwrtPV->at(iElec)) > 0.02) return false;
#ifdef __ISCSA14
    if (T_Elec_dzwrtPV->at(iElec) > 0.1) return false;
#endif
#ifdef __ISSUMM12
    if (fabs(T_Elec_vz->at(iElec)-T_Vertex_z->at(0)) > .1) return false;
#endif
    if (fabs(1/Elec.E() - 1/Elec.P()) > 0.05)  return false;
    if (getElecIso(iElec) > 0.15)  return false;
    if (T_Elec_passConversionVeto->at(iElec) != 1) return false;
    if (T_Elec_nLost->at(iElec) > 1) return false;
     return true;
  }

  else if (fabs(T_Elec_SC_Eta->at(iElec)) >= 1.566 && fabs(T_Elec_SC_Eta->at(iElec)) < 2.5){
    if (fabs(T_Elec_deltaEtaIn->at(iElec)) > 0.007) return false;
    if (fabs(T_Elec_deltaPhiIn->at(iElec)) > 0.03) return false;
    if (T_Elec_sigmaIetaIeta->at(iElec) > 0.03) return false;
    if (T_Elec_HtoE->at(iElec) > 0.1) return false;
    if (fabs(T_Elec_IPwrtPV->at(iElec)) > 0.02) return false;
#ifdef __ISCSA14
    if (T_Elec_dzwrtPV->at(iElec) > 0.1) return false;
#endif
#ifdef __ISSUMM12
    if (fabs(T_Elec_vz->at(iElec)-T_Vertex_z->at(0)) > .1) return false;
#endif
    if (fabs(1/Elec.E() - 1/Elec.P()) > 0.05) return false; 
    if (getElecIso(iElec) > 0.1) return false;
    if (T_Elec_passConversionVeto->at(iElec) != 1) return false;
    if (T_Elec_nLost->at(iElec) > 1) return false;
    return true;
  }


  else return false;

}
float TreeAnalysisTop::getElecIso(int iElec){
  if (iElec < 0) return 9999.;
  if (iElec >= (int)T_Elec_chargedHadronIso->size()) return 9999.;

  float relIso = (T_Elec_chargedHadronIso->at(iElec) + 
		  max((double)0.0, 
		      T_Elec_neutralHadronIso->at(iElec) + 
		      T_Elec_photonIso->at(iElec) - 
		      0.5*T_Elec_puChargedHadronIso->at(iElec)
		      )
		  )/T_Elec_Pt->at(iElec);

  return relIso;
}

bool TreeAnalysisTop::ElPassesTrigger(Int_t iElec){
  TLorentzVector Elec(T_Elec_Px->at(iElec),T_Elec_Py->at(iElec),T_Elec_Pz->at(iElec),T_Elec_Energy->at(iElec));
  if (T_Elec_dr03EcalSumEt->at(iElec)/Elec.Pt() > 0.2) return false;
  if (T_Elec_dr03HcalSumEt->at(iElec)/Elec.Pt() > 0.2) return false;
  if (T_Elec_dr03TkSumPt->at(iElec)/Elec.Pt() > 0.2) return false;
  
  return true;
}


void TreeAnalysisTop::CleanDataMembers(){
  TightElectrons.clear();
  TightMuons.clear();
  SelectedLeptons.clear();
  MisMeasuredEl.clear();
  return;
}

bool TreeAnalysisTop::IsBarrelElectron(Int_t iElec){
  if (fabs(T_Elec_SC_Eta->at(iElec)) < 1.479) return true;
  return false;
}

Int_t TreeAnalysisTop::GetNBarrelElectrons(){
   Int_t N = 0;
   for (unsigned int lep = 0; lep<SelectedLeptons.size(); lep++){
     Int_t ind = SelectedLeptons[lep].GetIndex();
     if (IsBarrelElectron(ind)) N++;
   }
   return N;
 }

bool TreeAnalysisTop::MatchLeptonsWithMCTruth(){
  for (unsigned int sel = 0; sel<SelectedLeptons.size(); sel++){
    bool Matched = false;
    TLorentzVector V1 = SelectedLeptons[sel].Get4Momentum();
#ifdef __ISCSA14
    for (unsigned int MCel = 0; MCel<T_Gen_FinalElec_pdgId->size(); MCel++){
      TLorentzVector V2(T_Gen_FinalElec_Px->at(MCel),
			T_Gen_FinalElec_Py->at(MCel),
			T_Gen_FinalElec_Pz->at(MCel),
			T_Gen_FinalElec_Energy->at(MCel));
      if (V1.DeltaR(V2) < 0.3){
	Matched = true;
	SelectedLeptons[sel].SetMCindex(MCel);
	continue;
      }
    }
#endif

#ifdef __ISSUMM12
    for (unsigned int MCel = 0; MCel<T_Gen_Elec_MPz->size(); MCel++){
    
      TLorentzVector V2(T_Gen_Elec_Px->at(MCel),
			T_Gen_Elec_Py->at(MCel),
			T_Gen_Elec_Pz->at(MCel),
			T_Gen_Elec_Energy->at(MCel));
      if (V1.DeltaR(V2) < 0.3){
	Matched = true;
	SelectedLeptons[sel].SetMCindex(MCel);
	continue;
      }
    }
#endif
    if (!Matched) return false;
  }
  return true;
}

void TreeAnalysisTop::CompareCharges(){
#ifndef __ISGUN
  for (unsigned int el = 0; el<SelectedLeptons.size(); el++){
    Int_t MC_index = SelectedLeptons[el].GetMCIndex();
#ifdef __ISCSA14
    if (T_Gen_FinalElec_pdgId->at(MC_index)*SelectedLeptons[el].GetCharge() < 0) SelectedLeptons[el].SetChargeMiss(false); //FinalElec_pdfId > 0 if e^- //false means well measured
    else SelectedLeptons[el].SetChargeMiss(true);
#endif 

#ifdef __ISSUMM12
    if (T_Gen_Elec_PID->at(MC_index)*SelectedLeptons[el].GetCharge() < 0) SelectedLeptons[el].SetChargeMiss(false);
    else SelectedLeptons[el].SetChargeMiss(true);
#endif
  }

#endif

#ifdef __ISGUN
  if (SelectedLeptons[0].GetCharge() > 0) SelectedLeptons[0].SetChargeMiss(true); //detecting e^+ means bad measurement
  else SelectedLeptons[0].SetChargeMiss(false); 					  
#endif
}
