#include <MainAnalysis/MainAnalysis.h>
#include <MainAnalysis/MainAnalysisIncludes.h>
#include <MainAnalysis/MainAnalysisVariables.h>
//#include <MainAnalysis/MainAnalysisJobOptions.h>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

// this is needed to distribute the algorithm to the workers
ClassImp(MainAnalysis)

MainAnalysis :: MainAnalysis ()
{

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

  job.useXAOD ();
  // add PileUpReweighting to file
  EL::OutputStream      pu_output("PileUpReweighting");
  if(jobOptions_createPUfile) job.outputAdd(pu_output);

  // let's initialize the algorithm to use the xAODRootAccess package
  xAOD::Init( "MainAnalysis" ).ignore();        // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MainAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single     file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MainAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MainAnalysis :: initialize ()
{

  // create a local reference to the xAODRootAccess event object
  m_event = wk()->xaodEvent();

  // here we are defining the TStore this tool needs
  // Create a transient object store needed for METRebuilder
  m_store = new xAOD::TStore();

  //number of entries
  m_entries = static_cast< int >( m_event->getEntries() );

  Info("initialize()", "Number of events = %i", m_entries );

  //==============
  // event info ||
  //==============
  const xAOD::EventInfo*        eventInfo = 0;
  if( !m_event->retrieve( eventInfo, "EventInfo").isSuccess() ){
    Error("initialize()", "Failed to retrieve EventInfo. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  bool  isMC                                                      = false;
  if( eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) isMC = true;
  bool  isData                                                    = !isMC;


  //View jets
  m_ViewElemJetCont  = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  //View taus
  m_ViewElemTauCont  = new xAOD::TauJetContainer(SG::VIEW_ELEMENTS);
  //View muons
  m_ViewElemMuonCont = new xAOD::MuonContainer(SG::VIEW_ELEMENTS);


  // count number of events
  m_eventCounter   = 0;
  m_numGrlEvents   = 0;
  m_numCleanEvents = 0;
  m_numWeightedEvents = 0.;

  //other counters
  m_numGoodJets = 0;
  m_numGoodTaus = 0;

  //simulation flag
  m_isMC = false;

  //event information
  m_eventNumber     = -999.;
  m_runNumber       = -999.;
  m_mcChannelNumber = -999.;
  m_rndRunNumber    = -999.;
  //
  //m_hm            = new HistoManager;

  //----------------------------
  // Common Analysis Tools
  //---------------------------

  //============
  // GRL Tool   ||
  //============
  m_grl = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");
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
  m_pileup= new CP::PileupReweightingTool("PileupReweighting");

  // preliminary pileupconfig file
  
    std::vector<std::string>   confFiles;
    std::vector<std::string>   lcalcFiles;

    if(jobOptions_doPileupReweighting){
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

  m_jetCleaning = new JetCleaningTool("JetCleaning");
  m_jetCleaning->msg().setLevel( MSG::ERROR );
  m_jetCleaning->setProperty( "CutLevel", jobOptions_jet_cleaning);
  if( ! m_jetCleaning->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Jet Cleaning Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //============
  // JER Tools ||
  //============
  m_JERTool = new JERTool("JERTool");//, "JetResolution/JERProviderPlots_2012.root", "AntiKt4LCTopoJES");
  m_JERTool->msg().setLevel( MSG::WARNING );
  m_JERTool->setProperty("PlotFileName", "JetResolution/JERProviderPlots_2012.root");
  m_JERTool->setProperty("CollectionName", "AntiKt4LCTopoJets") ;
  m_JERTool->setProperty("BeamEnergy", "8TeV") ;
  m_JERTool->setProperty("SimulationType", "FullSim");

  m_JERSmearingTool = new JERSmearingTool("JERSmearingTool");
  m_JERSmearingTool->msg().setLevel( MSG::WARNING );
  m_JERSmearingTool->setJERTool(m_JERTool); //connect the two tools

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

  std::string   JetCalibSeq = isData ? "EtaJES_Insitu" : "EtaJES";
  std::string   JetCalibConf = "$ROOTCOREBIN/data/JetCalibTools/CalibrationConfigs/JES_Full2012dataset_Preliminary_MC14.config";
  m_jetCalibTool = new JetCalibrationTool( "JetCalibrationTool",
                                           jobOptions_jet_collection,
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
  //===================
  m_JESUncertaintyTool = new JetUncertaintiesTool("JESUncProvider");
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
  m_tauSmearTool = new TauAnalysisTools::TauSmearingTool("MainAnalysis-TauSmearingTool");
  m_tauSmearTool->msg().setLevel( MSG::ERROR );
  if (! m_tauSmearTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Tau Smearing Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //====================
  // TauSelectionTool   ||
  //====================
  m_tauSelTool = new TauAnalysisTools::TauSelectionTool("MainAnalysisTauSelectionTool");
  m_tauSelTool->msg().setLevel( MSG::DEBUG );

  //use predefined cuts
  if(jobOptions_tau_selection_recommended)
    m_tauSelTool->setRecommendedProperties();
  else{
    //define cuts by hand
    std::vector<double>           jobOptions_tau_abs_eta_region = {0, 1.37, 1.52, 2.5};   // define eta regions, excluding crack region
    double                       jobOptions_tau_abs_charge     = 1 ;      // define absolute charge requirement, in general should be set to 1
    std::vector<size_t>           jobOptions_tau_n_tracks = {1,3};        // define number of tracks required, most analysis use 1 and 3 track taus

    m_tauSelTool->setProperty("AbsEtaRegion", jobOptions_tau_abs_eta_region);
    m_tauSelTool->setProperty("PtMin", jobOptions_tau_min_pt);
    m_tauSelTool->setProperty("AbsCharge", jobOptions_tau_abs_charge);
    m_tauSelTool->setProperty("NTracks", jobOptions_tau_n_tracks);
    m_tauSelTool->setProperty("JetIDWP", int(m_bt.TauJetBDTindex(jobOptions_tau_jet_bdt) ) );
    m_tauSelTool->setProperty("EleBDTWP", int(m_bt.TauEleBDTindex(jobOptions_tau_ele_bdt) ) );
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

  m_tauEffTool = new TauAnalysisTools::TauEfficiencyCorrectionsTool("TauEfficiencyCorrectionsTool",m_tauSelTool);
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

  m_muonCalibrationAndSmearingTool = new CP::MuonCalibrationAndSmearingTool( "MuonCorrectionTool" );
  m_muonCalibrationAndSmearingTool->msg().setLevel( MSG::WARNING );
  if (! m_muonCalibrationAndSmearingTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the MuonCalibrationAndSmearingTool Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //============================
  // Muon Selection tool       ||
  //============================

  m_muonSelectionTool = new CP::MuonSelectionTool("MuonSelection");
  m_muonSelectionTool->msg().setLevel( MSG::WARNING );
  if (! m_muonSelectionTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the Muon Selection Tool. Exiting..." );
    return EL::StatusCode::FAILURE;
  }

  //============================
  // Muon Scale Factors tool   ||
  //============================

  m_muonEfficiencyScaleFactorsTool = new CP::MuonEfficiencyScaleFactors("effi_corr");
  m_muonEfficiencyScaleFactorsTool->setProperty("WorkingPoint","CBandST");
  m_muonEfficiencyScaleFactorsTool->setProperty("DataPeriod","2012");
  if (! m_muonEfficiencyScaleFactorsTool->initialize().isSuccess() ){
    Error("initialize()", "Failed to properly initialize the MuonEfficiencyScaleFactors Tool. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //====================
  // METRebuilder       ||
  //====================

  m_metRebuilder = new met::METRebuilder("METRebuilderTool");

  m_metRebuilder->setProperty("JetColl", jobOptions_met_jetColl); // key to save the shallowcopyjets.first
  m_metRebuilder->setProperty("MuonColl", jobOptions_met_muonColl); // for muons
  m_metRebuilder->setProperty("TauColl", jobOptions_met_tauColl); // for taus
  m_metRebuilder->setProperty("EleColl", jobOptions_met_eleColl); // for electrons
  m_metRebuilder->setProperty("GammaColl","");//need to switch it off explicitely, otherwise it is recalculated from the Photon Container
  m_metRebuilder->setProperty("OutputContainer", jobOptions_met_outMETCont);
  m_metRebuilder->setProperty("OutputTotal", jobOptions_met_outMETTerm);
  m_metRebuilder->setProperty( "EleTerm", jobOptions_met_eleTerm);
  m_metRebuilder->setProperty( "GammaTerm", jobOptions_met_gammaTerm);
  m_metRebuilder->setProperty( "TauTerm", jobOptions_met_tauTerm);
  m_metRebuilder->setProperty( "JetTerm", jobOptions_met_jetTerm);
  m_metRebuilder->setProperty( "MuonTerm", jobOptions_met_muonTerm);
  m_metRebuilder->setProperty( "SoftTerm", jobOptions_met_softTerm);
  m_metRebuilder->setProperty("CalibJetPtCut", jobOptions_met_jetPtCut * GeV2MeV);
  m_metRebuilder->setProperty("DoJetJVFCut", jobOptions_met_jetDoJvf);

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
  jesshiftSet->insert(CP::SystematicVariation("BJES_Response",1.0));
  jesshiftSet->insert(CP::SystematicVariation("BJES_Response",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_1",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_1",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_2",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_2",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_3",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_3",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_4",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_4",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_5",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_5",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_6restTerm",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EffectiveNP_6restTerm",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EtaIntercalibration_Modelling",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EtaIntercalibration_Modelling",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("EtaIntercalibration_TotalStat",1.0));
  jesshiftSet->insert(CP::SystematicVariation("EtaIntercalibration_TotalStat",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("RelativeNonClosure_MC12",1.0));
  jesshiftSet->insert(CP::SystematicVariation("RelativeNonClosure_MC12",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("SingleParticle_HighPt",1.0));
  jesshiftSet->insert(CP::SystematicVariation("SingleParticle_HighPt",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("Flavor_Response",1.0));
  jesshiftSet->insert(CP::SystematicVariation("Flavor_Response",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("Flavor_Composition",1.0));
  jesshiftSet->insert(CP::SystematicVariation("Flavor_Composition",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_OffsetMu",1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_OffsetMu",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_OffsetNPV",1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_OffsetNPV",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_PtTerm",1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_PtTerm",-1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_RhoTopology",1.0));
  jesshiftSet->insert(CP::SystematicVariation("Pileup_RhoTopology",-1.0));

  CP::SystematicRegistry::getInstance().registerSystematics(*jesshiftSet);
  CP::SystematicRegistry::getInstance().addSystematicsToRecommended(*jesshiftSet);

  // loop over systematics registry:
  const CP::SystematicRegistry& registry               = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet&      recommendedSystematics = registry.recommendedSystematics();     // get list of recommended systematics

  // this is the nominal set, no systematic
  m_sysList.push_back(CP::SystematicSet());

  // loop over recommended systematics
  for(CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr){
    m_sysList.push_back( CP::SystematicSet() );
    m_sysList.back().insert( *sysItr );
  }

  //list of strings of  systematics
  std::vector<std::string>  m_syst;
  for( auto &syst : m_sysList ){
    if( syst.name().empty() )
      m_syst.push_back( "NOMINAL" );
    else
      m_syst.push_back( m_bt.replace_substr( syst.name(), "_", "") );
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

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode MainAnalysis :: execute ()
{
  // Event initializations
  m_evtw  = 1.; //total event weight
  m_puw = 1.; //pile up weight
  m_mcw = 1.; //mc event weight


  //bool debug = 0;  // tmp - Info output: if true, print - temporary, will be removed once the code becomes stable

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
  //m_pileup->SetRandomSeed( m_eventNumber+m_mcChannelNumber );
  if(m_isMC){
    m_pileup->apply(eventInfo);
    // m_rndRunNumber = m_pileup->GetRandomRunNumber(eventInfo->runNumber());
    // m_puw = m_pileup->GetCombinedWeight(m_rndRunNumber, m_mcChannelNumber,eventInfo->actualInteractionsPerCrossing());
    //m_puw = eventInfo->auxdata< double >( "PileupWeight" );
  }
  m_evtw *= m_puw; //update total event weight
  m_numWeightedEvents +=m_evtw;

  if(jobOptions_createPUfile) return EL::StatusCode::SUCCESS;

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
  if ( !m_event->retrieve( JetCont, (jobOptions_jet_collection+"Jets").c_str() ).isSuccess() ){
    Error("execute()", "Failed to retrieve AntiKt4LCTopoJets container. Exiting..." );
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
  if( m_event->contains<xAOD::MissingETContainer>(jobOptions_met_container) ){
    if( !m_event->retrieve(MissingETCont, jobOptions_met_container) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting.", jobOptions_met_container.c_str());
      return EL::StatusCode::FAILURE;
    }
  }else{
    Warning("execute()", "Did not find %s container...", jobOptions_met_container.c_str());
  }

  //retrieve MET and terms
  const xAOD::MissingET       *MET = 0;
  const xAOD::MissingET       *MET_SoftTerm = 0;
  const xAOD::MissingET       *MET_JetTerm  = 0;
  const xAOD::MissingET       *MET_TauTerm = 0;
  const xAOD::MissingET       *MET_MuonTerm = 0;
  const xAOD::MissingET       *MET_EleTerm = 0;
  if( MissingETCont ){
    MET  = (*MissingETCont)[jobOptions_met_outMETTerm];
    if(!MET){
      Error("execute()", "Null pointer to %s out MET term", jobOptions_met_outMETTerm.c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_SoftTerm = (*MissingETCont)[jobOptions_met_softTerm];
    if(!MET_SoftTerm){
      Error("execute()", "Null pointer to MET %s term", jobOptions_met_softTerm.c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_JetTerm = (*MissingETCont)[jobOptions_met_jetTerm];
    if(!MET_JetTerm){
      Error("execute()", "Null pointer to MET %s term", jobOptions_met_jetTerm.c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_TauTerm = (*MissingETCont)[jobOptions_met_tauTerm];
    if(!MET_TauTerm){
      Error("execute()", "Null pointer to MET %s term", jobOptions_met_tauTerm.c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_MuonTerm = (*MissingETCont)[jobOptions_met_muonTerm];
    if(!MET_MuonTerm){
      Error("execute()", "Null pointer to MET %s term", jobOptions_met_muonTerm.c_str());
      return EL::StatusCode::FAILURE;
    }

    MET_EleTerm = (*MissingETCont)[jobOptions_met_eleTerm];
    if(!MET_MuonTerm){
      Error("execute()", "Null pointer to MET %s term", jobOptions_met_eleTerm.c_str());
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

  for (std::vector<CP::SystematicSet>::const_iterator sysListItr = m_sysList.begin(); sysListItr != m_sysList.end(); ++sysListItr){
  //  for (std::vector<CP::SystematicSet>::const_iterator sysListItr = m_sysList.begin(); sysListItr < m_sysList.begin() + 1; ++sysListItr){//tmp - just to test nominal only
    if(!jobOptions_doSystematics && nvar>0) break;

    //if(debug) std::cout<<"Variation "<<nvar<<std::endl;
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
    std::string systName = (*sysListItr).name().empty() ? "NOMINAL" :  m_bt.replace_substr( (*sysListItr).name(), "_", "");//name for tree keyword

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
      //if(debug) std::cout<<"Before muon calibration:"<<(*MuonShallowCopyItr)->pt()<<std::endl;

      if(jobOptions_apply_muon_calibration_and_smearing){
	CP::CorrectionCode res = m_muonCalibrationAndSmearingTool->applyCorrection(**MuonShallowCopyItr);
	if( res == CP::CorrectionCode::Error ){
	  Warning("execute()","Cannot apply shallow-copy muon calibration/smearing. Exiting...");
	  //return EL::StatusCode::FAILURE;
	}
      }

      //if(debug) std::cout<<"After muon calibration:"<<(*MuonShallowCopyItr)->pt()<<std::endl;

      //Efficiency SF
      float effSF = 1.0;
      if(IsMC()) CP::CorrectionCode mucorrcode = m_muonEfficiencyScaleFactorsTool->getEfficiencyScaleFactor(**MuonShallowCopyItr, effSF);
      (*MuonShallowCopyItr)->auxdata< float >( "eff_sf" ) = static_cast< float >( effSF );

      //if(debug) std::cout<<"Muon eff SF: "<<effSF<<std::endl;

      //isolation
      float ptc20(0);
      (*MuonShallowCopyItr)->isolation(ptc20, xAOD::Iso::ptcone20);

      //muons for tau-muon overlap removal
      ( *MuonShallowCopyItr )->auxdata< int >( "overlap" ) = static_cast< int >( 
										(*MuonShallowCopyItr)->pt() * MeV2GeV > jobOptions_muon_OR_min_pt &&
										std::fabs( (*MuonShallowCopyItr)->eta() )  < jobOptions_muon_OR_max_abs_eta
										//other?
										? 1:0);

      //apply selection cuts
      ( *MuonShallowCopyItr )->auxdata< int >( "accepted" ) = static_cast< int >( m_muonSelectionTool->accept( **MuonShallowCopyItr ) ? 1:0);
      ( *MuonShallowCopyItr )->auxdata< int >( "pt_cut" ) = static_cast< int >( (*MuonShallowCopyItr)->pt() * MeV2GeV > jobOptions_muon_min_pt ? 1:0 );
      ( *MuonShallowCopyItr )->auxdata< int >( "iso_cut" ) = static_cast< int >( ptc20/(*MuonShallowCopyItr)->pt() < jobOptions_muon_max_iso ? 1:0 );

      //determine goodness
      ( *MuonShallowCopyItr )->auxdata< int >( "good" ) = static_cast< int >( m_bt.Goodness(*MuonShallowCopyItr, vMuonGoodFlags)  );

    }// Muon loop

    // Link to original muons needed for METRebuilder
    if( ! xAOD::setOriginalObjectLink(*MuonCont, *m_MuonContShallowCopy.first)) {
      Error("execute()", "Failed to set original muons links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }

    // Save corrected muons in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_MuonContShallowCopy.first, jobOptions_met_muonColl ) ){
      Error("execute()", "Failed to record %s to TStore.", jobOptions_met_muonColl.c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_MuonContShallowCopy.second,  jobOptions_met_muonColl+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore.", jobOptions_met_muonColl.c_str() );
      return EL::StatusCode::FAILURE;
    }


    //---------------
    // Taus         |
    //---------------
    for( auto TauJetShallowCopyItr = (m_TauJetContShallowCopy.first)->begin(); TauJetShallowCopyItr != (m_TauJetContShallowCopy.first)->end(); TauJetShallowCopyItr++) {


      //tes correction, only sys in MC, nominal shift in data
      //if(debug) std::cout<<"Before tau smearing:"<<(*TauJetShallowCopyItr)->pt()<<std::endl;
      
      if(jobOptions_apply_tau_correction){
	if( m_tauSmearTool->applyCorrection((**TauJetShallowCopyItr)) != CP::CorrectionCode::Ok){
	  Error("execute()","Cannot apply tes correction, eta = %.2f, pt = %.2f. Exiting...", (*TauJetShallowCopyItr)->eta(), (*TauJetShallowCopyItr)->pt());
	  return EL::StatusCode::FAILURE;
	}
      }

      // if(debug) std::cout<<"After tau smearing:"<<(*TauJetShallowCopyItr)->pt()<<std::endl;

      //tau efficiency
      double effSF= 1.0;
      if(IsMC()) m_tauEffTool->getEfficiencyScaleFactor((**TauJetShallowCopyItr), effSF);
      (*TauJetShallowCopyItr)->auxdata< double >( "jet_id_eff_sf" ) = static_cast< double >( effSF );

      //if(debug) std::cout<<"Tau eff SF: "<<effSF<<std::endl;

      //
      //tau selection
      (*TauJetShallowCopyItr)->auxdata< int >( "accepted" ) = static_cast< int >( (m_tauSelTool->accept((*TauJetShallowCopyItr)) ? 1:0) );

      //tau -muon overlap
      (*TauJetShallowCopyItr)->auxdata< int >( "no_muon_overlap" ) = static_cast< int >( ! (m_bt.TauOverlapsWithMuon( *TauJetShallowCopyItr,
                                                                                                                      m_MuonContShallowCopy.first,
                                                                                                                      jobOptions_tau_muon_overal_dR) ? 1:0) );
      //define goodness
      ( *TauJetShallowCopyItr )->auxdata< int >( "good" ) = static_cast< int >( m_bt.Goodness(*TauJetShallowCopyItr, vTauGoodFlags) );

    }

    // Link to original taus needed for METRebuilder
    if( ! xAOD::setOriginalObjectLink(*TauJetCont, *m_TauJetContShallowCopy.first)) { //method is defined in the header file xAODBase/IParticleHelpers.h
      Error("execute()", "Failed to set original tau links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }
    // Save smeared taus in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_TauJetContShallowCopy.first, jobOptions_met_tauColl ) ){
      Error("execute()", "Failed to record %s to TStore.", jobOptions_met_tauColl.c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_TauJetContShallowCopy.second,  jobOptions_met_tauColl+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore.", jobOptions_met_tauColl.c_str() );
      return EL::StatusCode::FAILURE;
    }

    //--------------
    // Jets        |
    //--------------

    double      tmp_jpt    = 0; //tmp

    int ijet(0);
    for(auto JetShallowCopyItr = (m_JetContShallowCopy.first)->begin(); JetShallowCopyItr != (m_JetContShallowCopy.first)->end(); JetShallowCopyItr++){

      tmp_jpt=(*JetShallowCopyItr)->pt();

      // if(debug) std::cout<<"Before jet calibration:"<<(*JetShallowCopyItr)->pt()<<std::endl;
      // apply jet recalibration
      if(jobOptions_apply_jet_recalibration){
        if( m_jetCalibTool->applyCalibration(**JetShallowCopyItr) == CP::CorrectionCode::Error){
          Error("execute()","Cannot apply shallow-copy jet recalibration: run %i event %i", m_runNumber, m_eventNumber);
        }
      }
      //if(debug) std::cout<<"After jet calibration:"<<(*JetShallowCopyItr)->pt()<<std::endl;
      //decorate calibration sf
      ( *JetShallowCopyItr )->auxdata< double >( "jet_CalibSF" ) = static_cast< double >((*JetShallowCopyItr)->pt()/tmp_jpt );

      // apply corrections
      if( IsMC() ){
        //apply JES correction
        if(jobOptions_apply_JES_correction && m_npv> 0){
          if( m_JESUncertaintyTool->applyCorrection(**JetShallowCopyItr) == CP::CorrectionCode::Error ){
            Error("execute()","Cannot apply shallow-copy JES correction: run %i event %i", m_runNumber, m_eventNumber);
          }
        }
	//if(debug) std::cout<<"After JES systematics:"<<(*JetShallowCopyItr)->pt()<<std::endl;
        //apply JER correction, temporary: ask for explicit sys variation
        //if(jobOptions_apply_JER_correction){
	if(jobOptions_apply_JER_correction && nvar==23){
          if( m_JERSmearingTool->applyCorrection(**JetShallowCopyItr) == CP::CorrectionCode::Error ){
            Error("execute()","Cannot apply shallow-copy JER correction: run %i event %i", m_runNumber, m_eventNumber);
          }
        }
	//if(debug) std::cout<<"After jer systematics:"<<(*JetShallowCopyItr)->pt()<<std::endl;
      } // end if MC

      //apply jvf
      float jetvf  = ((*JetShallowCopyItr)->auxdata< std::vector<float> >( "JVF" )).at(0);
      ( *JetShallowCopyItr )->auxdata< int >( "jvf_cut" ) = static_cast< int >((fabs(jetvf)>jobOptions_jet_jvfcut && fabs((*JetShallowCopyItr)->eta())<2.4) ? 1:0);
      // if(debug) std::cout<<"JVF test: "<<jobOptions_jet_jvfcut<<": "<<jetvf<<" -> "<< ( *JetShallowCopyItr )->auxdata< int >( "jvf_cut" )<<std::endl;

      //apply jet cleaning
      ( *JetShallowCopyItr )->auxdata< int >( "clean" ) = static_cast< int >( m_jetCleaning->accept( **JetShallowCopyItr ) ? 1:0);

      //apply jet cuts
      ( *JetShallowCopyItr )->auxdata< int >( "pt_cut" ) = static_cast< int >( (*JetShallowCopyItr)->pt() * MeV2GeV > jobOptions_jet_min_pt ? 1:0 );
      ( *JetShallowCopyItr )->auxdata< int >( "eta_cut" ) = static_cast< int >( std::fabs( (*JetShallowCopyItr)->eta() ) < jobOptions_jet_max_abs_eta );

      //jet -tau overlap
      ( *JetShallowCopyItr )->auxdata< int >( "no_tau_overlap" ) = static_cast< int >( ! (m_bt.JetOverlapsWithGoodTau( *JetShallowCopyItr,
                                                                                                                       m_TauJetContShallowCopy.first,
                                                                                                                       jobOptions_tau_jet_overal_dR) ? 1:0) );
      //define goodness
      ( *JetShallowCopyItr )->auxdata< int >( "good" ) = static_cast< int >( m_bt.Goodness(*JetShallowCopyItr, vJetGoodFlags)  );

      ijet++;
    }//end of loop over the jet cont shallow copy

    // Link to original jets needed for METRebuilder
    if( ! xAOD::setOriginalObjectLink(*JetCont, *m_JetContShallowCopy.first)) {
      Error("execute()", "Failed to set original jet links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }

    // Save calibrated jets in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_JetContShallowCopy.first, jobOptions_met_jetColl ) ){
      Error("execute()", "Failed to record %s to TStore.", jobOptions_met_jetColl.c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_JetContShallowCopy.second,  jobOptions_met_jetColl+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore.", jobOptions_met_jetColl.c_str() );
      return EL::StatusCode::FAILURE;
    }

    //--------------
    // Electrons    |
    //--------------
    if( ! xAOD::setOriginalObjectLink(*EleCont, *m_EleContShallowCopy.first)) {
      Error("execute()", "Failed to set original electrons links -- MET rebuilding cannot proceed.");
      return StatusCode::FAILURE;
    }

    // Save corrected electrons in TStore for METRebuilder : hence must NOT delete them at end
    if( ! m_store->record(m_EleContShallowCopy.first, jobOptions_met_eleColl ) ){
      Error("execute()", "Failed to record %s to TStore.", jobOptions_met_eleColl.c_str());
      return EL::StatusCode::FAILURE;
    }
    if( ! m_store->record(m_EleContShallowCopy.second,  jobOptions_met_eleColl+"Aux.") ){
      Error("execute()", "Failed to record %sAux. to TStore.", jobOptions_met_eleColl.c_str() );
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
        m_ViewElemTauCont->push_back( std::move(*TauJetShallowCopyItr) );       //move: acts as push_back(T&&)  instead of push_back(const T&),
      }
    }
    //pt sort of the "view elements" jet container
    m_bt.PtSort(m_ViewElemTauCont);

    //===========
    // Jets     ||
    //===========

    //select good jets
    for(auto JetShallowCopyItr = (m_JetContShallowCopy.first)->begin(); JetShallowCopyItr != (m_JetContShallowCopy.first)->end(); JetShallowCopyItr++){
      if( (*JetShallowCopyItr)->auxdata< int >("good") ){
        m_ViewElemJetCont->push_back( std::move(*JetShallowCopyItr) );  //move: acts as push_back(T&&)  instead of push_back(const T&),
        // alternative of push_back(static_cast<T&&>(mp)); - emplace_back
      }
    }
    //pt sort of the "view elements" jet container
    m_bt.PtSort(m_ViewElemJetCont);

    //===========
    // Muons    ||
    //===========

    //select good taus
    for(auto MuonShallowCopyItr = (m_MuonContShallowCopy.first)->begin(); MuonShallowCopyItr != (m_MuonContShallowCopy.first)->end(); MuonShallowCopyItr++) {
      if( (*MuonShallowCopyItr)->auxdata< int >("good") )
        m_ViewElemMuonCont->push_back( std::move(*MuonShallowCopyItr) );
    }
    //pt sort of the "view elements" muon container
    m_bt.PtSort(m_ViewElemMuonCont);

    //---------------
    // MET          |
    //---------------

    //retrieve containers from the store - cross-check
    //jets
    const xAOD::JetContainer    *tmpJetCont = 0;
    if( ! m_store->retrieve(tmpJetCont, jobOptions_met_jetColl) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting.", jobOptions_met_jetColl.c_str());
      return EL::StatusCode::FAILURE;
    }
    //taus
    const xAOD::TauJetContainer    *tmpTauCont = 0;
    if( ! m_store->retrieve(tmpTauCont, jobOptions_met_tauColl) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting.", jobOptions_met_tauColl.c_str());
      return EL::StatusCode::FAILURE;
    }
    //muons
    const xAOD::MuonContainer    *tmpMuonCont = 0;
    if( ! m_store->retrieve(tmpMuonCont, jobOptions_met_muonColl) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting.", jobOptions_met_muonColl.c_str());
      return EL::StatusCode::FAILURE;
    }

    //electrons
    const xAOD::ElectronContainer    *tmpEleCont = 0;
    if( ! m_store->retrieve(tmpEleCont, jobOptions_met_eleColl) ){
      Error("execute()", "Failed to retrieve the %s container. Exiting.", jobOptions_met_eleColl.c_str());
      return EL::StatusCode::FAILURE;
    }

    //call METrebuilder
    if( !m_metRebuilder->execute() ){
      Error("execute()", "Failed to execute METRebuilder. Exiting.");
      return EL::StatusCode::FAILURE;
    }

    // auto tmp_JetShallowCopyCont = xAOD::shallowCopyContainer( *tmpEleCont );
    //auto tmp_JetShallowCopyCont = xAOD::shallowCopyContainer( *tmpJetCont );
    //tmp - to check if store collection and default collections match
    /*
      auto def_JetShallowCopyCont = xAOD::shallowCopyContainer( *JetCont );
      for(int i =0; i < def_JetShallowCopyCont.first->size(); i++){

      if( std::fabs( def_JetShallowCopyCont.first->at(i)->pt() - tmpJetCont->at(i)->pt()) > 1e-6 ) {
      Error("execute()", "Jets not matching def pt=%.6f  tmp pt=%.6f  Exiting...", def_JetShallowCopyCont.first->at(i)->pt() , tmpJetCont->at(i)->pt() );
      return EL::StatusCode::FAILURE;
      }

      }
    */

    // Retrieve new MET
    m_MissingETCalibCont = 0;
    if( ! m_store->retrieve(m_MissingETCalibCont, jobOptions_met_outMETCont) ){
      Error("execute()", "Failed to retrieve %s. Exiting...", jobOptions_met_outMETCont.c_str());
      return EL::StatusCode::FAILURE;
    }

    const xAOD::MissingET *MET_Calib = 0;
    const xAOD::MissingET *MET_Calib_SoftTerm = 0;
    const xAOD::MissingET *MET_Calib_JetTerm = 0;
    const xAOD::MissingET *MET_Calib_TauTerm = 0;
    const xAOD::MissingET *MET_Calib_MuonTerm = 0;
    const xAOD::MissingET *MET_Calib_EleTerm = 0;

    if( m_MissingETCalibCont ){
      MET_Calib  = (*m_MissingETCalibCont)[jobOptions_met_outMETTerm];
      if(!MET_Calib){
        Error("execute()", "Null pointer to %s out MET Calib term", jobOptions_met_outMETTerm.c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_SoftTerm = (*m_MissingETCalibCont)[jobOptions_met_softTerm];
      if(!MET_Calib_SoftTerm){
        Error("execute()", "Null pointer to MET Calib %s term", jobOptions_met_softTerm.c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_JetTerm = (*m_MissingETCalibCont)[jobOptions_met_jetTerm];
      if(!MET_Calib_JetTerm){
        Error("execute()", "Null pointer to MET Calib %s term", jobOptions_met_jetTerm.c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_TauTerm = (*m_MissingETCalibCont)[jobOptions_met_tauTerm];
      if(!MET_Calib_TauTerm){
        Error("execute()", "Null pointer to MET Calib %s term", jobOptions_met_tauTerm.c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_MuonTerm = (*m_MissingETCalibCont)[jobOptions_met_muonTerm];
      if(!MET_Calib_MuonTerm){
        Error("execute()", "Null pointer to MET Calib %s term", jobOptions_met_muonTerm.c_str());
        return EL::StatusCode::FAILURE;
      }

      MET_Calib_EleTerm = (*m_MissingETCalibCont)[jobOptions_met_eleTerm];
      if(!MET_Calib_EleTerm){
        Error("execute()", "Null pointer to MET Calib %s term", jobOptions_met_eleTerm.c_str());
        return EL::StatusCode::FAILURE;
      }

    }else{
      Error("execute()", "Null pointer to MissingETCalib container");
      return EL::StatusCode::FAILURE;
    }

    //=============
    // Fill Trees ||
    //=============

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
      FillTreeVar("leading_tau_met_mt", m_bt.MT( m_ViewElemTauCont->at(0), MET_Calib) );

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
      FillTreeVar("leading_muon_met_mt", m_bt.MT( m_ViewElemMuonCont->at(0), MET_Calib) );
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
      FillTreeVar("tau_muon_deta", m_bt.DeltaEta(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) );
      FillTreeVar("tau_muon_dphi", m_bt.DeltaPhi(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) );
      FillTreeVar("tau_muon_dR", m_bt.DeltaR(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) );
      FillTreeVar("tau_muon_cosalpha", m_bt.CosAlpha(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) );
      FillTreeVar("tau_muon_qxq", m_ViewElemTauCont->at(0)->charge() * m_ViewElemMuonCont->at(0)->charge() );
      FillTreeVar("tau_muon_m_vis", m_bt.MassVisible(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) * MeV2GeV);
      FillTreeVar("tau_muon_vect_sum_pt", m_bt.VectorSumPt(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) * MeV2GeV );
      FillTreeVar("tau_muon_scal_sum_pt", m_bt.ScalarSumPt(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0) * MeV2GeV);
      FillTreeVar("tau_muon_met_bisect", m_bt.METbisect(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0, MET_Calib) );
      FillTreeVar("tau_muon_met_min_dphi", m_bt.METmindeltaphi(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0, MET_Calib) );
      FillTreeVar("tau_muon_met_centrality", m_bt.METcentrality(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0, MET_Calib) );
      double x1_col; double x2_col; double m_col; bool ok_col = m_bt.MassCollinear(m_ViewElemTauCont, 0, m_ViewElemMuonCont, 0, MET_Calib, true, m_col, x1_col, x2_col) ;
      FillTreeVar("tau_muon_col", ok_col);
      FillTreeVar("tau_muon_m_col", m_col * MeV2GeV);
      FillTreeVar("tau_muon_x1_col", x1_col);
      FillTreeVar("tau_muon_x2_col", x2_col);

    }

    // dimuon system
    if(m_ViewElemMuonCont->size() > 1){
      FillTreeVar("dimuon", 1. );
      FillTreeVar("dimuon_deta", m_bt.DeltaEta(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_dphi", m_bt.DeltaPhi(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_dR", m_bt.DeltaR(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_cosalpha", m_bt.CosAlpha(m_ViewElemMuonCont) );
      FillTreeVar("dimuon_qxq", m_ViewElemMuonCont->at(0)->charge() * m_ViewElemMuonCont->at(1)->charge());
      FillTreeVar("dimuon_m_vis", m_bt.MassVisible(m_ViewElemMuonCont) * MeV2GeV);
      FillTreeVar("dimuon_vect_sum_pt", m_bt.VectorSumPt(m_ViewElemMuonCont) * MeV2GeV );
      FillTreeVar("dimuon_scal_sum_pt", m_bt.ScalarSumPt(m_ViewElemMuonCont) * MeV2GeV);
      FillTreeVar("dimuon_met_bisect", m_bt.METbisect(m_ViewElemMuonCont, MET_Calib) );
      FillTreeVar("dimuon_met_min_dphi", m_bt.METmindeltaphi(m_ViewElemMuonCont, MET_Calib) );
      FillTreeVar("dimuon_met_centrality", m_bt.METcentrality(m_ViewElemMuonCont, MET_Calib) );
      double x1_col; double x2_col; double m_col; bool ok_col = m_bt.MassCollinear(m_ViewElemMuonCont, MET_Calib, true, m_col, x1_col, x2_col) ;
      FillTreeVar("dimuon_col", ok_col);
      FillTreeVar("dimuon_m_col", m_col * MeV2GeV);
      FillTreeVar("dimuon_x1_col", x1_col);
      FillTreeVar("dimuon_x2_col", x2_col);

    }

    // ditau system
    if(m_ViewElemTauCont->size() > 1){
      FillTreeVar("ditau", 1. );
      FillTreeVar("ditau_deta", m_bt.DeltaEta(m_ViewElemTauCont) );
      FillTreeVar("ditau_dphi", m_bt.DeltaPhi(m_ViewElemTauCont) );
      FillTreeVar("ditau_dR", m_bt.DeltaR(m_ViewElemTauCont) );
      FillTreeVar("ditau_cosalpha", m_bt.CosAlpha(m_ViewElemTauCont) );
      FillTreeVar("ditau_qxq", m_ViewElemTauCont->at(0)->charge() * m_ViewElemTauCont->at(1)->charge());
      FillTreeVar("ditau_m_vis", m_bt.MassVisible(m_ViewElemTauCont) * MeV2GeV);
      FillTreeVar("ditau_vect_sum_pt", m_bt.VectorSumPt(m_ViewElemTauCont) * MeV2GeV );
      FillTreeVar("ditau_scal_sum_pt", m_bt.ScalarSumPt(m_ViewElemTauCont) * MeV2GeV);
      FillTreeVar("ditau_met_bisect", m_bt.METbisect(m_ViewElemTauCont, MET_Calib) );
      FillTreeVar("ditau_met_min_dphi", m_bt.METmindeltaphi(m_ViewElemTauCont, MET_Calib) );
      FillTreeVar("ditau_met_centrality", m_bt.METcentrality(m_ViewElemTauCont, MET_Calib) );
      double x1_col; double x2_col; double m_col; bool ok_col = m_bt.MassCollinear(m_ViewElemTauCont, MET_Calib, true, m_col, x1_col, x2_col) ;
      FillTreeVar("ditau_col", ok_col);
      FillTreeVar("ditau_m_col", m_col * MeV2GeV);
      FillTreeVar("ditau_x1_col", x1_col);
      FillTreeVar("ditau_x2_col", x2_col);

    }

    //dijet system
    if(m_ViewElemJetCont->size() > 1){
      FillTreeVar("dijet", 1. );
      FillTreeVar("dijet_deta", m_bt.DeltaEta(m_ViewElemJetCont) );
      FillTreeVar("dijet_dy", m_bt.DeltaRapidity(m_ViewElemJetCont) );
      FillTreeVar("dijet_dphi", m_bt.DeltaPhi(m_ViewElemJetCont) );
      FillTreeVar("dijet_dR", m_bt.DeltaR(m_ViewElemJetCont) );
      FillTreeVar("dijet_cosalpha", m_bt.CosAlpha(m_ViewElemJetCont) );
      FillTreeVar("dijet_m_vis", m_bt.MassVisible(m_ViewElemJetCont) * MeV2GeV);
      FillTreeVar("dijet_vect_sum_pt", m_bt.VectorSumPt(m_ViewElemJetCont) * MeV2GeV);
      FillTreeVar("dijet_scal_sum_pt", m_bt.ScalarSumPt(m_ViewElemJetCont) * MeV2GeV);
      FillTreeVar("dijet_etaxeta", m_ViewElemJetCont->at(0)->eta() * m_ViewElemJetCont->at(1)->eta());

    }

    //fill tree
    if ( m_Tree.find(systName) != m_Tree.end() )
      m_Tree[systName]->Fill();
    else
      Error("execute()", "Cannot fill tree with name '%s'", systName.c_str());


    //##################################################### FillHistos ############################################################

    if( jobOptions_doHistos){

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
    if(m_ViewElemMuonCont->size() > 1) FillH1("muons_mll_0_1"       ,systname,m_bt.MassVisible(m_ViewElemMuonCont,0,m_ViewElemMuonCont,1)*MeV2GeV, m_evtw);
    if(m_ViewElemMuonCont->size() > 1 && fabs(m_bt.MassVisible(m_ViewElemMuonCont,0,m_ViewElemMuonCont,1)*MeV2GeV-91.)<20. ) FillH1("muons_met_zpeak",systname , MET_Calib->met() * MeV2GeV, m_evtw);
    


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


    if(m_ViewElemJetCont->size()>1) FillH1("jet_rapgap"  ,systname, m_bt.DeltaEta((*m_ViewElemJetCont)[0],(*m_ViewElemJetCont)[1]) , m_evtw);
    //if(m_ViewElemJetCont->size()>1) FillH1("jet_rapgap"  , m_bt.DeltaEta(m_ViewElemJetCont , m_evtw);
    

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

    //clear deep copy containers: Clearing container deletes contents
    // nothing at the moment

    //clear copy VIEW containers:  clearing view container DOES NOT delete contents
    m_ViewElemJetCont->clear();
    m_ViewElemTauCont->clear();
    m_ViewElemMuonCont->clear();

    //clear shallow copies: clearing container deletes contents ncluding AuxStore
    // jet container shallpw copy saved in TStore so NOT deleted
    //JetContShallowCopy->clear();

    //delete copy containers
    //delete JetContShallowCopy.first; delete JetContShallowCopy.second;
    //delete TauJetContShallowCopy.first; delete TauJetContShallowCopy.second;
    //delete MuonContShallowCopy.first; delete MuonContShallowCopy.second;

    // Clear transient store
    m_store->clear();

    nvar++;
  } // end check that systematic applied ok

  //-------------------
  // End Systematics  |
  //-------------------  


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


  Info( "finalize()","Finished processing %i events", m_eventCounter);
  Info("finalize()", "Number of clean events = %i", m_numCleanEvents);
  Info("finalize()", "Number of weighted events = %f", m_numWeightedEvents);

  // Write PU-configfiles
  // name of file defined in setupJob()
  if(jobOptions_createPUfile) m_pileup->WriteToFile(wk()->getOutputFile("PileUpReweighting"));

  if( m_grl ) {
    delete m_grl;
    m_grl = 0;
  }
  if(m_pileup){
    delete m_pileup;
    m_pileup=0;
  }

  if( m_jetCleaning ) {
    delete m_jetCleaning;
    m_jetCleaning = 0;
  }

  if(m_JERTool){
    delete m_JERTool;
    m_JERTool = 0;
  }

  if(m_JERSmearingTool){
    delete m_JERSmearingTool;
    m_JERSmearingTool = 0;
  }

  //deletion of calibtool gives segfault
  //  if(m_jetCalibTool){
  //      delete m_jetCalibTool;
  //      m_jetCalibTool=0;
  //   }
  if(m_tauSelTool){
    delete  m_tauSelTool;
    m_tauSelTool=0;
  }
  if(m_tauSmearTool){
    delete m_tauSmearTool;
    m_tauSmearTool=0;
  }
  if(m_tauEffTool){
    delete m_tauEffTool;
    m_tauEffTool=0;
  }
  if(m_muonCalibrationAndSmearingTool){
    delete m_muonCalibrationAndSmearingTool;
    m_muonCalibrationAndSmearingTool = 0;
  }

  if(m_muonSelectionTool){
    delete m_muonSelectionTool;
    m_muonSelectionTool = 0;
  }

  if(m_muonEfficiencyScaleFactorsTool){
    delete m_muonEfficiencyScaleFactorsTool;
    m_muonEfficiencyScaleFactorsTool = 0;
  }

  if(m_store){
    delete m_store;
    m_store = 0;
  }

  if(m_metRebuilder){
    delete m_metRebuilder;
    m_metRebuilder = 0;
  }

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
  //event information histograms
  int N_int_pbc = 100; double x_int_pbc = 0; double y_int_pbc = 100;
  int N_pu_w = 2; double x_pu_w = 0; double y_pu_w = 200;

  m_H1["event_weight"] = new TH1D("event_weight","event_weight", 200, -10, 10);
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

void MainAnalysis::setJobOptions(FILE *f, Int_t &target, const char *label)
{
  char line[256],sub1[256],sub2[256];
  Int_t temp;
  fseek(f,0,SEEK_SET);

  while (1) {
    fgets(line,256,f);
    if (feof(f)) break;
    if (sscanf(line,"%s = %s",sub1,sub2) != 2) continue;
    if (strcmp(sub1,label) != 0) continue;
    sscanf(sub2,"%d",&temp);
    std::cout << sub1 <<": " << temp << "  (default: "<< target << ")"<<std::endl;
    target = temp;
  }

}

void MainAnalysis::setJobOptions(FILE *f, std::string &target, const char *label)
{
  char line[256],sub1[256],sub2[256];
  char temp[256];
  fseek(f,0,SEEK_SET);
  while (1) {
    fgets(line,256,f);
    if (feof(f)) break;
    if (sscanf(line,"%s = %s",sub1,sub2) != 2) continue;
    if (strcmp(sub1,label) != 0) continue;
    sscanf(sub2,"%s",temp);
    std::cout << sub1 <<": " << temp << "  (default: "<< target << ")"<<std::endl;
    target = temp;
  }
}

void MainAnalysis::setJobOptions(FILE *f, Double_t &target, const char *label)
{
  char line[256],sub1[256],sub2[256];
  Double_t temp;
  fseek(f,0,SEEK_SET);
  while (1) {
    fgets(line,256,f);
    if (feof(f)) break;
    if (sscanf(line,"%s = %s",sub1,sub2) != 2) continue;
    if (strcmp(sub1,label) != 0) continue;
    sscanf(sub2,"%lf",&temp);
    std::cout << sub1 <<": " << temp << "  (default: "<< target << ")"<<std::endl;
    target = temp;
  }
}

void MainAnalysis::setJobOptions(FILE *f, Bool_t &target, const char *label)
{
  char line[256],sub1[256],sub2[256];
  Int_t temp;
  fseek(f,0,SEEK_SET);

  while (1) {
    fgets(line,256,f);
    if (feof(f)) break;
    if (sscanf(line,"%s = %s",sub1,sub2) != 2) continue;
    if (strcmp(sub1,label) != 0) continue;
    sscanf(sub2,"%d",&temp);
    std::cout << sub1 <<": " << temp << "  (default: "<< target << ")"<<std::endl;
    target = temp;
  }
  if(temp != 0) target = kTRUE;
  else target = kFALSE;
}

