

#include <xJobOptions/xJobOptions.h>

bool xJobOptions::Configure(){
  FILE *jobOptFile = fopen( m_filename.c_str(), "rt");
  
  if (!jobOptFile) {
    std::cerr << "xJobOptions::Configure() - this JobOptionFile does not want to be read: " << m_filename << std::endl;
    return false;
  }

  //try, throw and catch here
  try{

    setJobOptions(jobOptFile, mapRef(DO_PILEUP_FILE), "createPUfile");
    setJobOptions(jobOptFile, mapRef(DO_TEST_RUN), "testRun");
    setJobOptions(jobOptFile, mapRef(DO_PILEUP_REWEIGHTING), "doPileupReweighting");
    setJobOptions(jobOptFile, mapRef(DO_SYSTEMATICS), "doSystematics");
    setJobOptions(jobOptFile, mapRef(DO_HISTOS), "doHistos");
    setJobOptions(jobOptFile, mapRef(DO_DERIVATION), "isDerivation");
  
    setJobOptions(jobOptFile, mapRef(JET_COLLECTION), "jet_collection");
    setJobOptions(jobOptFile, mapRef(JET_MIN_PT), "jet_min_pt");
    setJobOptions(jobOptFile, mapRef(JET_MAX_ABS_ETA), "jet_max_abs_eta");
    setJobOptions(jobOptFile, mapRef(JET_MIN_ABS_JVF), "jet_min_abs_jvf"); 
    setJobOptions(jobOptFile, mapRef(JET_JVF_CUT_MAX_ABS_ETA), "jet_jvf_cut_max_abs_eta"); 
    setJobOptions(jobOptFile, mapRef(JET_RECALIBRATION), "apply_jet_recalibration");
    setJobOptions(jobOptFile, mapRef(JET_JES_CORRECTION), "apply_JES_correction");
    setJobOptions(jobOptFile, mapRef(JET_JER_CORRECTION), "apply_JER_correction");
    setJobOptions(jobOptFile, mapRef(JET_CLEANING), "jet_cleaning");
    setJobOptions(jobOptFile, mapRef(JET_PRESELECT_SKIP_CALIB_LOW_PT), "presel_jet_skipCaliblowpt");
    setJobOptions(jobOptFile, mapRef(JET_PRESELECT_CALIB_MIN_PT), "presel_jet_calib_min_pt");
    
    setJobOptions(jobOptFile, mapRef(TAU_SELECTION_RECOMMENDED), "tau_selection_recommended");
    setJobOptions(jobOptFile, mapRef(TAU_MIN_PT), "tau_min_pt");
    setJobOptions(jobOptFile, mapRef(TAU_JET_BDT), "tau_jet_bdt");
    setJobOptions(jobOptFile, mapRef(TAU_ELE_BDT), "tau_ele_bdt");
    setJobOptions(jobOptFile, mapRef(TAU_JET_OVERLAP_DR), "tau_jet_overal_dR");
    setJobOptions(jobOptFile, mapRef(TAU_MUON_OVERLAP_DR), "tau_muon_overal_dR");
    setJobOptions(jobOptFile, mapRef(TAU_ELE_OVERLAP_DR), "tau_ele_overal_dR");
    setJobOptions(jobOptFile, mapRef(TAU_CORRECTION), "apply_tau_correction");
    
    setJobOptions(jobOptFile, mapRef(MUON_MIN_PT), "muon_min_pt");
    setJobOptions(jobOptFile, mapRef(MUON_MAX_ISO), "muon_max_iso");
    setJobOptions(jobOptFile, mapRef(MUON_OVERLAP_MIN_PT), "muon_OR_min_pt");
    setJobOptions(jobOptFile, mapRef(MUON_OVERLAP_MAX_ABS_ETA), "muon_OR_max_abs_eta");
    setJobOptions(jobOptFile, mapRef(MUON_CALIBRATION_SMEARING), "apply_muon_calibration_and_smearing");
    setJobOptions(jobOptFile, mapRef(MUON_JET_OVERLAP_DR), "muon_jet_overal_dR");
    
    setJobOptions(jobOptFile, mapRef(MET_CONTAINER), "met_container");
    setJobOptions(jobOptFile, mapRef(MET_JETCOL), "met_jetColl");
    setJobOptions(jobOptFile, mapRef(MET_TAUCOL), "met_tauColl");
    setJobOptions(jobOptFile, mapRef(MET_MUONCOL), "met_muonColl");
    setJobOptions(jobOptFile, mapRef(MET_ELECOL), "met_eleColl");
    setJobOptions(jobOptFile, mapRef(MET_JETTERM), "met_jetTerm");
    setJobOptions(jobOptFile, mapRef(MET_TAUTERM), "met_tauTerm");
    setJobOptions(jobOptFile, mapRef(MET_MUONTERM), "met_muonTerm");
    setJobOptions(jobOptFile, mapRef(MET_ELETERM), "met_eleTerm");
    setJobOptions(jobOptFile, mapRef(MET_SOFTTERM), "met_softTerm");
    setJobOptions(jobOptFile, mapRef(MET_GAMMATERM), "met_gammaTerm");
    setJobOptions(jobOptFile, mapRef(MET_OUTMETTERM), "met_outMETTerm");
    setJobOptions(jobOptFile, mapRef(MET_OUTMETCONT), "met_outMETCont");
    setJobOptions(jobOptFile, mapRef(MET_JETPTCUT), "met_jetPtCut");
    setJobOptions(jobOptFile, mapRef(MET_JETDOJVF), "met_jetDoJvf");
    
    //DAN: electron JO
    setJobOptions(jobOptFile, mapRef(ELE_ID                  ), "ele_id");
    setJobOptions(jobOptFile, mapRef(ELE_PT                  ), "ele_pt");
    setJobOptions(jobOptFile, mapRef(ELE_ETA_CRACK_LO        ), "ele_eta_crack_lo");
    setJobOptions(jobOptFile, mapRef(ELE_ETA_CRACK_HI        ), "ele_eta_crack_hi");
    setJobOptions(jobOptFile, mapRef(ELE_ETA_MAX             ), "ele_eta_max");
    setJobOptions(jobOptFile, mapRef(ELE_ISO_ETCONE20        ), "ele_iso_etcone20");
    setJobOptions(jobOptFile, mapRef(ELE_ISO_PTCONE40        ), "ele_iso_ptcone40");
    setJobOptions(jobOptFile, mapRef(ELE_CALIBRATION_SMEARING), "apply_ele_calibration_and_smearing");
    setJobOptions(jobOptFile, mapRef(ELE_MUON_OVERLAP_DR     ), "ele_muon_overlap_dr");
    setJobOptions(jobOptFile, mapRef(ELE_ID_FOR_TAUOVERLAP   ), "ele_id_for_tauoverlap");
    setJobOptions(jobOptFile, mapRef(ELE_JET_OVERLAP_DR      ), "ele_jet_overal_dR");
    

    //  setJobOptions(jobOptFile, mapRef(), "");

  }
  
  catch (std::exception& e) {
    std::cout << e.what() << '\n';
  }



  return true;
}

