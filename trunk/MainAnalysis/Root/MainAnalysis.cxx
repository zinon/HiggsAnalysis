#include <MainAnalysis/MainAnalysis.h>
#include <MainAnalysis/MainAnalysisIncludes.h>
#include <MainAnalysis/MainAnalysisVariables.h>
#include <MainAnalysis/MainAnalysisCPTools.h>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>


// this is needed to distribute the algorithm to the workers
ClassImp(MainAnalysis)

MainAnalysis :: MainAnalysis ()
{
  std::cout<<"Constructor"<<std::endl;
  start = std::chrono::system_clock::now();
}

EL::StatusCode MainAnalysis :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  Info("setupJob", "call <<<=====");
  std::cout<<"setupJob"<<std::endl;

  // std::unique_ptr<TLorentzVector> tlv = CxxUtils::make_unique<TLorentzVector>();
  //  foo5(tlv);
  


  job.useXAOD ();

  //================
  // xJobOptions   ||
  //================
  m_jo = CxxUtils::make_unique<xJobOptions>(m_job_options_file);
 
 if( !m_jo->Configure() )
    Error("setupJob()", "Failed to configure the job options class instance with input file %s",  m_job_options_file.c_str());

  // add PileUpReweighting to file
  EL::OutputStream pu_output("PileUpReweighting");
  if(m_jo->get<bool>(xJobOptions::DO_PILEUP_FILE)){
    Info("setupJob()", "Will create PileUpReweighting output file" );
    job.outputAdd(pu_output);
  }

  // let's initialize the algorithm to use the xAODRootAccess package
  xAOD::Init( "MainAnalysis" ).ignore();        // call before opening first file

