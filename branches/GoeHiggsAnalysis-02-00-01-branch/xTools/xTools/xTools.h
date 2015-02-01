#ifndef xTools_xTools_H
#define xTools_xTools_H
// Toolbox for self-defined, hand-polished methods of pure awesomeness


//=============
//STD includes
//=============
#include <iostream>
#include <string>

//=============
//STL includes
//=============
#include <vector>
#include <iterator>

//=============
//ROOT includes
//=============
#include "TLorentzVector.h"
#include "TMatrixD.h"
#include "TMath.h"

//============
//EDM includes
//============
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODMissingET/MissingETAuxContainer.h"

#include "TauAnalysisTools/Enums.h"

//============
//ASG includes
//============
#include "AsgTools/AsgTool.h"


class xTools {

 public: //methods

  // this is a standard constructor
  xTools ();
  virtual ~xTools () = default; //auto generated


 public: //variables
  bool Goodness(const xAOD::IParticle *, std::vector<std::string> &);

  //distances
  double DeltaR(const xAOD::IParticle *, const xAOD::IParticle *);
  double DeltaR(const xAOD::IParticleContainer *, unsigned int, unsigned int);
  double DeltaR(const xAOD::IParticleContainer *);
  double DeltaR(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);


  double DeltaEta(const xAOD::IParticle *, const xAOD::IParticle *);
  double DeltaEta(const xAOD::IParticleContainer *, unsigned int, unsigned int);
  double DeltaEta(const xAOD::IParticleContainer *);
  double DeltaEta(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);

  double DeltaRapidity(const xAOD::IParticleContainer *);

  double DeltaPhi(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);
  double DeltaPhi(const xAOD::IParticleContainer *);
  double DeltaPhi(const double, const double);

  double CosAlpha(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);
  double CosAlpha(const xAOD::IParticleContainer *);

  double ScalarSumPt(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);
  double ScalarSumPt(const xAOD::IParticleContainer *);

  double VectorSumPt(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);
  double VectorSumPt(const xAOD::IParticleContainer *);

  double METmindeltaphi(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int, const xAOD::MissingET*);
  double METmindeltaphi(const xAOD::IParticleContainer *, const xAOD::MissingET*);

  bool METbisect(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int, const xAOD::MissingET*);
  bool METbisect(const xAOD::IParticleContainer *, const xAOD::MissingET*);

  void TagJetsOverlapTaus( xAOD::JetContainer *, const xAOD::TauJetContainer *, double);

  void TagTausOverlapMuons( xAOD::TauJetContainer *, const xAOD::MuonContainer *, double);

  bool TauOverlapsWithMuon( const xAOD::TauJet *, const xAOD::MuonContainer *, double);
  bool TauOverlapsWithEle( const xAOD::TauJet *, const xAOD::ElectronContainer *, double); //DAN
  bool EleOverlapsWithGoodMuon( const xAOD::Electron *, const xAOD::MuonContainer *, double); //DAN
  bool JetOverlapsWithTau( const xAOD::Jet *, const xAOD::TauJetContainer *, double);
  bool JetOverlapsWithGoodTau( const xAOD::Jet *, const xAOD::TauJetContainer *, double);
  bool JetOverlapsWithGoodMuon( const xAOD::Jet *, const xAOD::MuonContainer *, double); //DAN
  bool JetOverlapsWithGoodEle( const xAOD::Jet *, const xAOD::ElectronContainer *, double); //DAN

  void PtSort(xAOD::IParticleContainer *);

  double MassVisible(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int);  
  double MassVisible(const xAOD::IParticleContainer *, const std::vector<unsigned int> &);
  double MassVisible(const xAOD::IParticleContainer *);

  bool MassCollinearCore(const TLorentzVector&, const TLorentzVector&, const double, const double, double &, double &, double &);   
  bool MassCollinear(const xAOD::TauJetContainer *, const xAOD::MissingET*, const bool, double &, double &, double &); 
  bool MassCollinear(const xAOD::MuonContainer *, const xAOD::MissingET*, const bool, double &, double &, double &); 
  bool MassCollinear(const xAOD::ElectronContainer *, const xAOD::MissingET*, const bool, double &, double &, double &); 
  bool MassCollinear(const xAOD::TauJetContainer *, unsigned int, const xAOD::MuonContainer *, unsigned int, const xAOD::MissingET*, const bool, 
		     double &, double &, double &);
  bool MassCollinear(const xAOD::TauJetContainer *, unsigned int, const xAOD::ElectronContainer *, unsigned int, const xAOD::MissingET*, const bool, 
		     double &, double &, double &);
  bool MassCollinear(const xAOD::MuonContainer *, unsigned int, const xAOD::ElectronContainer *, unsigned int, const xAOD::MissingET*, const bool, 
		     double &, double &, double &);

  double MT(const xAOD::IParticle *, const xAOD::MissingET*);

  TLorentzVector CombinedVector(const xAOD::IParticleContainer *, const std::vector<unsigned int> &);

  double METcentrality(const xAOD::IParticle *, const xAOD::IParticle *, const xAOD::MissingET*);
  double METcentrality(const xAOD::IParticleContainer *, unsigned int, const xAOD::IParticleContainer *, unsigned int, const xAOD::MissingET*);
  double METcentrality(const xAOD::IParticleContainer *, const xAOD::MissingET*);

  
  std::string replace_substr(std::string, const std::string&, const std::string&);

 private:
  struct PtSortPredicate {
    bool operator()(const xAOD::IParticle* p1, const xAOD::IParticle* p2) const {
      return p1->pt() > p2->pt();
    }
  };


  //  }; //namespace
};

#endif
