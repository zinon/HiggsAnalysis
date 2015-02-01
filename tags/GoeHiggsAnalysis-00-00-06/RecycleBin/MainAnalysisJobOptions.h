#ifndef MainAnalysis_MainAnalysisJobOptions_H
#define MainAnalysis_MainAnalysisJobOptions_H

#include <string>

class MainAnalysisJobOptions {


 public:


  //C++11 allows non-static data members to be initialized where it is declared (in their class)

  //jets
  std::string jobOptions_jet_collection {"AntiKt4LCTopo"};
  double jobOptions_jet_min_pt = 18.;
  double jobOptions_jet_max_abs_eta = 4.5;
  bool jobOptions_apply_JES_correction = 1;
  bool jobOptions_apply_JER_correction = 1;
  std::string jobOptions_jet_cleaning {"VeryLooseBad"}; //VeryLooseBad MediumBad;
  bool jobOptions_apply_jet_recalibration = 1;
  double jobOptions_jet_jvfcut = -1;
  double jobOptions_presel_jet_skipCaliblowpt = 1;

  //taus
  double jobOptions_tau_jet_overal_dR = 0.2;
  double jobOptions_tau_muon_overal_dR = 0.2;
  bool jobOptions_tau_selection_recommended = 1;
  bool jobOptions_apply_tau_correction = 1;
  double jobOptions_tau_min_pt = 20.0;
  std::string jobOptions_tau_jet_bdt = "JETIDBDTLOOSE";
  std::string jobOptions_tau_ele_bdt = "ELEIDBDTLOOSE";

  //muons
  bool jobOptions_apply_muon_calibration_and_smearing = 1;
  double jobOptions_muon_min_pt = 20.0;
  double jobOptions_muon_max_iso = 0.1;
  double jobOptions_muon_OR_min_pt = 2.0;
  double jobOptions_muon_OR_max_abs_eta = 2.5;

  //met
  std::string jobOptions_met_container = "MET_RefFinal";
  std::string jobOptions_met_jetColl = "AntiKt4LCTopoCalibJets";
  std::string jobOptions_met_muonColl = "CalibMuons";
  std::string jobOptions_met_tauColl = "CalibTaus";
  std::string jobOptions_met_eleColl = "CalibElectrons";
  std::string jobOptions_met_outMETCont = "MET_Calib";
  std::string jobOptions_met_outMETTerm = "Final";
  std::string jobOptions_met_eleTerm = "RefEle";
  std::string jobOptions_met_gammaTerm = "RefGamma";
  std::string jobOptions_met_tauTerm = "RefTau";
  std::string jobOptions_met_jetTerm = "RefJet"; //RefJet RefJet_JVFCut
  std::string jobOptions_met_muonTerm = "Muons";
  std::string jobOptions_met_softTerm = "SoftClus"; // SoftClus PVSoftTrk
  double jobOptions_met_jetPtCut = 0.0; //GeV
  bool jobOptions_met_jetDoJvf = 0;

  //general
  bool jobOptions_createPUfile = 0;
  bool jobOptions_doPileupReweighting = 0;
  bool jobOptions_doSystematics = 0;
  bool jobOptions_runGrid = 0;
  bool jobOptions_testRun = 0;
  bool jobOptions_doHistos = 0;

  bool jobOptions_dummy = 0;

};

#endif