std::cout<<"setupJob"<<std::endl;
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MainAnalysis :: fileExecute ()
{
  Info("fileExecute", "call <<<=====");
  // Here you do everything that needs to be done exactly once for every
  // single     file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MainAnalysis :: changeInput (bool firstFile)
{
  Info("changeInput", "call <<<=====");
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.


//retrieve sum of mc weights from MetaData if running on DxAOD
  if(m_isMC && m_jo->get<bool>(xJobOptions::DO_DERIVATION)){
     std::cout<<"((((((((((((((((SumOfWeights))))))))))))))))"<<std::endl;
    
    double finalSumOfWeightsInFile = 0.;
    double initialSumOfWeightsInFile = 0.;
    
    if(firstFile){
      //mc weights for DxAODs
      std::cout<<"Processing First file, setting mc_weight_sums to zero"<<std::endl;
      m_initialSumOfWeights  = 0; 
      m_finalSumOfWeights = 0; 
    }
    
    // get the MetaData tree once a new file is opened, with
     TTree *MetaData = dynamic_cast<TTree*>(wk()->inputFile()->Get("MetaData"));
     if (MetaData) {
        // extract the information from the EventBookkeepers branch
        TTreeFormula tf("tf","EventBookkeepers.m_nWeightedAcceptedEvents",MetaData);
        MetaData->LoadTree(0);
        tf.UpdateFormulaLeaves();
        tf.GetNdata();
        finalSumOfWeightsInFile = tf.EvalInstance(0);
        initialSumOfWeightsInFile = tf.EvalInstance(1);
      }
     
     std::cout<<"In Original xAOD:  "<<initialSumOfWeightsInFile<<"\n In Derivation:    "<<finalSumOfWeightsInFile<<std::endl;
     
     m_finalSumOfWeights += finalSumOfWeightsInFile;
     m_initialSumOfWeights += initialSumOfWeightsInFile; 
    }

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MainAnalysis :: initialize ()
{
std::cout<<"initialize"<<std::endl;
std::cout<<"gridflag "<<m_runGrid<<std::endl;
  start_initialize = std::chrono::system_clock::now();

  // create a local reference to the xAODRootAccess event object
  m_event = wk()->xaodEvent();

  // here we are defining the TStore this tool needs
  // Create a transient object store needed for METRebuilder
  m_store = CxxUtils::make_unique<xAOD::TStore>();

  //number of entries
  m_entries = static_cast< int >( m_event->getEntries() );
  Info("initialize()", "Number of events = %i", m_entries );

  // event info
  const xAOD::EventInfo* eventInfo = 0;
  if( !m_event->retrieve( eventInfo, "EventInfo").isSuccess() ){
    Error("initialize()", "Failed to retrieve EventInfo. Exiting." );
    return EL::StatusCode::FAILURE;
  }
  bool  isMC = false;
  if( eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) isMC = true;
  bool  isData = !isMC;

  //deep "view" copies containers

  //View jets
  //m_ViewElemJetCont  = std::shared_ptr<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
  m_ViewElemJetCont  = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  //View taus
  m_ViewElemTauCont  = new xAOD::TauJetContainer(SG::VIEW_ELEMENTS);
  //View muons
  m_ViewElemMuonCont = new xAOD::MuonContainer(SG::VIEW_ELEMENTS);

  // count number of events - all these counters will go in one vector one day
  m_eventCounter   = 0;
  m_numGrlEvents   = 0;
  m_numCleanEvents = 0;
  m_numWeightedEvents = 0.;

  //other counters
  m_numGoodJets = 0;
  m_numGoodTaus = 0;

  //event information
  m_eventNumber     = -999.;
  m_runNumber       = -999.;
  m_mcChannelNumber = -999.;
  m_rndRunNumber    = -999.;

//mc event weight safety init - only called when vars not supposed to be used (ie data/xAOD)
  if(!(m_isMC) || !(m_jo->get<bool>(xJobOptions::DO_DERIVATION))){
      m_initialSumOfWeights  = -999; //!
      m_finalSumOfWeights = -999; //!
  }

  //========================
  // xTools Helper class   ||
  //========================
  m_t = CxxUtils::make_unique<xTools>();

  //============
  // GRL Tool   ||
  //============
  m_grl = CxxUtils::make_unique<GoodRunsListSelectionTool>("GoodRunsListSelectionTool");
  std::vector<std::string>      vecStringGRL;
  vecStringGRL.push_back("$ROOTCOREBIN/data/MainAnalysis/data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml");
  m_grl->setProperty( "GoodRunsListVec", vecStringGRL);
  m_grl->setProperty("PassThrough", false);     // if true (default) will ignore result of GRL and will just pass all events
  if (!m_grl->initialize().isSuccess()) {
    Error("initialize()", "Failed to properly initialize the GRL. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //===============
  // PileUp Tool  ||
  //===============
  m_pileup = CxxUtils::make_unique<CP::PileupReweightingTool>("PileupReweighting");

  // preliminary pileupconfig file

  std::vector<std::string>   confFiles;
  std::vector<std::string>   lcalcFiles;

  if( m_jo->get<bool>(xJobOptions::DO_PILEUP_REWEIGHTING) ){
    confFiles.push_back("MainAnalysis/PileUp/PileUpReweighting.141027.root");
    lcalcFiles.push_back("MainAnalysis/PileUp/ilumicalc_histograms_EF_mu18_tight_mu8_EFFS_202798.root");
    m_pileup->setProperty( "ConfigFiles", confFiles);
    m_pileup->setProperty( "LumiCalcFiles", lcalcFiles);
    m_pileup->setProperty("UnrepresentedDataAction",2);
    m_pileup->setProperty("DefaultChannel", 110101);
  }

  if( ! m_pileup->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the PileUp Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //====================
  //Jet Cleaning Tool   ||
  //====================

  m_jetCleaning = CxxUtils::make_unique<JetCleaningTool>("JetCleaning");
  m_jetCleaning->msg().setLevel( MSG::ERROR );
  m_jetCleaning->setProperty( "CutLevel", m_jo->get<std::string>(xJobOptions::JET_CLEANING));
  if( ! m_jetCleaning->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Jet Cleaning Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //============
  // JER Tools ||
  //============
  m_JERTool = CxxUtils::make_unique<JERTool>("JERTool");
  m_JERTool->msg().setLevel( MSG::WARNING );
  m_JERTool->setProperty("PlotFileName", "JetResolution/JERProviderPlots_2012.root");
  m_JERTool->setProperty("CollectionName", "AntiKt4LCTopoJets") ;
  m_JERTool->setProperty("BeamEnergy", "8TeV") ;
  m_JERTool->setProperty("SimulationType", "FullSim");

  m_JERSmearingTool = CxxUtils::make_unique<JERSmearingTool>("JERSmearingTool");
  m_JERSmearingTool->msg().setLevel( MSG::WARNING );
  m_JERSmearingTool->setJERTool(m_JERTool.get()); //connect the two tools

  if (! m_JERTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the JER Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  if (! m_JERSmearingTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the JER Smearing Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //===================
  // Jet Calibration  ||
  //===================
  //  auto glambda = [](auto &&filename) -> bool { ifstream ifile(filename.c_str()); return ifile; };
  std::string   JetCalibSeq = isData ? "EtaJES_Insitu" : "EtaJES";
  std::string   JetCalibConf = "JES_Full2012dataset_Preliminary_MC14.config";
  m_jetCalibTool = CxxUtils::make_unique<JetCalibrationTool>( "JetCalibrationTool",
							      m_jo->get<std::string>(xJobOptions::JET_COLLECTION),
							      JetCalibConf,
							      JetCalibSeq,
							      isData);
  m_jetCalibTool->msg().setLevel( MSG::ERROR );
  m_jetCalibTool->initializeTool("JetCalibrationTool");
  if (! m_jetCalibTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Jet Calibration Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //===================
  // JES uncertainty  ||
  //==================
  m_JESUncertaintyTool = CxxUtils::make_unique<JetUncertaintiesTool>("JESUncProvider");
  m_JESUncertaintyTool->msg().setLevel( MSG::WARNING );
  // has to be configurable eventually
  m_JESUncertaintyTool->setProperty("JetDefinition", "AntiKt4LCTopo");
  m_JESUncertaintyTool->setProperty("MCType","MC12");
  // start small
  m_JESUncertaintyTool->setProperty( "ConfigFile","JES_2012/Final/InsituJES2012_14NP.config");
  //  m_JESUncertaintyTool->setProperty("ConfigFile","JES_2012/Final/InsituJES2012_23NP_ByCategory.config");

  //====================
  // TauSmearing Tool   ||
  //====================
  //tau pt already smeared in simulation - pt shifting only in data!
  m_tauSmearTool = CxxUtils::make_unique<TauAnalysisTools::TauSmearingTool>("MainAnalysis-TauSmearingTool");
  m_tauSmearTool->msg().setLevel( MSG::ERROR );
  if (! m_tauSmearTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Tau Smearing Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //====================
  // TauSelectionTool   ||
  //====================
  m_tauSelTool =  CxxUtils::make_unique<TauAnalysisTools::TauSelectionTool>("MainAnalysisTauSelectionTool");
  m_tauSelTool->msg().setLevel( MSG::DEBUG );

  //use predefined cuts
  if( m_jo->get<bool>(xJobOptions::TAU_SELECTION_RECOMMENDED) )
    m_tauSelTool->setRecommendedProperties();
  else{
    //define cuts by hand
    std::vector<double> jobOptions_tau_abs_eta_region = {0, 1.37, 1.52, 2.5};   // define eta regions, excluding crack region
    double jobOptions_tau_abs_charge     = 1 ;      // define absolute charge requirement, in general should be set to 1
    std::vector<size_t> jobOptions_tau_n_tracks = {1,3};        // define number of tracks required, most analysis use 1 and 3 track taus

    m_tauSelTool->setProperty("AbsEtaRegion", jobOptions_tau_abs_eta_region);
    m_tauSelTool->setProperty("PtMin", m_jo->get<double>(xJobOptions::TAU_MIN_PT));
    m_tauSelTool->setProperty("AbsCharge", jobOptions_tau_abs_charge);
    m_tauSelTool->setProperty("NTracks", jobOptions_tau_n_tracks);
    m_tauSelTool->setProperty("JetIDWP", int(m_jo->TauJetBDTindex(m_jo->get<std::string>(xJobOptions::TAU_JET_BDT))) );
    m_tauSelTool->setProperty("EleBDTWP", int(m_jo->TauEleBDTindex(m_jo->get<std::string>(xJobOptions::TAU_ELE_BDT))) );
    //cuts to be executed:
    m_tauSelTool->setProperty( "SelectionCuts",
                               int(
                                   TauAnalysisTools::CutPt
                                   | TauAnalysisTools::CutAbsEta
                                   | TauAnalysisTools::CutAbsCharge
                                   | TauAnalysisTools::CutNTrack
                                   | TauAnalysisTools::CutJetIDWP
                                   | TauAnalysisTools::CutEleBDTWP
                                   ) );

  }
  if (! m_tauSelTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Tau Selection Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //=======================
  // Tau Efficiency Tool  ||
  //=======================
  m_tauEffTool =  CxxUtils::make_unique<TauAnalysisTools::TauEfficiencyCorrectionsTool>("TauEfficiencyCorrectionsTool", m_tauSelTool.get());//synchronize the 2 tau tools
  m_tauEffTool->msg().setLevel( MSG::ERROR );
  m_tauEffTool->setProperty("EfficiencyCorrectionType", (int) TauAnalysisTools::SFJetID);
  //m_tauEffTool->setProperty("IDLevel", (int) TauAnalysisTools::JETIDBDTLOOSE);
  // m_tauEffTool->setProperty("SharePath","RootCoreBin/data/TauAnalysisTools/EfficiencyCorrections/");
  if (! m_tauEffTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Tau Efficiency Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //============================
  // MuonCalibrationSmearing   ||
  //============================
  m_muonCalibrationAndSmearingTool = CxxUtils::make_unique<CP::MuonCalibrationAndSmearingTool>( "MuonCorrectionTool" );
  m_muonCalibrationAndSmearingTool->msg().setLevel( MSG::WARNING );
  if (! m_muonCalibrationAndSmearingTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the MuonCalibrationAndSmearingTool Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //============================
  // Muon Selection tool       ||
  //============================

  m_muonSelectionTool = CxxUtils::make_unique<CP::MuonSelectionTool>("MuonSelection");
  m_muonSelectionTool->msg().setLevel( MSG::WARNING );
  if (! m_muonSelectionTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Muon Selection Tool. Exiting..." );
    return EL::StatusCode::FAILURE;
  }

  //============================
  // Muon Scale Factors tool   ||
  //============================

  m_muonEfficiencyScaleFactorsTool = CxxUtils::make_unique<CP::MuonEfficiencyScaleFactors>("effi_corr");
  m_muonEfficiencyScaleFactorsTool->setProperty("WorkingPoint","CBandST");
  m_muonEfficiencyScaleFactorsTool->setProperty("DataPeriod","2012");
  if (! m_muonEfficiencyScaleFactorsTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the MuonEfficiencyScaleFactors Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //====================
  // METRebuilder       ||
  //====================

  m_metRebuilder =  CxxUtils::make_unique<met::METRebuilder>("METRebuilderTool");

  m_metRebuilder->setProperty("JetColl", m_jo->get<std::string>(xJobOptions::MET_JETCOL)); // key to save the shallowcopyjets.first
  m_metRebuilder->setProperty("MuonColl", m_jo->get<std::string>(xJobOptions::MET_MUONCOL)); // for muons
  m_metRebuilder->setProperty("TauColl", m_jo->get<std::string>(xJobOptions::MET_TAUCOL)); // for taus
  m_metRebuilder->setProperty("EleColl", m_jo->get<std::string>(xJobOptions::MET_ELECOL)); // for electrons
  m_metRebuilder->setProperty("GammaColl","");//need to switch it off explicitely, otherwise it is recalculated from the Photon Container
  m_metRebuilder->setProperty("EleTerm", m_jo->get<std::string>(xJobOptions::MET_ELETERM));
  m_metRebuilder->setProperty("GammaTerm", m_jo->get<std::string>(xJobOptions::MET_GAMMATERM));
  m_metRebuilder->setProperty("TauTerm", m_jo->get<std::string>(xJobOptions::MET_TAUTERM));
  m_metRebuilder->setProperty("JetTerm", m_jo->get<std::string>(xJobOptions::MET_JETTERM));
  m_metRebuilder->setProperty("MuonTerm", m_jo->get<std::string>(xJobOptions::MET_MUONTERM));
  m_metRebuilder->setProperty("SoftTerm", m_jo->get<std::string>(xJobOptions::MET_SOFTTERM));
  m_metRebuilder->setProperty("CalibJetPtCut", m_jo->get<double>(xJobOptions::MET_JETPTCUT) * GeV2MeV);
  m_metRebuilder->setProperty("DoJetJVFCut", m_jo->get<bool>(xJobOptions::MET_JETDOJVF) );
  m_metRebuilder->setProperty("OutputContainer", m_jo->get<std::string>(xJobOptions::MET_OUTMETCONT) );
  m_metRebuilder->setProperty("OutputTotal", m_jo->get<std::string>(xJobOptions::MET_OUTMETTERM));

  m_metRebuilder->msg().setLevel( MSG::INFO);

  if( !m_metRebuilder->initialize() ){
    Error("initialize()", "Failed to initialize METRebuilder. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //====================
  // Systematics       ||
  //====================

  //clear syst list
  m_sysList.clear();

  //define shift range for jet systematics, 14NP - tmp solution
  CP::SystematicSet* jesshiftSet = new CP::SystematicSet();
  jesshiftSet->insert(CP::SystematicVariation("JET_BJES_Response__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_BJES_Response__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_1__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_1__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_2__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_2__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_3__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_3__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_4__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_4__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_5__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_5__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_6__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_6__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_6restTerm__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EffectiveNP_6restTerm__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EtaIntercalibration_Modelling__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EtaIntercalibration_Modelling__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EtaIntercalibration_TotalStat__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_EtaIntercalibration_TotalStat__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Flavor_Composition__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Flavor_Composition__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Flavor_Response__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Flavor_Response__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_OffsetMu__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_OffsetMu__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_OffsetNPV__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_OffsetNPV__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_PtTerm__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_PtTerm__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_RhoTopology__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_Pileup_RhoTopology__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_PunchThrough_MC12__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_PunchThrough_MC12__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_RelativeNonClosure_MC12__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_RelativeNonClosure_MC12__continuous", -1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_SingleParticle_HighPt__continuous", +1.0));
  jesshiftSet->insert(CP::SystematicVariation("JET_SingleParticle_HighPt__continuous", -1.0));

  CP::SystematicRegistry::getInstance().registerSystematics(*jesshiftSet);
  CP::SystematicRegistry::getInstance().addSystematicsToRecommended(*jesshiftSet);

  
  // loop over systematics registry:
  const CP::SystematicRegistry& registry               = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet&      recommendedSystematics = registry.recommendedSystematics();     // get list of recommended systematics

//  // this is the nominal set, no systematic
//  m_sysList.push_back(CP::SystematicSet());

  // using PAT method to derive +- 1sigma systematics
  m_sysList = CP::make_systematics_vector(recommendedSystematics);
  std::cout<<"Systematic variations: "<<m_sysList.size()<<std::endl;
  
//  // loop over recommended systematics
//  for(CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr){
//    m_sysList.push_back( CP::SystematicSet() );
//    m_sysList.back().insert( *sysItr );
//  }

  Info("initialize()", "Number of systematic variations %lu", m_sysList.size() );

  //list of strings of  systematics
  std::vector<std::string>  m_syst;
  for( auto &syst : m_sysList ){
    if( syst.name().empty() )
      m_syst.push_back( "NOMINAL" );
    else
      m_syst.push_back( m_t->replace_substr( syst.name(), "_", "") );
  }

  // initialize JESUnc later
  if (! m_JESUncertaintyTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the JES Uncertainty Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }


  //===============
  // Tree booking ||
  //===============

  //tree initialization
  for( const auto &syst : m_syst){
    Info("initialize()", "systematic: %s", syst.c_str());
    m_Tree.insert(std::make_pair(syst,  new TTree(syst.c_str(), syst.c_str()) )  ); // this will change
  }

  //link variables to tree branches - variables are defined in MainAnalysisVariables.h
  for( auto &tr : m_Tree ){
    tr.second->SetMaxTreeSize();
    for( auto &var : m_tree_var){
      tr.second->Branch( var.first.c_str(), &var.second, (var.first+"/D").c_str() );
    }
  }

  //add trees to wk output
  for( const auto &tr : m_Tree )
    wk()->addOutput(tr.second);

  end_initialize = std::chrono::system_clock::now();
  start_execute = std::chrono::system_clock::now();

std::cout<<"initialize"<<std::endl;
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode MainAnalysis :: execute ()
{

  // Event initializations
  m_evtw  = 1.; //total event weight
  m_puw = 1.; //pile up weight
  m_mcw = 1.; //mc event weight

  if( (m_eventCounter % 1000) ==0 ) Info("execute()", "Event   number = %i", m_eventCounter );
  m_eventCounter++;

  // Event information
  const xAOD::EventInfo *eventInfo = 0;
  if( ! m_event->retrieve( eventInfo, "EventInfo").isSuccess() ){
    Error("execute()", "Failed to retrieve event info. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  m_isMC = eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION );

  const xAOD::VertexContainer* vertices = NULL;
  if( !m_event->retrieve( vertices, "PrimaryVertices").isSuccess() ){
    Error("initialize()", "Failed to retrieve PrimaryVertices!" );
    return EL::StatusCode::FAILURE;
  }

  int m_npv = 0;
  xAOD::VertexContainer::const_iterator vitr;
  for (vitr = vertices->begin(); vitr != vertices->end(); ++vitr){
    if ( (*vitr)->nTrackParticles() > 1) m_npv++;
  }

  //  std::cout<<"npv: "<<NPV<<std::endl;


  //retrieve the mc
  const std::vector<float> weights = m_isMC ? eventInfo->mcEventWeights() : std::vector<float>({});
  m_mcw = weights.size() > 0 ? weights[0] : 1.;
  m_evtw *= m_mcw; //update total event weight

  //retrieving event info
  m_eventNumber     = static_cast< int >(eventInfo->eventNumber() );
  m_runNumber       = static_cast< int >( eventInfo->runNumber() );
  m_mcChannelNumber = m_isMC ? (static_cast< int >( eventInfo->mcChannelNumber() )) : 0;

  //pileup weight : TODO move somewhere else?
  if(m_isMC && (m_jo->get<bool>(xJobOptions::DO_PILEUP_REWEIGHTING) || m_jo->get<bool>(xJobOptions::DO_PILEUP_FILE)) ){

    m_pileup->apply(eventInfo);

    if(m_jo->get<bool>(xJobOptions::DO_PILEUP_REWEIGHTING)){
      m_puw = eventInfo->auxdata< double >( "PileupWeight" );
      m_evtw *= m_puw; //update total event weight
    }
  }

  m_numWeightedEvents +=m_evtw;

  if(m_jo->get<bool>(xJobOptions::DO_PILEUP_FILE)) return EL::StatusCode::SUCCESS;

  //GRL:  if data check if event passes GRL
  if( IsData() ){
    if(!m_grl->passRunLB(*eventInfo)){
      Info("execute()", "discarding event  %i", m_eventNumber );
      return EL::StatusCode::SUCCESS;   // go to next event
    }
  }
  m_numGrlEvents++;

  // Apply event cleaning to remove events due to  problematic regions of the detector, and incomplete events: on data
  if( IsData() ){
    if( (eventInfo->eventFlags(xAOD::EventInfo::LAr)==2) || (eventInfo->eventFlags(xAOD::EventInfo::Tile) == 2) || (eventInfo->eventFlags(xAOD::EventInfo::Core)!=0) ){
      return EL::StatusCode::SUCCESS;   // go to the next event
    }
  }
  m_numCleanEvents++;

  //----------------------------
  // Retrieve xAOD containers  |
  //----------------------------

  // get tau container
  const xAOD::TauJetContainer *TauJetCont = 0;
  if(!m_event->retrieve(TauJetCont, "TauRecContainer" ).isSuccess()){
    Error("execute()", "Failed to retrieve TauRecContainer container. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  // retrieve jet container
  const xAOD::JetContainer*   JetCont = 0;
  if ( !m_event->retrieve( JetCont, (m_jo->get<std::string>(xJobOptions::JET_COLLECTION)+"Jets").c_str() ).isSuccess() ){
    Error("execute()", "Failed to retrieve %s container. Exiting...", m_jo->get<std::string>(xJobOptions::DO_PILEUP_REWEIGHTING).c_str() );
    return EL::StatusCode::FAILURE;
  }

  // get Muon container
  const xAOD::MuonContainer*  MuonCont = 0;
  if( m_event->contains<xAOD::MuonContainer>("Muons") ){
    if ( !m_event->retrieve( MuonCont, "Muons" ).isSuccess() ){
      Error("execute()", "Failed to retrieve Muons container. Exiting." );
      return EL::StatusCode::FAILURE;
    }
  }else{
    Error("execute()", "No Muons container in event %i, run %i. Exiting.", m_eventNumber, m_runNumber );
    return EL::StatusCode::FAILURE;
  }

  //Ele container
  const xAOD::ElectronContainer*  EleCont = 0;
  if( m_event->contains<xAOD::ElectronContainer>("ElectronCollection") ){
    if ( !m_event->retrieve( EleCont, "ElectronCollection" ).isSuccess() ){
      Error("execute()", "Failed to retrieve Electrons container. Exiting." );
      return EL::StatusCode::FAILURE;
    }
  }else{
    Error("execute()", "No Electrons container in event %i, run %i. Exiting.", m_eventNumber, m_runNumber );
    return EL::StatusCode::FAILURE;
  }

  // get default MET container and terms
  const xAOD::MissingETContainer*     MissingETCont = 0;
  if( m_event->contains<xAOD::MissingETContainer>(m_jo->get<std::string>(xJobOptions::MET_CONTAINER)) ){
    if( !m_event->retrieve(MissingETCont, m_jo->get<std::string>(xJobOptions::MET_CONTAINER)) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting.", m_jo->get<std::string>(xJobOptions::MET_CONTAINER).c_str());
      return EL::StatusCode::FAILURE;
    }
  }else{
    Warning("execute()", "Did not find %s container...", m_jo->get<std::string>(xJobOptions::MET_CONTAINER).c_str());
  }

  //retrieve MET and terms
  const xAOD::MissingET       *MET = 0;
  const xAOD::MissingET       *MET_SoftTerm = 0;
  const xAOD::MissingET       *MET_JetTerm  = 0;
  const xAOD::MissingET       *MET_TauTerm = 0;
  const xAOD::MissingET       *MET_MuonTerm = 0;
  const xAOD::MissingET       *MET_EleTerm = 0;
  if( MissingETCont ){
    MET  = (*MissingETCont)[m_jo->get<std::string>(xJobOptions::MET_OUTMETTERM)];
    if(!MET){
      Error("execute()", "Null pointer to %s out MET term", m_jo->get<std::string>(xJobOptions::MET_OUTMETTERM).c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_SoftTerm = (*MissingETCont)[m_jo->get<std::string>(xJobOptions::MET_SOFTTERM)];
    if(!MET_SoftTerm){
      Error("execute()", "Null pointer to MET %s term", m_jo->get<std::string>(xJobOptions::MET_SOFTTERM).c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_JetTerm = (*MissingETCont)[m_jo->get<std::string>(xJobOptions::MET_JETTERM)];
    if(!MET_JetTerm){
      Error("execute()", "Null pointer to MET %s term", m_jo->get<std::string>(xJobOptions::MET_JETTERM).c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_TauTerm = (*MissingETCont)[m_jo->get<std::string>(xJobOptions::MET_TAUTERM)];
    if(!MET_TauTerm){
      Error("execute()", "Null pointer to MET %s term", m_jo->get<std::string>(xJobOptions::MET_TAUTERM).c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_MuonTerm = (*MissingETCont)[m_jo->get<std::string>(xJobOptions::MET_MUONTERM)];
    if(!MET_MuonTerm){
      Error("execute()", "Null pointer to MET %s term", m_jo->get<std::string>(xJobOptions::MET_MUONTERM).c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_EleTerm = (*MissingETCont)[m_jo->get<std::string>(xJobOptions::MET_ELETERM)];
    if(!MET_MuonTerm){
      Error("execute()", "Null pointer to MET %s term", m_jo->get<std::string>(xJobOptions::MET_ELETERM).c_str());
      return EL::StatusCode::FAILURE;
    }

  }else{
    Error("execute()", "Null pointer to MissingET container");
    return EL::StatusCode::FAILURE;
  }

  //------------------
  // Define Goodness  |
  //------------------

  //list of cuts on muons that define the good ones
  std::vector<std::string> vMuonGoodFlags { "accepted", "pt_cut", "iso_cut"};

  //list of cuts on taus that define the good ones
  std::vector<std::string> vTauGoodFlags { "accepted", "no_muon_overlap" };

  //list of cuts on jets that define the good ones
  std::vector<std::string> vJetGoodFlags { "clean", "pt_cut", "eta_cut" , "no_tau_overlap", "jvf_cut"};

  //---------------
  // Systematics  |
  //---------------
  int nvar=0;

  std::vector<CP::SystematicSet>::const_iterator sysListItr_end = m_jo->get<bool>(xJobOptions::DO_SYSTEMATICS) ? m_sysList.end() : m_sysList.begin() + 1;
  for(std::vector<CP::SystematicSet>::const_iterator sysListItr = m_sysList.begin(); sysListItr != sysListItr_end; ++sysListItr){

    /*if(debug) *///std::cout<<"Variation "<<nvar<<std::endl;
    /*
      if(debug){
      if((*sysListItr).name() == ""){
      Info("execute()", "Systematic: Nominal (no syst) ");
      }
      else{
      Info("execute()", "Systematic: %s ", (*sysListItr).name().c_str() );
      }
      }
    */

    std::string systname = (*sysListItr).name(); //tmp - for histos
    std::string systName = (*sysListItr).name().empty() ? "NOMINAL" :  m_t->replace_substr( (*sysListItr).name(), "_", "");//name for tree keyword

    //--------------------------------------------
    // apply recommended systematic for CP Tools |
    //--------------------------------------------

    if( m_muonCalibrationAndSmearingTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
      Warning("execute()", "Cannot configure muon calibration tool for systematic" );
      continue; // go to next systematic
    }

    if( m_muonEfficiencyScaleFactorsTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
      Warning("execute()", "Cannot configure muon efficiency tool for systematic" );
      continue;
    }

    if( m_JERSmearingTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
      Warning("execute()", "Cannot configure JER smearing tool for systematic" );
      continue;
    }

    if( m_JESUncertaintyTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
      Warning("execute()", "Cannot configure JES uncertainty tool for systematic" );
      continue;
    }

    //Temp: leave explicitely at "Nominal", < Base,2.0.18
    //if(nvar==0 || nvar>45){


    if( m_tauSmearTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
      Warning("execute()", "Cannot configure tau smearing tool for systematic" );
      continue;
    }

    if( m_tauEffTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
      Warning("execute()", "Cannot configure tau efficincy tool for systematic" );
      continue;
    }

    //}

    // -----------------------
    // Transient Containers  |
    // -----------------------
    //inside the syst loop: recreate shallow copies in every systematics iteration and apply smearing for each systematc independently

    //create a tau shallow copy
    m_TauJetContShallowCopy = xAOD::shallowCopyContainer( *TauJetCont );

    //jet  shallow copy
    m_JetContShallowCopy = xAOD::shallowCopyContainer( *JetCont );

    // create a shallow copy of the muons container
    m_MuonContShallowCopy = xAOD::shallowCopyContainer( *MuonCont );

    // create a shallow copy of the electrons container
    m_EleContShallowCopy = xAOD::shallowCopyContainer( *EleCont );

    //--------------
    // Muons       |
    //--------------
    for(auto MuonShallowCopyItr = (m_MuonContShallowCopy.first)->begin(); MuonShallowCopyItr != (m_MuonContShallowCopy.first)->end(); MuonShallowCopyItr++) {

      //calibration and smearing
      if(m_jo->get<bool>(xJobOptions::MUON_CALIBRATION_SMEARING)){
        CP::CorrectionCode res = m_muonCalibrationAndSmearingTool->applyCorrection(**MuonShallowCopyItr);
        if( res == CP::CorrectionCode::Error ){
          Warning("execute()","Cannot apply shallow-copy muon calibration/smearing run %i event %i", m_runNumber, m_eventNumber);
          //return EL::StatusCode::FAILURE;
        }
      }

      //Efficiency SF
      float effSF = 1.0;
      if(IsMC()) CP::CorrectionCode mucorrcode = m_muonEfficiencyScaleFactorsTool->getEfficiencyScaleFactor(**MuonShallowCopyItr, effSF);
      (*MuonShallowCopyItr)->auxdata< float >( "eff_sf" ) = static_cast< float >( effSF );

      //isolation
      float ptc20(0);
      (*MuonShallowCopyItr)->isolation(ptc20, xAOD::Iso::ptcone20);

      //muons for tau-muon overlap removal
      ( *MuonShallowCopyItr )->auxdata< int >( "overlap" ) = static_cast< int >(
                                                                                (*MuonShallowCopyItr)->pt() * MeV2GeV > m_jo->get<double>(xJobOptions::MUON_OVERLAP_MIN_PT)
                                                                                &&
                                                                                std::fabs( (*MuonShallowCopyItr)->eta() )  < m_jo->get<double>(xJobOptions::MUON_OVERLAP_MAX_ABS_ETA)
                                                                                //other?
                                                                                ? 1:0);

      //apply selection cuts
      ( *MuonShallowCopyItr )->auxdata< int >( "accepted" ) = static_cast< int >( m_muonSelectionTool->accept( **MuonShallowCopyItr ) ? 1:0);
      ( *MuonShallowCopyItr )->auxdata< int >( "pt_cut" ) = static_cast< int >( (*MuonShallowCopyItr)->pt() * MeV2GeV > m_jo->get<double>(xJobOptions::MUON_MIN_PT) ? 1:0 );
      ( *MuonShallowCopyItr )->auxdata< int >( "iso_cut" ) = static_cast< int >( ptc20/(*MuonShallowCopyItr)->pt() < m_jo->get<double>(xJobOptions::MUON_MAX_ISO) ? 1:0 );

      //determine goodness
      ( *MuonShallowCopyItr )->auxdata< int >( "good" ) = static_cast< int >( m_t->Goodness(*MuonShallowCopyItr, vMuonGoodFlags)  );

    }// Muon loop

    // Link to original muons needed for METRebuilder
    if( ! xAOD::setOriginalObjectLink(*MuonCont, *m_MuonContShallowCopy.first)) {
      Error("execute()", "Failed to set original muons links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }

    // Save corrected muons in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_MuonContShallowCopy.first, m_jo->get<std::string>(xJobOptions::MET_MUONCOL)) ){
      Error("execute()", "Failed to record %s to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_MUONCOL).c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_MuonContShallowCopy.second, m_jo->get<std::string>(xJobOptions::MET_MUONCOL)+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_MUONCOL).c_str() );
      return EL::StatusCode::FAILURE;
    }


    //---------------
    // Taus         |
    //---------------
    for( auto TauJetShallowCopyItr = (m_TauJetContShallowCopy.first)->begin(); TauJetShallowCopyItr != (m_TauJetContShallowCopy.first)->end(); TauJetShallowCopyItr++) {

      //tes correction, only sys in MC, nominal shift in data
      if(m_jo->get<bool>(xJobOptions::TAU_CORRECTION)){
        if( m_tauSmearTool->applyCorrection((**TauJetShallowCopyItr)) != CP::CorrectionCode::Ok){
          Error("execute()","Cannot apply tes correction, eta = %.2f, pt = %.2f. Exiting...", (*TauJetShallowCopyItr)->eta(), (*TauJetShallowCopyItr)->pt());
          return EL::StatusCode::FAILURE;
        }
      }

      //tau efficiency
      double effSF= 1.0;
      if(IsMC()) m_tauEffTool->getEfficiencyScaleFactor((**TauJetShallowCopyItr), effSF);
      (*TauJetShallowCopyItr)->auxdata< double >( "jet_id_eff_sf" ) = static_cast< double >( effSF );

      //tau selection
      (*TauJetShallowCopyItr)->auxdata< int >( "accepted" ) = static_cast< int >( (m_tauSelTool->accept((*TauJetShallowCopyItr)) ? 1:0) );

      //tau -muon overlap
      (*TauJetShallowCopyItr)->auxdata< int >( "no_muon_overlap" ) = static_cast< int >( ! (m_t->TauOverlapsWithMuon( *TauJetShallowCopyItr,
                                                                                                                      m_MuonContShallowCopy.first,
                                                                                                                      m_jo->get<double>(xJobOptions::TAU_MUON_OVERLAP_DR)) ? 1:0) );
      //define goodness
      ( *TauJetShallowCopyItr )->auxdata< int >( "good" ) = static_cast< int >( m_t->Goodness(*TauJetShallowCopyItr, vTauGoodFlags) );

    }

    // Link to original taus needed for METRebuilder
    if( ! xAOD::setOriginalObjectLink(*TauJetCont, *m_TauJetContShallowCopy.first)) { //method is defined in the header file xAODBase/IParticleHelpers.h
      Error("execute()", "Failed to set original tau links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }
    // Save smeared taus in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_TauJetContShallowCopy.first, m_jo->get<std::string>(xJobOptions::MET_TAUCOL) ) ){
      Error("execute()", "Failed to record %s to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_TAUCOL).c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_TauJetContShallowCopy.second, m_jo->get<std::string>(xJobOptions::MET_TAUCOL)+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore. Exiting..", m_jo->get<std::string>(xJobOptions::MET_TAUCOL).c_str() );
      return EL::StatusCode::FAILURE;
    }

    //--------------
    // Jets        |
    //--------------

    int ijet(0); double tmp_jpt(0);
    for(auto JetShallowCopyItr = (m_JetContShallowCopy.first)->begin(); JetShallowCopyItr != (m_JetContShallowCopy.first)->end(); JetShallowCopyItr++){

      tmp_jpt=(*JetShallowCopyItr)->pt();

      // apply jet recalibration only for jets with a min pt
      if(m_jo->get<bool>(xJobOptions::JET_RECALIBRATION))
        if(!m_jo->get<bool>(xJobOptions::JET_PRESELECT_SKIP_CALIB_LOW_PT) || tmp_jpt*MeV2GeV > m_jo->get<double>(xJobOptions::JET_PRESELECT_CALIB_MIN_PT))
          if( m_jetCalibTool->applyCalibration(**JetShallowCopyItr) == CP::CorrectionCode::Error)
            Error("execute()","Cannot apply shallow-copy jet recalibration: run %i event %i", m_runNumber, m_eventNumber);

      //decorate calibration sf
      ( *JetShallowCopyItr )->auxdata< double >( "jet_CalibSF" ) = static_cast< double >((*JetShallowCopyItr)->pt()/tmp_jpt );

      // apply corrections
      if( IsMC() ){
        //apply JES correction
        if(m_jo->get<bool>(xJobOptions::JET_JES_CORRECTION) && m_npv > 0){
          if( m_JESUncertaintyTool->applyCorrection(**JetShallowCopyItr) == CP::CorrectionCode::Error ){
            Error("execute()","Cannot apply shallow-copy JES correction: run %i event %i", m_runNumber, m_eventNumber);
          }
        }

        //apply JER correction
        if(m_jo->get<bool>(xJobOptions::JET_JER_CORRECTION)){
          if( m_JERSmearingTool->applyCorrection(**JetShallowCopyItr) == CP::CorrectionCode::Error ){
            Error("execute()","Cannot apply shallow-copy JER correction: run %i event %i", m_runNumber, m_eventNumber);
          }
        }

      } // end if MC

      //apply jvf
      float jetvf  = ((*JetShallowCopyItr)->auxdata< std::vector<float> >( "JVF" )).at(0);
      ( *JetShallowCopyItr )->auxdata< int >( "jvf_cut" ) = static_cast< int >( 
									       (std::fabs(jetvf) > m_jo->get<double>(xJobOptions::JET_MIN_ABS_JVF) 
										&& 
										std::fabs((*JetShallowCopyItr)->eta()) < m_jo->get<double>(xJobOptions::JET_JVF_CUT_MAX_ABS_ETA)) 
									       ? 1:0);

      //apply jet cleaning
      ( *JetShallowCopyItr )->auxdata< int >( "clean" ) = static_cast< int >( m_jetCleaning->accept( **JetShallowCopyItr ) ? 1:0);

      //apply jet cuts
      ( *JetShallowCopyItr )->auxdata< int >( "pt_cut" ) = static_cast< int >( (*JetShallowCopyItr)->pt() * MeV2GeV > m_jo->get<double>(xJobOptions::JET_MIN_PT) ? 1:0 );
      ( *JetShallowCopyItr )->auxdata< int >( "eta_cut" ) = static_cast< int >( std::fabs( (*JetShallowCopyItr)->eta() ) < m_jo->get<double>(xJobOptions::JET_MAX_ABS_ETA) );

      //jet -tau overlap
      ( *JetShallowCopyItr )->auxdata< int >( "no_tau_overlap" ) = static_cast< int >( ! (m_t->JetOverlapsWithGoodTau( *JetShallowCopyItr,
                                                                                                                       m_TauJetContShallowCopy.first,
                                                                                                                       m_jo->get<double>(xJobOptions::TAU_JET_OVERLAP_DR)) ? 1:0) );
      //define goodness
      ( *JetShallowCopyItr )->auxdata< int >( "good" ) = static_cast< int >( m_t->Goodness(*JetShallowCopyItr, vJetGoodFlags)  );

      ijet++;
    }//end of loop over the jet cont shallow copy

    // Link to original jets needed for METRebuilder
    if( ! xAOD::setOriginalObjectLink(*JetCont, *m_JetContShallowCopy.first)) {
      Error("execute()", "Failed to set original jet links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }

    // Save calibrated jets in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_JetContShallowCopy.first, m_jo->get<std::string>(xJobOptions::MET_JETCOL)) ){
      Error("execute()", "Failed to record %s to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_JETCOL).c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_JetContShallowCopy.second, m_jo->get<std::string>(xJobOptions::MET_JETCOL)+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_JETCOL).c_str() );
      return EL::StatusCode::FAILURE;
    }

    //--------------
    // Electrons    |
    //--------------
    if( ! xAOD::setOriginalObjectLink(*EleCont, *m_EleContShallowCopy.first)) {
      Error("execute()", "Failed to set original electrons links -- MET rebuilding cannot proceed. Exiting...");
      return StatusCode::FAILURE;
    }

    // Save corrected electrons in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_EleContShallowCopy.first, m_jo->get<std::string>(xJobOptions::MET_ELECOL) ) ){
      Error("execute()", "Failed to record %s to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_ELECOL).c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_EleContShallowCopy.second, m_jo->get<std::string>(xJobOptions::MET_ELECOL)+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore. Exiting...", m_jo->get<std::string>(xJobOptions::MET_ELECOL).c_str() );
      return EL::StatusCode::FAILURE;
    }

    //==============================
    // Deep Copy / View Containers ||
    //==============================

    //===========
    // Taus     ||
    //===========

    //select good taus
    for(auto TauJetShallowCopyItr = (m_TauJetContShallowCopy.first)->begin(); TauJetShallowCopyItr != (m_TauJetContShallowCopy.first)->end(); TauJetShallowCopyItr++) {
      if( (*TauJetShallowCopyItr)->auxdata< int >("good") ){
        m_ViewElemTauCont->emplace_back(*TauJetShallowCopyItr); //using emplace_back avoids the extra copy or move operation required when using push_back
	//push_back( std::move(*TauJetShallowCopyItr) ); //move: acts as push_back(T&&)  instead of push_back(const T&),
      }
    }
    //pt sort of the "view elements" jet container
    m_t->PtSort(m_ViewElemTauCont);

    //===========
    // Jets     ||
    //===========
    //select good jets
    for(auto JetShallowCopyItr = (m_JetContShallowCopy.first)->begin(); JetShallowCopyItr != (m_JetContShallowCopy.first)->end(); JetShallowCopyItr++){
      if( (*JetShallowCopyItr)->auxdata< int >("good") ){
        m_ViewElemJetCont->emplace_back(*JetShallowCopyItr);
      }
    }
    //pt sort of the "view elements" jet container
    m_t->PtSort(m_ViewElemJetCont);

    //===========
    // Muons    ||
    //===========

    //select good muons
    for(auto MuonShallowCopyItr = (m_MuonContShallowCopy.first)->begin(); MuonShallowCopyItr != (m_MuonContShallowCopy.first)->end(); MuonShallowCopyItr++) {
      if( (*MuonShallowCopyItr)->auxdata< int >("good") )
        m_ViewElemMuonCont->emplace_back(*MuonShallowCopyItr);
    }
    //pt sort of the "view elements" muon container
    m_t->PtSort(m_ViewElemMuonCont);

    //---------------
    // MET          |
    //---------------

    //retrieve containers from the store - cross-check
    //jets
    const xAOD::JetContainer    *tmpJetCont = 0;
    if( ! m_store->retrieve(tmpJetCont, m_jo->get<std::string>(xJobOptions::MET_JETCOL)) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting...", m_jo->get<std::string>(xJobOptions::MET_JETCOL).c_str());
      return EL::StatusCode::FAILURE;
    }
    //taus
    const xAOD::TauJetContainer    *tmpTauCont = 0;
    if( ! m_store->retrieve(tmpTauCont, m_jo->get<std::string>(xJobOptions::MET_TAUCOL)) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting...", m_jo->get<std::string>(xJobOptions::MET_TAUCOL).c_str());
      return EL::StatusCode::FAILURE;
    }
    //muons
    const xAOD::MuonContainer    *tmpMuonCont = 0;
    if( ! m_store->retrieve(tmpMuonCont, m_jo->get<std::string>(xJobOptions::MET_MUONCOL)) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting...", m_jo->get<std::string>(xJobOptions::MET_MUONCOL).c_str());
      return EL::StatusCode::FAILURE;
    }

    //electrons
    const xAOD::ElectronContainer    *tmpEleCont = 0;
    if( ! m_store->retrieve(tmpEleCont, m_jo->get<std::string>(xJobOptions::MET_ELECOL)) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting...", m_jo->get<std::string>(xJobOptions::MET_ELECOL).c_str());
      return EL::StatusCode::FAILURE;
    }

    //call METrebuilder
    if( !m_metRebuilder->execute() ){
      Error("execute()", "Failed to execute METRebuilder. Exiting...");
      return EL::StatusCode::FAILURE;
    }

    // Retrieve new MET
    m_MissingETCalibCont = 0;
    if( ! m_store->retrieve(m_MissingETCalibCont, m_jo->get<std::string>(xJobOptions::MET_OUTMETCONT)) ){
      Error("execute()", "Failed to retrieve %s. Exiting...", m_jo->get<std::string>(xJobOptions::MET_OUTMETCONT).c_str());
      return EL::StatusCode::FAILURE;
    }

    const xAOD::MissingET *MET_Calib = 0;
    const xAOD::MissingET *MET_Calib_SoftTerm = 0;
    const xAOD::MissingET *MET_Calib_JetTerm = 0;
    const xAOD::MissingET *MET_Calib_TauTerm = 0;
    const xAOD::MissingET *MET_Calib_MuonTerm = 0;
    const xAOD::MissingET *MET_Calib_EleTerm = 0;

    if( m_MissingETCalibCont ){
      MET_Calib  = (*m_MissingETCalibCont)[m_jo->get<std::string>(xJobOptions::MET_OUTMETTERM)];
      if(!MET_Calib){
        Error("execute()", "Null pointer to %s out MET Calib term. Exiting...", m_jo->get<std::string>(xJobOptions::MET_OUTMETTERM).c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_SoftTerm = (*m_MissingETCalibCont)[m_jo->get<std::string>(xJobOptions::MET_SOFTTERM)];
      if(!MET_Calib_SoftTerm){
        Error("execute()", "Null pointer to MET Calib %s term. Exiting...", m_jo->get<std::string>(xJobOptions::MET_SOFTTERM).c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_JetTerm = (*m_MissingETCalibCont)[m_jo->get<std::string>(xJobOptions::MET_JETTERM)];
      if(!MET_Calib_JetTerm){
        Error("execute()", "Null pointer to MET Calib %s term. Exiting...", m_jo->get<std::string>(xJobOptions::MET_JETTERM).c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_TauTerm = (*m_MissingETCalibCont)[m_jo->get<std::string>(xJobOptions::MET_TAUTERM)];
      if(!MET_Calib_TauTerm){
        Error("execute()", "Null pointer to MET Calib %s term. Exiting...", m_jo->get<std::string>(xJobOptions::MET_TAUTERM).c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_MuonTerm = (*m_MissingETCalibCont)[m_jo->get<std::string>(xJobOptions::MET_MUONTERM)];
      if(!MET_Calib_MuonTerm){
        Error("execute()", "Null pointer to MET Calib %s term", m_jo->get<std::string>(xJobOptions::MET_MUONTERM).c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_EleTerm = (*m_MissingETCalibCont)[m_jo->get<std::string>(xJobOptions::MET_ELETERM)];
      if(!MET_Calib_EleTerm){
        Error("execute()", "Null pointer to MET Calib %s term", m_jo->get<std::string>(xJobOptions::MET_ELETERM).c_str());
        return EL::StatusCode::FAILURE;
      }

    }else{
      Error("execute()", "Null pointer to MissingETCalib %s container. Exiting...", m_jo->get<std::string>(xJobOptions::MET_OUTMETCONT).c_str());
      return EL::StatusCode::FAILURE;
    }

    //=============
    // Fill Trees ||
    //=============

    /*    FillTree( m_tree_var

	     );
    */

    //clean tree variables
    for( auto &var : m_tree_var)
      var.second = 0;

    //event weights
    FillTreeVar("weight_mc", m_mcw);
    FillTreeVar("weight_pileup", m_puw);
    FillTreeVar("weight_total", m_evtw);
  
    
    //event
    FillTreeVar("actual_int_per_bunch_crossing",  eventInfo->actualInteractionsPerCrossing() );
    FillTreeVar("average_int_per_bunch_crossing",  eventInfo->averageInteractionsPerCrossing() ); //mu
    FillTreeVar("numPV", m_npv);

    //taus
    FillTreeVar("taus_N", m_ViewElemTauCont->size());
    if(m_ViewElemTauCont->size() > 0){
      FillTreeVar("leading_tau", 1);
      FillTreeVar("leading_tau_pt",  m_ViewElemTauCont->at(0)->pt() * MeV2GeV );
      FillTreeVar("leading_tau_eta", m_ViewElemTauCont->at(0)->eta() );
      FillTreeVar("leading_tau_phi",  m_ViewElemTauCont->at(0)->phi() );
      FillTreeVar("leading_tau_ntrks",  m_ViewElemTauCont->at(0)->nTracks() );
      FillTreeVar("leading_tau_Q", m_ViewElemTauCont->at(0)->charge() );
      FillTreeVar("leading_tau_JetBDT_score", m_ViewElemTauCont->at(0)->discriminant( xAOD::TauJetParameters::BDTJetScore ) );
      FillTreeVar("leading_tau_JetBDT_loose", m_ViewElemTauCont->at(0)->isTau( xAOD::TauJetParameters::JetBDTSigLoose ) );
      FillTreeVar("leading_tau_JetBDT_medium", m_ViewElemTauCont->at(0)->isTau( xAOD::TauJetParameters::JetBDTSigMedium ) );
      FillTreeVar("leading_tau_JetBDT_tight",  m_ViewElemTauCont->at(0)->isTau( xAOD::TauJetParameters::JetBDTSigTight ) );
      FillTreeVar("leading_tau_JetBDT_eff_sf",  m_ViewElemTauCont->at(0)->auxdata< double >( "jet_id_eff_sf" )  );
      FillTreeVar("leading_tau_met_mt", m_t->MT( m_ViewElemTauCont->at(0), MET_Calib) );

    }

    if(m_ViewElemTauCont->size() > 1){
      FillTreeVar("subleading_tau", 1);
      FillTreeVar("subleading_tau_pt",  m_ViewElemTauCont->at(1)->pt() * MeV2GeV );
      FillTreeVar("subleading_tau_eta", m_ViewElemTauCont->at(1)->eta() );
      FillTreeVar("subleading_tau_phi", m_ViewElemTauCont->at(1)->phi() );
      FillTreeVar("subleading_tau_ntrks", m_ViewElemTauCont->at(1)->nTracks() );
      FillTreeVar("subleading_tau_Q", m_ViewElemTauCont->at(1)->charge() );
      FillTreeVar("subleading_tau_JetBDT_score", m_ViewElemTauCont->at(1)->discriminant( xAOD::TauJetParameters::BDTJetScore ) );
      FillTreeVar("subleading_tau_JetBDT_loose", m_ViewElemTauCont->at(1)->isTau( xAOD::TauJetParameters::JetBDTSigLoose ) );
      FillTreeVar("subleading_tau_JetBDT_medium", m_ViewElemTauCont->at(1)->isTau( xAOD::TauJetParameters::JetBDTSigMedium ) );
      FillTreeVar("subleading_tau_JetBDT_tight",  m_ViewElemTauCont->at(1)->isTau( xAOD::TauJetParameters::JetBDTSigTight ) );
      FillTreeVar("subleading_tau_JetBDT_eff_sf", m_ViewElemTauCont->at(1)->auxdata< double >( "jet_id_eff_sf" )  );
    }

    //muons
    FillTreeVar("muons_N", m_ViewElemMuonCont->size());

    if(m_ViewElemMuonCont->size() > 0){
      FillTreeVar("leading_muon", 1);
      FillTreeVar("leading_muon_pt",  m_ViewElemMuonCont->at(0)->pt() * MeV2GeV );
      FillTreeVar("leading_muon_eta", m_ViewElemMuonCont->at(0)->eta() );
      FillTreeVar("leading_muon_phi", m_ViewElemMuonCont->at(0)->phi() );
      FillTreeVar("leading_muon_Q",  m_ViewElemMuonCont->at(0)->charge() );
      FillTreeVar("leading_muon_eff_sf",  m_ViewElemMuonCont->at(0)->auxdata< float >( "eff_sf" )  );
      FillTreeVar("leading_muon_met_mt", m_t->MT( m_ViewElemMuonCont->at(0), MET_Calib) );
    }
    if(m_ViewElemMuonCont->size() > 1){
      FillTreeVar("subleading_muon", 1);
      FillTreeVar("subleading_muon_pt",  m_ViewElemMuonCont->at(1)->pt() * MeV2GeV);
      FillTreeVar("subleading_muon_eta", m_ViewElemMuonCont->at(1)->eta() );
      FillTreeVar("subleading_muon_phi", m_ViewElemMuonCont->at(1)->phi() );
      FillTreeVar("subleading_muon_Q", m_ViewElemMuonCont->at(1)->charge() );
      FillTreeVar("subleading_muon_eff_sf",  m_ViewElemMuonCont->at(1)->auxdata< float >( "eff_sf" )  );
    }

    //jets
    FillTreeVar("jets_N", m_ViewElemJetCont->size());

    if(m_ViewElemJetCont->size() > 0){
      FillTreeVar("leading_jet", 1);
      FillTreeVar("leading_jet_pt",  m_ViewElemJetCont->at(0)->pt() * MeV2GeV );
      FillTreeVar("leading_jet_eta", m_ViewElemJetCont->at(0)->eta() );
      FillTreeVar("leading_jet_phi", m_ViewElemJetCont->at(0)->phi() );
      FillTreeVar("leading_jet_y", m_ViewElemJetCont->at(0)->rapidity() );
      float jetvf  = (m_ViewElemJetCont->at(0)->auxdata< std::vector<float> >( "JVF" )).at(0);
      FillTreeVar("leading_jet_jvf", jetvf);
    }

    if(m_ViewElemJetCont->size() > 1){
      FillTreeVar("subleading_jet", 1);
      FillTreeVar("subleading_jet_pt",  m_ViewElemJetCont->at(1)->pt() * MeV2GeV );
      FillTreeVar("subleading_jet_eta", m_ViewElemJetCont->at(1)->eta() );
      FillTreeVar("subleading_jet_phi", m_ViewElemJetCont->at(1)->phi() );
      FillTreeVar("subleading_jet_y", m_ViewElemJetCont->at(1)->rapidity() );
      float jetvf  = (m_ViewElemJetCont->at(1)->auxdata< std::vector<float> >( "JVF" )).at(0);
      FillTreeVar("subleading_jet_jvf", jetvf);
    }

    //met
    FillTreeVar("met_et", MET_Calib->met() * MeV2GeV );
    FillTreeVar("met_phi", MET_Calib->phi() );
    FillTreeVar("met_sumet", MET_Calib->sumet() * MeV2GeV );

    // tau muon system
    if(m_ViewElemTauCont->size() > 0 && m_ViewElemMuonCont->size() > 0){ 
      FillTreeVar("tau_muon", 1. );
      FillTreeVar("tau_muon_deta", m_t->DeltaEta(m_ViewElemTauCont, m_ViewElemMuonCont) );
      FillTreeVar("tau_muon_dphi", m_t->DeltaPhi(m_ViewElemTauCont, m_ViewElemMuonCont) );
      FillTreeVar("tau_muon_dR", m_t->DeltaR(m_ViewElemTauCont, m_ViewElemMuonCont) );
      FillTreeVar("tau_muon_cosalpha", m_t->CosAlpha(m_ViewElemTauCont, m_ViewElemMuonCont) );
      FillTreeVar("tau_muon_qxq", m_ViewElemTauCont->at(0)->charge() * m_ViewElemMuonCont->at(0)->charge() );
      FillTreeVar("tau_muon_m_vis", m_t->MassVisible(m_ViewElemTauCont, m_ViewElemMuonCont) * MeV2GeV);
      FillTreeVar("tau_muon_vect_sum_pt", m_t->VectorSumPt(m_ViewElemTauCont, m_ViewElemMuonCont) * MeV2GeV );
      FillTreeVar("tau_muon_scal_sum_pt", m_t->ScalarSumPt(m_ViewElemTauCont, m_ViewElemMuonCont) * MeV2GeV);
      FillTreeVar("tau_muon_met_bisect", m_t->METbisect(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0, MET_Calib) );
      FillTreeVar("tau_muon_met_min_dphi", m_t->METmindeltaphi(m_ViewElemTauCont, m_ViewElemMuonCont, MET_Calib) );
      FillTreeVar("tau_muon_met_centrality", m_t->METcentrality(m_ViewElemTauCont, m_ViewElemMuonCont, MET_Calib) );
      double x1_col; double x2_col; double m_col; bool ok_col = m_t->MassCollinear(m_ViewElemTauCont, m_ViewElemMuonCont, MET_Calib, true, m_col, x1_col, x2_col) ;
      FillTreeVar("tau_muon_col", ok_col);
      FillTreeVar("tau_muon_m_col", m_col * MeV2GeV);
      FillTreeVar("tau_muon_x1_col", x1_col);
      FillTreeVar("tau_muon_x2_col", x2_col);

    }

    // dimuon system
    if(m_ViewElemMuonCont->size() > 1){
      FillTreeVar("dimuon", 1. );
      FillTreeVar("dimuon_deta", m_t->DeltaEta(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_dphi", m_t->DeltaPhi(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_dR", m_t->DeltaR(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_cosalpha", m_t->CosAlpha(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_qxq", m_ViewElemMuonCont->at(0)->charge() * m_ViewElemMuonCont->at(1)->charge());
      FillTreeVar("dimuon_m_vis", m_t->MassVisible(m_ViewElemMuonCont) * MeV2GeV);
      
     // std::cout<<"Debug mvis\n";
     // std::cout<<"Container Size "<<m_ViewElemMuonCont->size();
     // std::cout<<"VisMass "<<( (*m_ViewElemMuonCont)[0]->p4() +  (*m_ViewElemMuonCont)[1]->p4() ).M()<<std::endl;
      FillTreeVar("dimuon_m_vis", m_t->MassVisible(m_ViewElemMuonCont) * MeV2GeV);
      
      FillTreeVar("dimuon_vect_sum_pt", m_t->VectorSumPt(m_ViewElemMuonCont) * MeV2GeV );
      FillTreeVar("dimuon_scal_sum_pt", m_t->ScalarSumPt(m_ViewElemMuonCont) * MeV2GeV);
      FillTreeVar("dimuon_met_bisect", m_t->METbisect(m_ViewElemMuonCont, MET_Calib) );
      FillTreeVar("dimuon_met_min_dphi", m_t->METmindeltaphi(m_ViewElemMuonCont, MET_Calib) );
      FillTreeVar("dimuon_met_centrality", m_t->METcentrality(m_ViewElemMuonCont, MET_Calib) );
      double x1_col; double x2_col; double m_col; bool ok_col = m_t->MassCollinear(m_ViewElemMuonCont, MET_Calib, true, m_col, x1_col, x2_col) ;
      FillTreeVar("dimuon_col", ok_col);
      FillTreeVar("dimuon_m_col", m_col * MeV2GeV);
      FillTreeVar("dimuon_x1_col", x1_col);
      FillTreeVar("dimuon_x2_col", x2_col);

    }

    // ditau system
    if(m_ViewElemTauCont->size() > 1){
      FillTreeVar("ditau", 1. );
      FillTreeVar("ditau_deta", m_t->DeltaEta(m_ViewElemTauCont) );
      FillTreeVar("ditau_dphi", m_t->DeltaPhi(m_ViewElemTauCont) );
      FillTreeVar("ditau_dR", m_t->DeltaR(m_ViewElemTauCont) );
      FillTreeVar("ditau_cosalpha", m_t->CosAlpha(m_ViewElemTauCont) );
      FillTreeVar("ditau_qxq", m_ViewElemTauCont->at(0)->charge() * m_ViewElemTauCont->at(1)->charge());
      FillTreeVar("ditau_m_vis", m_t->MassVisible(m_ViewElemTauCont) * MeV2GeV);
      FillTreeVar("ditau_vect_sum_pt", m_t->VectorSumPt(m_ViewElemTauCont) * MeV2GeV );
      FillTreeVar("ditau_scal_sum_pt", m_t->ScalarSumPt(m_ViewElemTauCont) * MeV2GeV);
      FillTreeVar("ditau_met_bisect", m_t->METbisect(m_ViewElemTauCont, MET_Calib) );
      FillTreeVar("ditau_met_min_dphi", m_t->METmindeltaphi(m_ViewElemTauCont, MET_Calib) );
      FillTreeVar("ditau_met_centrality", m_t->METcentrality(m_ViewElemTauCont, MET_Calib) );
      double x1_col; double x2_col; double m_col; bool ok_col = m_t->MassCollinear(m_ViewElemTauCont, MET_Calib, true, m_col, x1_col, x2_col) ;
      FillTreeVar("ditau_col", ok_col);
      FillTreeVar("ditau_m_col", m_col * MeV2GeV);
      FillTreeVar("ditau_x1_col", x1_col);
      FillTreeVar("ditau_x2_col", x2_col);

    }

    //dijet system
    if(m_ViewElemJetCont->size() > 1){
      FillTreeVar("dijet", 1. );
      FillTreeVar("dijet_deta", m_t->DeltaEta(m_ViewElemJetCont) );
      FillTreeVar("dijet_dy", m_t->DeltaRapidity(m_ViewElemJetCont) );
      FillTreeVar("dijet_dphi", m_t->DeltaPhi(m_ViewElemJetCont) );
      FillTreeVar("dijet_dR", m_t->DeltaR(m_ViewElemJetCont) );
      FillTreeVar("dijet_cosalpha", m_t->CosAlpha(m_ViewElemJetCont) );
      FillTreeVar("dijet_m_vis", m_t->MassVisible(m_ViewElemJetCont) * MeV2GeV);
      FillTreeVar("dijet_vect_sum_pt", m_t->VectorSumPt(m_ViewElemJetCont) * MeV2GeV);
      FillTreeVar("dijet_scal_sum_pt", m_t->ScalarSumPt(m_ViewElemJetCont) * MeV2GeV);
      FillTreeVar("dijet_etaxeta", m_ViewElemJetCont->at(0)->eta() * m_ViewElemJetCont->at(1)->eta());

    }

    //fill tree
    if ( m_Tree.find(systName) != m_Tree.end() )
      m_Tree[systName]->Fill();
    else
      Error("execute()", "Cannot fill tree with name '%s'", systName.c_str());


    //##################################################### FillHistos ############################################################
    // Histos will be removed
    if(m_jo->get<bool>(xJobOptions::DO_HISTOS)){

      //---> Event info
      FillH1("event_weight",systname, m_evtw);
      FillH1("pileup_weight", systname, m_puw);
      FillH1("actual_int_per_bunch_crossing", systname, eventInfo->actualInteractionsPerCrossing(), m_evtw);
      FillH1("average_int_per_bunch_crossing", systname, eventInfo->averageInteractionsPerCrossing(), m_evtw); //mu

      //---> Taus
      FillH1("tau_n",systname, m_ViewElemTauCont->size(), m_evtw );
      FillH1("tau_n_all", systname, (TauJetCont)->size(), m_evtw );


      for (auto itr = m_ViewElemTauCont->begin(); itr != m_ViewElemTauCont->end(); ++itr){

        //if(debug) std::cout<<(*itr)->pt()<<std::endl;

        FillH1("tau_pt"       ,systname,(*itr)->pt()*MeV2GeV , m_evtw);
        FillH1("tau_eta"      ,systname,(*itr)->eta() , m_evtw);
        FillH1("tau_phi"      ,systname,(*itr)->phi() , m_evtw);
        //FillH1("tau_E"        ,(*itr)->E() , m_evtw);
        FillH2("tau_SF_eff_vs_pt", systname, (*itr)->pt()*MeV2GeV,(*itr)->auxdata< double >( "tau_SF_eff" ), m_evtw);
      }

      if(m_ViewElemTauCont->size()>0) FillH1("tau_pt_0"        ,systname,(*m_ViewElemTauCont)[0]->pt()*MeV2GeV , m_evtw);
      if(m_ViewElemTauCont->size()>1) FillH1("tau_pt_1"      ,systname,(*m_ViewElemTauCont)[1]->pt()*MeV2GeV , m_evtw);

      if(m_ViewElemTauCont->size()>0) FillH1("tau_eta_0"       ,systname,(*m_ViewElemTauCont)[0]->eta() , m_evtw);
      if(m_ViewElemTauCont->size()>1) FillH1("tau_eta_1"     ,systname,(*m_ViewElemTauCont)[1]->eta() , m_evtw);


      //---> Muons
      FillH1("muon_n",systname, m_ViewElemMuonCont->size(), m_evtw );
      FillH1("muon_n_all", systname, (MuonCont)->size(), m_evtw );

      for (auto itr = m_ViewElemMuonCont->begin(); itr != m_ViewElemMuonCont->end(); ++itr){
        FillH1("muon_pt"       ,systname,(*itr)->pt()*MeV2GeV , m_evtw);
        FillH1("muon_eta"      ,systname,(*itr)->eta() , m_evtw);
        FillH1("muon_phi"      ,systname,(*itr)->phi() , m_evtw);
        //    FillH1("muon_iso"      ,systname,(*itr)->isolation() , m_evtw);

      }

      if(m_ViewElemMuonCont->size() > 0) FillH1("muon_pt_0"       ,systname,(*m_ViewElemMuonCont)[0]->pt()*MeV2GeV , m_evtw);
      if(m_ViewElemMuonCont->size() > 0) FillH1("muon_eta_0"       ,systname,(*m_ViewElemMuonCont)[0]->pt()*MeV2GeV , m_evtw);

      if(m_ViewElemMuonCont->size() > 1) FillH1("muon_pt_1"       ,systname,(*m_ViewElemMuonCont)[1]->pt()*MeV2GeV , m_evtw);
      if(m_ViewElemMuonCont->size() > 1) FillH1("muon_eta_1"       ,systname,(*m_ViewElemMuonCont)[1]->pt()*MeV2GeV , m_evtw);
      if(m_ViewElemMuonCont->size() > 1) FillH1("muons_mll_0_1"       ,systname,m_t->MassVisible(m_ViewElemMuonCont,0,m_ViewElemMuonCont,1)*MeV2GeV, m_evtw);
      if(m_ViewElemMuonCont->size() > 1 && fabs(m_t->MassVisible(m_ViewElemMuonCont,0,m_ViewElemMuonCont,1)*MeV2GeV-91.)<20. ) FillH1("muons_met_zpeak",systname , MET_Calib->met() * MeV2GeV, m_evtw);



      //---> Jets

      FillH1("jet_n", systname, m_ViewElemJetCont->size(), m_evtw );
      FillH1("jet_n_all", systname, JetCont->size(), m_evtw );


      for (auto itr = m_ViewElemJetCont->begin(); itr != m_ViewElemJetCont->end(); ++itr){

        //if(debug) std::cout<<(*itr)->pt()<<std::endl;

        FillH1("jet_pt"       ,systname,(*itr)->pt()*MeV2GeV , m_evtw);
        FillH1("jet_eta"      ,systname,(*itr)->eta() , m_evtw);
        FillH1("jet_phi"      ,systname,(*itr)->phi() , m_evtw);
        //FillH1("jet_E"        ,(*itr)->E() , m_evtw);
        FillH1("jet_JERes"    ,systname,(*itr)->auxdata< double >("jet_JERes") , m_evtw);
        FillH1("jet_CalibSF"  ,systname,(*itr)->auxdata< double >("jet_CalibSF"), m_evtw);
      }

      if(m_ViewElemJetCont->size()>0) FillH1("jet_pt_0"       ,systname,(*m_ViewElemJetCont)[0]->pt()*MeV2GeV , m_evtw);
      if(m_ViewElemJetCont->size()>1) FillH1("jet_pt_1"     ,systname,(*m_ViewElemJetCont)[1]->pt()*MeV2GeV , m_evtw);

      if(m_ViewElemJetCont->size()>0) FillH1("jet_eta_0"      ,systname,(*m_ViewElemJetCont)[0]->eta() , m_evtw);
      if(m_ViewElemJetCont->size()>1) FillH1("jet_eta_1"    ,systname,(*m_ViewElemJetCont)[1]->eta() , m_evtw);


      if(m_ViewElemJetCont->size()>1) FillH1("jet_rapgap"  ,systname, m_t->DeltaEta((*m_ViewElemJetCont)[0],(*m_ViewElemJetCont)[1]) , m_evtw);
      //if(m_ViewElemJetCont->size()>1) FillH1("jet_rapgap"  , m_t->DeltaEta(m_ViewElemJetCont , m_evtw);


      //---> MET

      FillH1("met_et", systname, MET->met()*MeV2GeV, m_evtw);
      FillH1("met_softTerm_et", systname, MET_SoftTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_jetTerm_et", systname, MET_JetTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_tauTerm_et", systname, MET_TauTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_muonTerm_et", systname, MET_MuonTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_eleTerm_et", systname, MET_EleTerm->met()*MeV2GeV, m_evtw);

      FillH1("met_calib_et", systname, MET_Calib->met()*MeV2GeV, m_evtw);
      FillH1("met_calib_softTerm_et", systname, MET_Calib_SoftTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_calib_jetTerm_et", systname, MET_Calib_JetTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_calib_tauTerm_et", systname, MET_Calib_TauTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_calib_muonTerm_et", systname, MET_Calib_MuonTerm->met()*MeV2GeV, m_evtw);
      FillH1("met_calib_eleTerm_et", systname, MET_Calib_EleTerm->met()*MeV2GeV, m_evtw);

      FillH1("met_det", systname, (MET->met()-MET_Calib->met())*MeV2GeV, m_evtw);
      FillH1("met_softTerm_det", systname, (MET_SoftTerm->met()-MET_Calib_SoftTerm->met())*MeV2GeV, m_evtw);
      FillH1("met_jetTerm_det", systname, (MET_JetTerm->met()-MET_Calib_JetTerm->met())*MeV2GeV, m_evtw);
      FillH1("met_tauTerm_det", systname, (MET_TauTerm->met()-MET_Calib_TauTerm->met())*MeV2GeV, m_evtw);
      FillH1("met_muonTerm_det", systname, (MET_MuonTerm->met()-MET_Calib_MuonTerm->met())*MeV2GeV, m_evtw);
      FillH1("met_eleTerm_det", systname, (MET_EleTerm->met()-MET_Calib_EleTerm->met())*MeV2GeV, m_evtw);

    }

    //clear copy VIEW containers:  clearing view container DOES NOT delete contents
    m_ViewElemJetCont->clear();
    m_ViewElemTauCont->clear();
    m_ViewElemMuonCont->clear();

    //do not delete copy containers, like
    //delete JetContShallowCopy.first; delete JetContShallowCopy.second;

    // Clear transient store instead
    m_store->clear();

    nvar++;
  } // end of systematics loop


  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MainAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}

//## finalise method - delete pointers, finish run
EL::StatusCode MainAnalysis :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

std::cout<<"finalize"<<std::endl;
  end_execute = std::chrono::system_clock::now();
  start_finalize = std::chrono::system_clock::now();

  Info( "finalize()","Finished processing %i events", m_eventCounter);
  Info("finalize()", "Number of clean events = %i", m_numCleanEvents);
  Info("finalize()", "Number of weighted events = %f", m_numWeightedEvents);

  // Write PU-configfiles
  // name of file defined in setupJob()
  if(m_jo->get<bool>(xJobOptions::DO_PILEUP_FILE)) 
    m_pileup->WriteToFile(wk()->getOutputFile("PileUpReweighting"));

  //reset/delete/empty ptrs 
  m_grl.reset(); m_grl = nullptr;
  m_pileup.reset(); m_pileup = nullptr;
  m_jetCleaning.reset(); m_jetCleaning = nullptr;
  m_JERTool.reset(); m_JERTool = nullptr;
  m_JERSmearingTool.reset(); m_JERSmearingTool = nullptr;
  m_JESUncertaintyTool.reset(); m_JESUncertaintyTool = nullptr;
  m_jetCalibTool.reset(); m_jetCalibTool = nullptr;
  m_muonSelectionTool.reset(); m_muonSelectionTool = nullptr;
  m_muonCalibrationAndSmearingTool.reset(); m_muonCalibrationAndSmearingTool = nullptr;
  m_muonEfficiencyScaleFactorsTool.reset(); m_muonEfficiencyScaleFactorsTool = nullptr;
  m_tauSelTool.reset();   m_tauSelTool = nullptr;
  m_tauSmearTool.reset(); m_tauSmearTool = nullptr;
  m_tauEffTool.reset(); m_tauEffTool = nullptr;
  m_metRebuilder.reset(); m_metRebuilder = nullptr;
  m_jo.reset(); m_jo = nullptr;
  m_t.reset(); m_t = nullptr;
  m_store.reset(); m_store = nullptr;

  end_finalize = std::chrono::system_clock::now();
  end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  std::chrono::duration<double> elapsed_seconds_initialize = end_initialize-start_initialize;
  std::chrono::duration<double> elapsed_seconds_execute = end_execute-start_execute;
  std::chrono::duration<double> elapsed_seconds_finalize = end_finalize-start_finalize;

  std::cout<< "Elapsed time : " <<std::endl
           <<" program "<< elapsed_seconds.count() << " s"<<std::endl
           <<" initialize "<< elapsed_seconds_initialize.count() << " s"<<std::endl
           <<" execute "<< elapsed_seconds_execute.count() << " s"<<std::endl
           <<" finalize "<< elapsed_seconds_finalize.count() << " s"<<std::endl;
  std::cout<<"Event processing : "<<m_eventCounter/elapsed_seconds_execute.count()<<" Hz ("<<m_eventCounter<<" events)"<<std::endl;

std::cout<<"finalize"<<std::endl;
  return EL::StatusCode::SUCCESS;
}

//## Getter MC bool
bool MainAnalysis::IsMC() const { return m_isMC; }

//## Getter Data bool
bool MainAnalysis::IsData() const { return !m_isMC; }

//##finalise histograms
EL::StatusCode MainAnalysis :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // thait gets called on all worker nodes regardless of whether
  // they processed input events.

  // m_tauSelTool->writeControlHistograms();
  
  //fill histo for sum of mc evt weight 
  FillH1("weight_mc_samplesum",""  , 0 , m_initialSumOfWeights);
  FillH1("weight_mc_samplesum", "" , 1 , m_finalSumOfWeights);
  
  int nT(0);
  for (auto entry : m_Tree)
    if( entry.second->GetEntries() ) nT++;

  int nH1(0);
  for (auto entry : m_H1)
    if( entry.second->GetEntries() ) nH1++;

  int nH2(0);
  for (auto entry : m_H2)
    if( entry.second->GetEntries() ) nH2++;

  Info("histFinalize()", "Number of filled trees = %i", nT);
  Info("histFinalize()", "Number of filled 1D histograms = %i", nH1);
  Info("histFinalize()", "Number of filled 2D histograms = %i", nH2);

  return EL::StatusCode::SUCCESS;
}

//## initialise histograms
EL::StatusCode MainAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  Info("histInitialize", "call <<<=====");

  //event information histograms
std::cout<<"histInitialize"<<std::endl;
std::cout<<"gridflag "<<m_runGrid<<std::endl;

  if(m_runGrid) m_jo = CxxUtils::make_unique<xJobOptions>(m_job_options_file);


  int N_int_pbc = 100; double x_int_pbc = 0; double y_int_pbc = 100;
  int N_pu_w = 2; double x_pu_w = 0; double y_pu_w = 200;

  m_H1["event_weight"] = new TH1D("event_weight","event_weight", 200, -10, 10);
  m_H1["weight_mc_samplesum"] = new TH1D("weight_mc_samplesum","weight_mc_samplesum", 4, -0.5, 3.5);
  m_H1["actual_int_per_bunch_crossing"] = new TH1D("actual_int_per_bunch_crossing", "", N_int_pbc, x_int_pbc, y_int_pbc);
  m_H1["average_int_per_bunch_crossing"] = new TH1D("average_int_per_bunch_crossing", "", N_int_pbc, x_int_pbc, y_int_pbc);
  m_H1["pileup_weight"] = new TH1D("pileup_weight", "pileup_weight", N_pu_w, x_pu_w, y_pu_w);

  //MET related histos


  int N_met_pt = 100; double x_met_pt = 0; double y_met_pt = 250;

  m_H1["met_etsum"] = new TH1D("met_etsum", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_et"] = new TH1D("met_et", "", N_met_pt, x_met_pt, y_met_pt);

  m_H1["met_softTerm_et"] = new TH1D("met_softTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_jetTerm_et"] = new TH1D("met_jetTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_tauTerm_et"] = new TH1D("met_tauTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_muonTerm_et"] = new TH1D("met_muonTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_eleTerm_et"] = new TH1D("met_eleTerm_et", "", N_met_pt, x_met_pt, y_met_pt);

  m_H1["met_calib_etsum"] = new TH1D("met_calib_etsum", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_calib_et"] = new TH1D("met_calib_et", "", N_met_pt, x_met_pt, y_met_pt);

  m_H1["met_calib_softTerm_et"] = new TH1D("met_calib_softTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_calib_jetTerm_et"] = new TH1D("met_calib_jetTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_calib_tauTerm_et"] = new TH1D("met_calib_tauTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_calib_muonTerm_et"] = new TH1D("met_calib_muonTerm_et", "", N_met_pt, x_met_pt, y_met_pt);
  m_H1["met_calib_eleTerm_et"] = new TH1D("met_calib_eleTerm_et", "", N_met_pt, x_met_pt, y_met_pt);


  m_H1["met_det"] = new TH1D("met_det", "", 101, -10.5, 10.5);
  m_H1["met_softTerm_det"] = new TH1D("met_softTerm_det", "", 101, -10.5, 10.5);
  m_H1["met_jetTerm_det"] = new TH1D("met_jetTerm_det", "", 101, -10.5, 10.5);
  m_H1["met_tauTerm_det"] = new TH1D("met_tauTerm_det", "", 101, -10.5, 10.5);
  m_H1["met_muonTerm_det"] = new TH1D("met_muonTerm_det", "", 101, -10.5, 10.5);
  m_H1["met_eleTerm_det"] = new TH1D("met_eleTerm_det", "", 101, -10.5, 10.5);

  //jet object related histograms
  int N_jet_pt = 100; double x_jet_pt = 0; double y_jet_pt = 250;
  int N_jet_eta = 100; double x_jet_eta = -5; double y_jet_eta = 5;
  int N_jet_phi = 100; double x_jet_phi = -3.5; double y_jet_phi = 3.5;
  int N_jet_E = 100; double x_jet_E = 0; double y_jet_E = 500;
  int N_jet_JERes = 200; double x_jet_JERes = 0; double y_jet_JERes = 2.;
  int N_jet_SF = 100; double x_jet_SF = 0; double y_jet_SF= 10;
  int N_jet_n = 11; double x_jet_n = -0.5; double y_jet_n = 10.5;

  m_H1["jet_pt"] = new TH1D("jet_pt", "", N_jet_pt, x_jet_pt, y_jet_pt);
  m_H1["jet_pt_0"] = new TH1D("jet_pt_0", "", N_jet_pt, x_jet_pt, y_jet_pt);
  m_H1["jet_pt_1"] = new TH1D("jet_pt_1", "", N_jet_pt, x_jet_pt, y_jet_pt);

  m_H1["jet_eta"] = new TH1D("jet_eta", "", N_jet_eta, x_jet_eta, y_jet_eta);
  m_H1["jet_eta_0"] = new TH1D("jet_eta_0", "", N_jet_eta, x_jet_eta, y_jet_eta);
  m_H1["jet_eta_1"] = new TH1D("jet_eta_1", "", N_jet_eta, x_jet_eta, y_jet_eta);

  m_H1["jet_phi"] = new TH1D("jet_phi", "", N_jet_phi, x_jet_phi, y_jet_phi);

  m_H1["jet_E"] = new TH1D("jet_E", "", N_jet_E, x_jet_E, y_jet_E);

  m_H1["jet_JERes"] = new TH1D("jet_JERes", "", N_jet_JERes, x_jet_JERes, y_jet_JERes);
  m_H1["jet_CalibSF"] = new TH1D("jet_CalibSF", "", N_jet_SF, x_jet_SF, y_jet_SF);

  m_H1["jet_n"] = new TH1D("jet_n", "", N_jet_n, x_jet_n, y_jet_n);
  m_H1["jet_n_all"] = new TH1D("jet_n_all", "", N_jet_n, x_jet_n, y_jet_n);

  //muon object related histograms
  int N_muon_pt = 100; double x_muon_pt = 0; double y_muon_pt = 100;
  int N_muon_eta = 100; double x_muon_eta = -5; double y_muon_eta = 5;
  int N_muon_phi = 70; double x_muon_phi = -3.5; double y_muon_phi = 3.5;
  int N_muon_n = 11; double x_muon_n = -0.5; double y_muon_n = 10.5;
  int N_muon_mll = 60; double x_muon_mll = 0; double y_muon_mll = 120;


  m_H1["muon_n"] = new TH1D("muon_n", "", N_muon_n, x_muon_n, y_muon_n);
  m_H1["muon_n_all"] = new TH1D("muon_n_all", "", N_muon_n, x_muon_n, y_muon_n);

  m_H1["muon_pt"] = new TH1D("muon_pt","" , N_muon_pt, x_muon_pt, y_muon_pt);
  m_H1["muon_pt_0"] = new TH1D("muon_pt_0","" , N_muon_pt, x_muon_pt, y_muon_pt);
  m_H1["muon_pt_1"] = new TH1D("muon_pt_1","" , N_muon_pt, x_muon_pt, y_muon_pt);

  m_H1["muon_eta"] = new TH1D("muon_eta","" , N_muon_eta, x_muon_eta, y_muon_eta);
  m_H1["muon_eta_0"] = new TH1D("muon_eta_0","" , N_muon_eta, x_muon_eta, y_muon_eta);
  m_H1["muon_eta_1"] = new TH1D("muon_eta_1","" , N_muon_eta, x_muon_eta, y_muon_eta);

  m_H1["muon_phi"] = new TH1D("muon_phi","" , N_muon_phi, x_muon_phi, y_muon_phi);

  m_H1["muons_met_zpeak"] = new TH1D("muons_met_zpeak", "", N_met_pt, x_met_pt, y_met_pt);

  m_H1["muons_mll_0_1"] = new TH1D("muons_mll_0_1", "", N_muon_mll, x_muon_mll, y_muon_mll);

  //tau object related histograms
  int N_tau_pt = 100; double x_tau_pt = 0; double y_tau_pt = 100;
  int N_tau_eta = 100; double x_tau_eta = -5; double y_tau_eta = 5;
  int N_tau_phi = 70; double x_tau_phi = -3.5; double y_tau_phi = 3.5;
  int N_tau_E = 100; double x_tau_E = 0; double y_tau_E = 500;
  int N_tau_n = 11; double x_tau_n = -0.5; double y_tau_n = 10.5;
  int N_tau_SF = 100; double x_tau_SF = 0; double y_tau_SF = 2.;

  m_H1["tau_pt"] = new TH1D("tau_pt","" , N_tau_pt, x_tau_pt, y_tau_pt);
  m_H1["tau_pt_corrected"] = new TH1D("tau_pt_corrected", "", N_tau_pt, x_tau_pt, y_tau_pt);

  m_H1["tau_pt_0"] = new TH1D("tau_pt_0", "", N_tau_pt, x_tau_pt, y_tau_pt);
  m_H1["tau_pt_1"] = new TH1D("tau_pt_1", "", N_tau_pt, x_tau_pt, y_tau_pt);

  m_H1["tau_eta"] = new TH1D("tau_eta", "", N_tau_eta, x_tau_eta, y_tau_eta);
  m_H1["tau_eta_0"] = new TH1D("tau_eta_0", "", N_tau_eta, x_tau_eta, y_tau_eta);
  m_H1["tau_eta_1"] = new TH1D("tau_eta_1", "", N_tau_eta, x_tau_eta, y_tau_eta);

  m_H1["tau_phi"] = new TH1D("tau_phi", "", N_tau_phi, x_tau_phi, y_tau_phi);

  m_H1["tau_E"] = new TH1D("tau_E", "", N_tau_E, x_tau_E, y_tau_E);

  m_H1["tau_n"] = new TH1D("tau_n", "", N_tau_n, x_tau_n, y_tau_n);
  m_H1["tau_n_all"] = new TH1D("tau_n_all", "", N_tau_n, x_tau_n, y_tau_n);

  m_H1["tau_SF_eff"] = new TH1D("tau_SF_eff", "", N_tau_SF, x_tau_SF, y_tau_SF);
  m_H2["tau_SF_eff_vs_pt"] = new TH2D("tau_SF_eff_vs_pt", "",N_tau_pt, x_tau_pt, y_tau_pt, N_tau_SF, x_tau_SF, y_tau_SF);


  //analysis related

  int N_rapgap = 80; double x_rapgap = 0; double y_rapgap = 8.;

  m_H1["jet_rapgap"] = new TH1D("jet_rapgap", "", N_rapgap, x_rapgap, y_rapgap);

  // Add histograms to job and set Sumw2
  for(std::map<std::string, TH1*>::iterator H1Itr = m_H1.begin(); H1Itr != m_H1.end(); H1Itr++){
    TH1 *h1 = H1Itr->second;
    h1->Sumw2();
    wk()->addOutput(h1);
  }

  Info("histInitialize()", "Number of 1D histograms booked = %lu", m_H1.size() );


  for(std::map<std::string, TH2*>::iterator H2Itr = m_H2.begin(); H2Itr != m_H2.end(); H2Itr++){
    TH2 *h2 = H2Itr->second;
    h2->Sumw2();
    wk()->addOutput(h2);
  }

  Info("histInitialize()", "Number of 2D histograms booked = %lu", m_H2.size() );

std::cout<<"histInitialize"<<std::endl;
  return EL::StatusCode::SUCCESS;
}

//## Fill 2D hists

void MainAnalysis::FillH1(const std::string &name,const std::string &syst, double x){
  FillH1(name, syst, x, 1);
}

void MainAnalysis::FillH1w(const std::string &name,const std::string &syst, double x){
  FillH1(name, syst, x, m_evtw);
}

void MainAnalysis::FillH1(const std::string &name,const std::string &syst, double x, double w ){
  if(syst.empty()){
    if ( m_H1.find(name) == m_H1.end() ) {
      Error("FillH1()", "Original Histogram %s not found ...", name.c_str() );
    }
    else m_H1[name]->Fill(x, w);
  }
  else{
    if(m_H1.find((syst+"/"+name).c_str()) == m_H1.end() ){
      Warning("FillH1()", "Syst Histogram %s not found, creating ...", (syst+"/"+name).c_str());

      m_H1[(syst+"/"+name).c_str()]=(TH1D*) m_H1[name]->Clone((syst+"/"+name).c_str());
      m_H1[(syst+"/"+name).c_str()]->Reset();
      wk()->addOutput(m_H1[(syst+"/"+name).c_str()]);
      m_H1[(syst+"/"+name).c_str()]->Fill(x, w);
    }
    else {
      m_H1[(syst+"/"+name).c_str()]->Fill(x, w);
    }
  }
}

//## Fill 2D hists
void MainAnalysis::FillH2(const std::string &name, const std::string &syst, double x, double y){
  FillH2(name, syst, x, y, 1.);
}

void MainAnalysis::FillH2w(const std::string &name,const std::string &syst, double x, double y){
  FillH2(name, syst, x, y, m_evtw);
}

void MainAnalysis::FillH2(const std::string &name,const std::string &syst, double x, double y,  double w ){
  if(syst.empty()){
    if ( m_H2.find(name) == m_H2.end() ) {
      Error("FillH2()", "Original Histogram %s not found ...", name.c_str() );
    }
    else m_H2[name]->Fill(x, y, w);
  }
  else{
    if(m_H2.find((syst+"/"+name).c_str()) == m_H2.end() ){
      Warning("FillH2()", "Syst Histogram %s not found, creating ...", (syst+"/"+name).c_str());

      m_H2[(syst+"/"+name).c_str()]=(TH2D*) m_H2[name]->Clone((syst+"/"+name).c_str());
      m_H2[(syst+"/"+name).c_str()]->Reset();
      wk()->addOutput(m_H2[(syst+"/"+name).c_str()]);
      m_H2[(syst+"/"+name).c_str()]->Fill(x, y, w);
    }
    else {
      m_H2[(syst+"/"+name).c_str()]->Fill(x, y, w);
    }
  }
}

void MainAnalysis::FillTreeVar(const std::string &varname, const double varvalue){

  if(m_tree_var.find(varname) != m_tree_var.end() )
    m_tree_var[varname] = varvalue;
  else
    Warning("FillTreeVar()", "Cannot find variable named '%s' ", varname.c_str());

}
