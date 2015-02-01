#ifndef MainAnalysis_MainAnalysis_H
#define MainAnalysis_MainAnalysis_H

#include <EventLoop/Algorithm.h>

//xAOD access
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

//=============
//ROOT includes
//=============
#include "TH1.h"
#include "TH1D.h"

#include "TH2.h"
#include "TH2D.h"

#include "TLorentzVector.h"
//=============
// definitions
//=============
#define MeV2GeV 1e-3 
#define GeV2MeV 1e3 

class MainAnalysis : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.

 public:
  /* struct jobOptions_t {

     double jet_min_pt; //!

   } _jobOptions; //!
  
   jobOptions_t *jobOptions;
  */
   //    jobOptions_t jobOptions;

  double jobOptions_jet_min_pt;
  double jobOptions_jet_max_abs_eta;
  bool jobOptions_apply_JES_correction;
  bool jobOptions_apply_JER_correction;
  std::string jobOptions_jet_cleaning;
  bool jobOptions_apply_jet_recalibration;

  double jobOptions_tau_jet_overal_dR;
  double jobOptions_tau_muon_overal_dR;

  bool jobOptions_tau_selection_recommended;
  bool jobOptions_apply_tau_correction;
  double jobOptions_tau_min_pt;
  std::string jobOptions_tau_jet_bdt;
  std::string jobOptions_tau_ele_bdt;

  bool jobOptions_apply_muon_calibration_and_smearing;
  double jobOptions_muon_min_pt; 
  double jobOptions_muon_max_iso;

  std::string jobOptions_met_container;
  std::string jobOptions_met_jetColl;
  std::string jobOptions_met_muonColl;
  std::string jobOptions_met_tauColl;
  std::string jobOptions_met_eleColl;
  std::string jobOptions_met_outMETCont;
  std::string jobOptions_met_outMETTerm;
  std::string jobOptions_met_eleTerm;
  std::string jobOptions_met_gammaTerm;
  std::string jobOptions_met_tauTerm;
  std::string jobOptions_met_jetTerm;
  std::string jobOptions_met_muonTerm;
  std::string jobOptions_met_softTerm;
  double jobOptions_met_jetPtCut;
  bool jobOptions_met_jetDoJvf;


  
  
  bool jobOptions_createPUfile;
  bool jobOptions_runGrid;
  bool jobOptions_testRun;

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)

  xAOD::TEvent *m_event;  //!

  //put all these counts in a Counter class
  int m_eventCounter; //!
  int m_numGrlEvents; //!
  int m_numCleanEvents; //!

  int m_numGoodJets; //!
  int m_numGoodTaus; //!

  int m_entries;
  bool m_isMC; //!
  double m_evtw; //!
  double m_puw; //!
  double m_mcw; //!
  int m_eventNumber; //!
  int m_runNumber; //!
  int m_rndRunNumber; //!
  int m_mcChannelNumber; //!


  //instances of tools
#ifndef __CINT__
  // Variables should be protected from CINT using //!
  
  //  GoodRunsListSelectionTool *m_grl; //!
   
  //TauAnalysisTools::TauSelectionTool             *m_tauSelTool; //!
  //TauAnalysisTools::TauEfficiencyCorrectionsTool *m_tauEffTool; //!
  //  TauAnalysisTools::TauSmearingTool              *m_tauSmearTool; //!
  
#endif // not __CINT__

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
 public:
  std::map<std::string, TH1*> m_H1; //!
  std::map<std::string, TH2*> m_H2; //!

  std::map<std::string, TTree*> m_Tree; //!
  //std::map<std::string, std::shared_ptr< TTree > > m_Tree; //!

  // Tree *myTree; //!
  // TH1 *myHist; //!

  //HistoManager *m_hm; //!

  // this is a standard constructor
  MainAnalysis ();

  //user defined functions
  //readers and setters
//  template <FILE *f, typename T = double, const char * label >
//    void SetJobOptions(T option);
  
  void setJobOptions(FILE *, Int_t &, const char *);
  void setJobOptions(FILE *, std::string &, const char *);
  void setJobOptions(FILE *, Double_t &, const char *);
  void setJobOptions(FILE *, Bool_t &, const char *);

  //getters
  bool IsMC() const;
  bool IsData() const;

  //histogram fill
  void FillH1(const std::string &,const std::string &,double );
  void FillH1w(const std::string &,const std::string &, double);
  void FillH1(const std::string &,const std::string &, double, double);
  
  void FillH2(const std::string &, const std::string &, double, double);
  void FillH2w(const std::string &, const std::string &, double, double);
  void FillH2(const std::string &, const std::string &, double, double,  double);


  //fill tree helper function
  void FillTreeVar(const std::string &, const double);

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(MainAnalysis, 1);
};

#endif
