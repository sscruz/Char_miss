////////////////////////////////////////////////////////////////////////////////
//
//    FILE: RunProof.C
// AUHORS: I. Gonzalez Caballero, A.Y. Rodriguez Marrero
//    DATE: 2010
//
// CONTENT: Main macro to run over MiniTrees or TESCO Trees using PROOF
//          in PROOF-Lite, PROOF-Cluster or Sequential mode
//
////////////////////////////////////////////////////////////////////////////////

TProof* proof = 0;

void RunTree_ReReco(TString  sampleName     = "DY_CSA14",
		    Int_t    nSlots         =  20,
		    Bool_t   DoSystStudies  =  false,
		    Bool_t   DoFR           =  false,
		    Long64_t nEvents        = -1,
		    TString  suffix         = ""
		    ){
  
  gROOT->LoadMacro("$PAFPATH/PAF.C");
  
  // VARIABLES TO BE USED AS PARAMETERS...
  Float_t G_Total_Lumi    = 19664.225;   // Total Luminosity
  Float_t G_Event_Weight  = 1.0;         // Event Weight
  Bool_t  G_IsData        = false;       // 1 for data, 0 for MC
  Float_t G_LumiForPUData = 19468.3;     // luminosity in http://www.hep.uniovi.es/jfernan/PUhistos

  Bool_t G_IsCSA14 = true;
  Bool_t G_IsMCSumm = false;
  Bool_t G_IsElGun = false;
  
  cout << "Params: " << endl;
  cout << "sampleName      " << sampleName      << endl;
  cout << "lumiForNorm     " << G_Total_Lumi    << endl;
  cout << "EventWeight     " << G_Event_Weight  << endl;
  cout << "lumiForPUdata   " << G_LumiForPUData << endl;
  cout << "DoSystStudies   " << DoSystStudies   << endl;
  cout << "DoFR            " << DoFR            << endl;
  cout << "nEvents         " << nEvents         << endl;
  
  // PROOF settings - see scripts/PAFOptions.h
  //----------------------------------------------------------------------------

  // PROOF mode
  //----------------------------------------------------------------------------
  if      (nSlots == 1) gPAFOptions->SetPAFMode(kSequential); // NO PROOF
  else if (nSlots < 8 ) gPAFOptions->SetPAFMode(kLite);       // PROOF Lite
  else                  gPAFOptions->SetPAFMode(kPoD);        // PoD
  
  gPAFOptions->SetNSlots(nSlots);

  //SANTI 
  //  gPAFOptions->proofMode = kSequential;       // No PROOF
  //  gPAFOptions->proofMode = kLite;             // PROOF Lite
  //  gPAFOptions->proofMode = kPoD;              // PoD

  //gPAFOptions->proofMode = kCluster;            // PROOF Cluster
  //  gPAFOptions->NSlots = 50;                   // Number of slots

  // Optional parameters for PROOF Cluster mode
  // gPAFOptions->proofServer = "proof.ifca.es";  // PROOF server
  // gPAFOptions->proofServerPort = 1093;         // PROOF port
  // gPAFOptions->maxSlavesPerNode = 9999;        // Max number of slaves / node
  
  
  // PROOF start
  //----------------------------------------------------------------------------
 cout << ">> Starting PROOF..." << endl;
 proof = InitProof();
 if (!proof && gPAFOptions->GetPAFMode() != kSequential) {
   cerr << "ERROR: I could not initialise a PROOF session!" << endl;
   return;
 }
 cout << "DEBUG: proof = " << proof << endl;
 if (gPAFOptions->GetPAFMode() != kSequential) 
   gPAFOptions->GetPROOFSession()->SetLogLevel(2, TProofDebug::kOutput); 
  
  // Tree type
  //----------------------------------------------------------------------------
  gPAFOptions->SetTreeType(kMiniTrees);

  // Base path to input files
  //----------------------------------------------------------------------------
 
  
  ///////////////////////////////
  // INPUT DATA SAMPLE
  ///////////////////////////////
  TString userhome = "/mnt_pool/fanae105/user/sscruz/";
  gROOT->LoadMacro(userhome+"/DatasetManager/DatasetManager.C+");
  
  TString dataPath = "/pool/ciencias/TreesCSA14/";
  std::vector<TString> datafiles;
   
  if (sampleName == "DY_CSA14"){
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_0.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_1.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_2.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_3.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_4.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_5.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_6.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_7.root"));
    datafiles.push_back(dataPath+Form("Tree_DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2_Skim2LPt17_8GeV_PU20bx25_8.root"));
    
    G_Event_Weight = 6025.2 / 997342;
    G_IsData = false;    
    G_IsCSA14 = true;
    G_IsMCSumm = false;
    gPAFOptions->AddDataFiles(datafiles); 
  }

  else if (sampleName == "ElGun_CSA14"){
    datafiles.push_back(dataPath+Form("Tree_ElectronGun_Pt-2to250.root"));
    G_Event_Weight = 6025.2 / 997342;
    G_IsData = false;
    G_IsCSA14 = true;
    G_IsMCSumm = false;    
    G_IsElGun = true;
    gPAFOptions->AddDataFiles(datafiles); 
  }

  else {
    DatasetManager* dm = new DatasetManager("Legacy_Summer12_53X");
    dm->RedownloadFiles();
    dm->LoadDataset(sampleName);
    gPAFOptions->AddDataFiles(dm->GetFiles());
    G_Event_Weight = dm->GetCrossSection() / dm->GetEventsInTheSample();
    
    cout << endl;
    cout << "      x-section = " << dm->GetCrossSection()      << endl;
    cout << "        nevents = " << dm->GetEventsInTheSample() << endl;
    cout << " base file name = " << dm->GetBaseFileName()      << endl;
    cout << "         weight = " << G_Event_Weight             << endl;
    cout << endl;


    G_IsData = false;
    G_IsCSA14 = false;
    G_IsMCSumm = true;    

    
  }


  if (G_IsCSA14 && G_IsMCSumm) std::cerr << "Error: Incompatible choices selected" << endl;
  if (!G_IsMCSumm && !G_IsCSA14) std::cerr << "Error: sample must be CSA14 or MCSumm" << endl;
  if (G_IsCSA14) std::cout << "Dealing with CSA14 samples" << endl;

 
    
    

    
  // Output file name
  //----------------------------------------------------------------------------
  Bool_t G_Use_CSVM = false;
  TString outputDir = "/mnt_pool/fanae105/user/sscruz/Fakes/results/";
  //CSVM_METType0I_3rdLepV_FullSyst_Mar20_jet25/";
  gSystem->mkdir(outputDir, kTRUE);
  
  std::ostringstream oss;      
  oss << G_Total_Lumi;
  
  TString LumiString = oss.str();
  
  TString outputFile = outputDir
    + "/"
//    + LumiString
//    + "pb-1_"
    + sampleName
    + suffix
    + ".root";
  
  gPAFOptions->SetOutputFile(outputFile);
  //
  //SANTI  gPAFOptions->outputFile = outputFile;

  // Parameters for the analysis
  //----------------------------------------------------------------------------
  //cout << "DEBUG: " << kSequential << " == " << gPAFOptions->GetPAFMode() << endl;
  if (!G_IsData) { 
    gSystem->AddIncludePath("-D__ISMC");
    if (gPAFOptions->GetPAFMode() != kSequential) 
      proof->Exec("gSystem->AddIncludePath(\"-D__ISMC\");");
  }
  if (G_IsMCSumm) {
    gSystem->AddIncludePath("-D__ISSUMM12");
    if (gPAFOptions->GetPAFMode() != kSequential) 
      proof->Exec("gSystem->AddIncludePath(\"-D__ISSUMM12\");");
  }
  if (G_IsCSA14) {
    gSystem->AddIncludePath("-D__ISCSA14");
    if (gPAFOptions->GetPAFMode() != kSequential)
      proof->Exec("gSystem->AddIncludePath(\"-D__ISCSA14\");");
  }
  if (G_IsElGun) {
    gSystem->AddIncludePath("-D__ISGUN");
    if (gPAFOptions->GetPAFMode() != kSequential)
      proof->Exec("gSystem->AddIncludePath(\"-D__ISGUN\");");
  }

  // See packages/InputParameters/InputParameters.h for information on how
  // to use this class.

  gPAFOptions->inputParameters = new InputParameters();
  
  //  gPAFOptions->inputParameters->SetNamedInt("sys_source",     sys_source);
  //  gPAFOptions->inputParameters->SetNamedInt("sys_direction",  sys_direction);
   
  gPAFOptions->inputParameters->SetNamedString("sampleName",    sampleName.Data());
  gPAFOptions->inputParameters->SetNamedBool  ("IsData",        G_IsData         );
  gPAFOptions->inputParameters->SetNamedBool  ("UseCSVM",       G_Use_CSVM       );
  gPAFOptions->inputParameters->SetNamedFloat ("weight",        G_Event_Weight   );
  gPAFOptions->inputParameters->SetNamedFloat ("LumiForPU",     G_LumiForPUData  );
  gPAFOptions->inputParameters->SetNamedFloat ("TotalLumi",     G_Total_Lumi     );
  gPAFOptions->inputParameters->SetNamedBool  ("DoSystStudies", DoSystStudies    );
  gPAFOptions->inputParameters->SetNamedBool  ("DoFR"         , DoFR             );
  
  // Number of events (Long64_t)
  //----------------------------------------------------------------------------
  gPAFOptions->SetNEvents(nEvents);
  // First event (Long64_t)
  //----------------------------------------------------------------------------
  gPAFOptions->SetFirstEvent(0);
  
  // Name of analysis class
  //----------------------------------------------------------------------------
  // If 0 the default name schema will be used, i.e. depending on the value
  // of gPAFOptions->treeType: MyAnalysisTESCO or MyAnalsyisMiniTrees
  gPAFOptions->SetAnalysis("TreeAnalysisTop");


  //  gPAFOptions->AddPackage("GlobalVariables");

  if (!G_IsMCSumm) gPAFOptions->SetTreeDir("demo");
  gPAFOptions->SetTreeName("Tree");

  // Control output and checks
  //----------------------------------------------------------------------------
  // + If true (default) PAF checks for new version in CVS every time
  // gPAFOptions->checkVersion = true;
  // + If true (default) the output file is reopened so the objects in the
  //   file can be interactively accessed. The object in the output are also
  //   listed
  gPAFOptions->ReopenOutputFile(false);
  
  // Run the analysis
  //----------------------------------------------------------------------------
  //  gPAFOptions->SetMergeThroughFile();
  if (!RunAnalysis())
    cerr << "ERROR: There was a problem running the analysis!" << endl;
}
