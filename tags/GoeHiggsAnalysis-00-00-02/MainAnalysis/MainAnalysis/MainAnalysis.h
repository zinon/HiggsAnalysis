#ifndef MainAnalysis_MainAnalysis_H
#define MainAnalysis_MainAnalysis_H

#include <EventLoop/Algorithm.h>

#include <MainAnalysis/MainAnalysisJobOptions.h>

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

class MainAnalysis : public EL::Algorithm, public MainAnalysisJobOptions
{

 public:

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker node (done by the //!)

  xAOD::TEvent *m_event;  //!

  //put all these counts in a Counter class
  int m_eventCounter; //!
  int m_numGrlEvents; //!
  int m_numCleanEvents; //!
  double  m_numWeightedEvents; //!

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
  //std::map<std::string, std::unique_ptr<TTree> > m_Tree1;

  // Tree *myTree; //!
  // TH1 *myHist; //!

  //std::shared_ptr<TFile> fd;
  //HistoManager *m_hm; //!

  // this is a standard constructor
  MainAnalysis ();

  //user defined functions  
  void setJobOptions(FILE *, Int_t &, const char *);
  void setJobOptions(FILE *, std::string &, const char *);
  void setJobOptions(FILE *, Double_t &, const char *);
  void setJobOptions(FILE *, Bool_t &, const char *);

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
