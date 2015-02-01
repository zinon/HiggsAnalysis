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
#include "TTreeFormula.h"

//=============
// definitions
//=============
#define MeV2GeV 1e-3
#define GeV2MeV 1e3


class MainAnalysis : public EL::Algorithm
{

 public:

  // this is a standard constructor
  MainAnalysis ();

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker node (done by the //!)

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
  
  double m_initialSumOfWeights; //!
  double m_finalSumOfWeights; //!
  
  int m_eventNumber; //!
  int m_runNumber; //!
  int m_rndRunNumber; //!
  int m_mcChannelNumber; //!

  //instances of classes

  xAOD::TEvent *m_event;  //!

#ifndef __CINT__
  // Variables should be protected from CINT using //!

#endif // not __CINT__

 private:
  
  std::string m_job_options_file;



  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
 public:

  std::map<std::string, TH1*> m_H1; //!
  std::map<std::string, TH2*> m_H2; //!

  std::map<std::string, TTree*> m_Tree; //!

  void JobOptionsFile(const std::string &s){ m_job_options_file = s; }

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

  // these are the functions inherited from Algorithm - don't touch
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