const std::string & xJobOptions::mapConstRef(const OptionType& t) const  {

const auto iter = m_options.find(t);

  if (iter != m_options.end() ){
    return iter->second;}

  std::clog<<"xJobOptions::mapConstRef : "<<t<<std::endl;

  throw mapexc;
}


std::string & xJobOptions::mapRef(const OptionType& t) {

  auto iter = m_options.find(t);

  if (iter != m_options.end() ){
    return iter->second;}

  std::clog<<"xJobOptions::mapRef : "<<t<<std::endl;

  throw mapexc;
}


void xJobOptions::setJobOptions(FILE *f, std::string &target, const char *label){ //JobOption setting for string inputs

  char line[256],sub1[256],sub2[256]; char temp[256];
  fseek(f,0,SEEK_SET);
  while (1) {
    fgets(line,256,f);
    if (feof(f)) break;
    if (sscanf(line,"%s = %s",sub1,sub2) != 2) continue;
    if (strcmp(sub1,label) != 0) continue;
    sscanf(sub2,"%s",temp);
    std::cout <<"xJobOptions::setJobOptions "<< sub1 <<" : " << temp << " (default: "<< target << ")"<<std::endl;
    target = temp;
  }

}

auto xJobOptions::TauJetBDTindex(const std::string &str) const -> TauAnalysisTools::e_JETID {

  if (str.find("JETIDNONE") != std::string::npos) 
    return TauAnalysisTools::JETIDNONE;
  else if (str.find("JETIDBDTLOOSE") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTLOOSE;
  else if (str.find("JETIDBDTMEDIUM") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTMEDIUM;
  else if (str.find("JETIDBDTTIGHT") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTTIGHT;
  else if (str.find("JETIDBDTFAIL") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTFAIL;
  else if (str.find("JETIDBDTOTHER") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTOTHER;
  else if (str.find("JETIDLLHLOOSE") != std::string::npos) 
    return TauAnalysisTools::JETIDLLHLOOSE;
  else if (str.find("JETIDLLHMEDIUM") != std::string::npos) 
    return TauAnalysisTools::JETIDLLHMEDIUM;
  else if (str.find("JETIDLLHTIGHT") != std::string::npos) 
    return TauAnalysisTools::JETIDLLHTIGHT;
  else if (str.find("JETIDLLHFAIL") != std::string::npos) 
    return TauAnalysisTools::JETIDLLHFAIL;
  else if (str.find("JETIDBDTLOOSENOTMEDIUM") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTLOOSENOTMEDIUM;
  else if (str.find("JETIDBDTLOOSENOTTIGHT") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTLOOSENOTTIGHT;
  else if (str.find("JETIDBDTMEDIUMNOTTIGHT") != std::string::npos) 
    return TauAnalysisTools::JETIDBDTMEDIUMNOTTIGHT;
  else
    Warning("TauAnalysisTools::e_JETID xJobOptions::TauJetBDTindex()", "Unable to return value for option %s", str.c_str() );

  return TauAnalysisTools::JETIDNONE;

}

auto xJobOptions::TauEleBDTindex(const std::string &str) const -> TauAnalysisTools::e_ELEID {
 
  if (str.find("ELEIDNONE") != std::string::npos) 
    return TauAnalysisTools::ELEIDNONE;
  else if (str.find("ELEIDBDTLOOSE") != std::string::npos) 
    return TauAnalysisTools::ELEIDBDTLOOSE;
  else if (str.find("ELEIDBDTMEDIUM") != std::string::npos) 
    return TauAnalysisTools::ELEIDBDTMEDIUM;
  else if (str.find("ELEIDBDTTIGHT") != std::string::npos) 
    return TauAnalysisTools::ELEIDBDTTIGHT;
  else if (str.find("ELEIDOTHER") != std::string::npos) 
    return TauAnalysisTools::ELEIDOTHER;
  else
    Warning("TauAnalysisTools::e_ELEID xJobOptions::TauEleBDTindex()", "Unable to return value for option %s", str.c_str() );
  
  return TauAnalysisTools::ELEIDNONE;
}

