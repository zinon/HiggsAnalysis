// Toolbox for self-defined, hand-polished methods of pure awesomeness

#include <BoolTox/BoolTox.h>

//namespace BT {
BoolTox::BoolTox(){


}



BoolTox::~BoolTox(){

}

void BoolTox::PtSort(xAOD::IParticleContainer *vp){

  if(!vp->empty()) std::sort(vp->begin(), vp->end(),
                             [](const xAOD::IParticle *a, const xAOD::IParticle *b)
                             -> bool { return a->pt() > b->pt(); } //C++11 lambda function
                             //PtSortPredicate
                              );
}

bool BoolTox::Goodness(const xAOD::IParticle *p, std::vector<std::string> &v){

  for (auto itr = begin(v); itr != end(v); ++itr)
    if( ! p->auxdata< int >( itr->c_str() ) )
      return false;


  return true;
}

bool BoolTox::TauOverlapsWithMuon( const xAOD::TauJet *tau, const xAOD::MuonContainer *muons, double dR = 0.2){

  for( const xAOD::Muon *muon : *muons )
    if( DeltaR( tau, muon) < dR)
      return true;
  
  return false;
}

bool BoolTox::JetOverlapsWithTau( const xAOD::Jet *jet, const xAOD::TauJetContainer *taus, double dR = 0.2){

  for( const xAOD::TauJet *tau : *taus )
    if( DeltaR( jet, tau) < dR)
      return true;
  
  return false;
}

bool BoolTox::JetOverlapsWithGoodTau( const xAOD::Jet *jet, const xAOD::TauJetContainer *taus, double dR = 0.2){

  for( const xAOD::TauJet *tau : *taus )
    if( tau->auxdata< int >( "good" ) ) 
      if( DeltaR( jet, tau) < dR)
	return true;
  
  return false;
}

void BoolTox::TagTausOverlapMuons( xAOD::TauJetContainer *taus, const xAOD::MuonContainer *muons, double dR = 0.2){
  //tags taus if they overlap with selected muons

  for( xAOD::TauJet *tau : *taus )
    tau->auxdata< int >( "no_muon_overlap" ) = 1;

  for( xAOD::TauJet *tau : *taus )
    for( const xAOD::Muon *muon : *muons )
      if( DeltaR( tau, muon) < dR)
	tau->auxdata< int >( "no_muon_overlap" ) = 0;

}

void BoolTox::TagJetsOverlapTaus( xAOD::JetContainer *jets, const xAOD::TauJetContainer *taus, double dR = 0.2){
  //tags jets if they overlap with good taus 

  for(auto jet_itr = jets->begin(); jet_itr != jets->end(); jet_itr++)
    ( *jet_itr )->auxdata< int >( "no_tau_overlap" ) = 1;

  for(auto tau_itr = taus->begin(); tau_itr != taus->end(); tau_itr++)
    if( ( *tau_itr )->auxdata< int >( "good" ) ) 
      for(auto jet_itr = jets->begin(); jet_itr != jets->end(); jet_itr++)
        if( DeltaR( *jet_itr, *tau_itr) > dR)
          ( *jet_itr )->auxdata< int >( "no_tau_overlap" ) = 0;

}


double BoolTox::DeltaR(const xAOD::IParticleContainer *pc, unsigned int i, unsigned int j){

  if( i < static_cast<unsigned int>(pc->size() ) &&  j < static_cast<unsigned int>(pc->size() ) )
    return (*pc)[i]->p4().DeltaR((*pc)[j]->p4());
  else
    Warning("BoolTox::DeltaR", "elements %i or %i over the range of  the container ... ", i, j );

  return -1;
}

double BoolTox::DeltaR(const xAOD::IParticleContainer *pc){

  if( pc->size()  > 1 )
    return (*pc)[0]->p4().DeltaR((*pc)[1]->p4());
  else
    Warning("BoolTox::DeltaR", "too few elements in the container ");

  return -1;

}

double BoolTox::DeltaR(const xAOD::IParticle *particle1, const xAOD::IParticle *particle2){

  return particle1->p4().DeltaR(particle2->p4());

}

double BoolTox::DeltaR(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return (*pci)[i]->p4().DeltaR((*pcj)[j]->p4());
  else
    Warning("BoolTox::DeltaR", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double BoolTox::DeltaEta(const xAOD::IParticle *particle1, const xAOD::IParticle *particle2){
  return std::fabs(particle1->eta() - particle2->eta());
}

double BoolTox::DeltaEta(const xAOD::IParticleContainer *pc, unsigned int i,  unsigned int j){
  if( i < static_cast<unsigned int>(pc->size() ) &&  j < static_cast<unsigned int>(pc->size() ) )
    return std::fabs( (*pc)[i]->eta() - (*pc)[j]->eta() );
  else
    Warning("BoolTox::DeltaEta", "elements %i, %i over the range of  the container ", i, j );
  return -1;
}

double BoolTox::DeltaEta(const xAOD::IParticleContainer *pc){
  if( pc->size()  > 1 )
    return std::fabs( (*pc)[0]->eta() - ((*pc)[1]->eta()) );
  else
    Warning("BoolTox::DeltaEta", "too few elements in the container ");

  return -1;
}

double BoolTox::DeltaEta(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::fabs( (*pci)[i]->eta() - (*pcj)[j]->eta() );
  else
    Warning("BoolTox::DeltaEta", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double BoolTox::DeltaPhi(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::fabs( (*pci)[i]->p4().DeltaPhi((*pcj)[j]->p4()) );
  else
    Warning("BoolTox::DeltaPhi", "elements %i or %i over the range of the input containers... ", i, j );

  return -11;
}

double BoolTox::DeltaPhi(const xAOD::IParticleContainer *pc){
  if( pc->size()  > 1 )
    return std::fabs( (*pc)[0]->p4().DeltaPhi((*pc)[1]->p4()) );
  else
    Warning("BoolTox::DeltaPhi", "too few elements in the container : %lui ", pc->size() );

  return -11;
}

double BoolTox::DeltaPhi(const double phi1, const double phi2){
  return TMath::Pi() - std::fabs( std::fabs( phi1 - phi2 ) - TMath::Pi() );
}

double BoolTox::CosAlpha(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::cos( (*pci)[i]->p4().Angle( (*pcj)[j]->p4().Vect() ) );
  else
    Warning("BoolTox::CosAlpha", "elements %i or %i over the range of the input containers... ", i, j );

  return -11;
}

double BoolTox::CosAlpha(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1 ) 
    return std::cos( (*pc)[0]->p4().Angle( (*pc)[1]->p4().Vect() ) );
  else
    Warning("BoolTox::CosAlpha", "too few elements for the input containers %lui ", pc->size() );

  return -11;
}

double BoolTox::ScalarSumPt(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return (*pci)[i]->pt() +  (*pcj)[j]->pt();
  else
    Warning("BoolTox::ScalarSumPt", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double BoolTox::ScalarSumPt(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1 )
    return (*pc)[0]->pt() +  (*pc)[1]->pt();
  else
    Warning("BoolTox::ScalarSumPt", "too few elements (%lui) for the input container ", pc->size() );

  return -1;
}

double BoolTox::VectorSumPt(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return ( (*pci)[i]->p4() +  (*pcj)[j]->p4() ).Pt();
  else 
    Warning("BoolTox::VectorSumPt", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double BoolTox::VectorSumPt(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1 )
    return ( (*pc)[0]->p4() +  (*pc)[1]->p4() ).Pt();
  else 
    Warning("BoolTox::VectorSumPt", "too few elements (%lui) for the input container  ", pc->size());

  return -1;
}

double BoolTox::MassVisible(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return ( (*pci)[i]->p4() +  (*pcj)[j]->p4() ).M();
  else
    Warning("BoolTox::MassVisible", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double BoolTox::MassVisible(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1) 
    return ( (*pc)[0]->p4() +  (*pc)[1]->p4() ).M();
  else
    Warning("BoolTox::MassVisible", "too few elements for the input container %lui ", pc->size() );

  return -1;
}

double BoolTox::MassVisible(const xAOD::IParticleContainer *pc, const std::vector<unsigned int> &entries){

  return CombinedVector(pc, entries).M();
}

double BoolTox::METmindeltaphi(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j, const xAOD::MissingET* met){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::min( DeltaPhi( (*pci)[i]->phi(), met->phi() ), DeltaPhi( (*pcj)[j]->phi(), met->phi() ) ); 
  else
    Warning("BoolTox::METbisect", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double BoolTox::METmindeltaphi(const xAOD::IParticleContainer *pc, const xAOD::MissingET* met){

  if( pc->size() > 1 )
    return std::min( DeltaPhi( (*pc)[0]->phi(), met->phi() ), DeltaPhi( (*pc)[1]->phi(), met->phi() ) ); 
  else
    Warning("BoolTox::METbisect", "too few elements (%lui) for the input container ", pc->size() );

  return -1;
}

bool BoolTox::METbisect(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j, const xAOD::MissingET* met){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) ){

    double dphi = DeltaPhi( (*pcj)[j]->phi(), (*pcj)[j]->phi() );
    double dphi1 = DeltaPhi( (*pci)[i]->phi(), met->phi() );
    double dphi2 = DeltaPhi( (*pcj)[j]->phi(), met->phi() );

    return (std::max(dphi1, dphi2)<=dphi) && (dphi1+dphi2 <= TMath::Pi() );

  }else
    Warning("BoolTox::METbisect", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

bool BoolTox::METbisect(const xAOD::IParticleContainer *pc, const xAOD::MissingET* met){

  if( pc->size() > 1){

    double dphi = DeltaPhi( (*pc)[0]->phi(), (*pc)[1]->phi() );
    double dphi1 = DeltaPhi( (*pc)[0]->phi(), met->phi() );
    double dphi2 = DeltaPhi( (*pc)[1]->phi(), met->phi() );

    return (std::max(dphi1, dphi2)<=dphi) && (dphi1+dphi2 <= TMath::Pi() );

  }else
    Warning("BoolTox::METbisect", "too few elements (%lui) for the input container", pc->size() );

  return -1;
}

TLorentzVector BoolTox::CombinedVector(const xAOD::IParticleContainer *pc, const std::vector<unsigned int> &entries ){

  TLorentzVector v(0,0,0,0);
  for(const auto i : entries)
    if( i < static_cast<unsigned int>(pc->size() ) )
      v+=(*pc)[i]->p4();
    else
      Warning("BoolTox::CombinedVector", "element %i exceeds size of container ", i );

  return v;

}

bool BoolTox::MassCollinear(const xAOD::TauJetContainer *pc, //particle container 
			    const xAOD::MissingET* met, //met
			    const bool kMMCsynchronize, //mmc sychronization
			    double &mass, double &xp1, double &xp2){ //result

  if(pc->size() <= 1) return false;

  TLorentzVector k1 = std::move( (*pc)[0]->p4() );   
  TLorentzVector k2 = std::move( (*pc)[1]->p4() );
        
  ///redefine tau vectors if necessary - MMC sychronization
  if(kMMCsynchronize){
    k1.SetPtEtaPhiM( k1.Pt(), k1.Eta(), k1.Phi(), (*pc)[0]->nTracks() < 3 ? 800. : 1200. ); //MeV
    k2.SetPtEtaPhiM( k2.Pt(), k2.Eta(), k2.Phi(), (*pc)[1]->nTracks() < 3 ? 800. : 1200. );
  }

   return MassCollinearCore( k1, k2, met->mpx(), met->mpy(), mass, xp1, xp2);
}

bool BoolTox::MassCollinearCore(const TLorentzVector &k1, const TLorentzVector &k2, //particles 
				const double metetx, const double metety, //met
				double &mass, double &xp1, double &xp2){ //result

  TMatrixD K(2, 2);
  K(0, 0) = k1.Px();      K(0, 1) = k2.Px();
  K(1, 0) = k1.Py();      K(1, 1) = k2.Py();

  if(K.Determinant()==0)
    return false;

  TMatrixD M(2, 1);
  M(0, 0) = metetx; 
  M(1, 0) = metety;

  TMatrixD Kinv = K.Invert();

  TMatrixD X(2, 1);
  X = Kinv*M;

  double X1 = X(0, 0);    double X2 = X(1, 0);
  double x1 = 1./(1.+X1); double x2 = 1./(1.+X2);

  TLorentzVector p1 = k1*(1/x1);
  TLorentzVector p2 = k2*(1/x2);

  double m = (p1+p2).M();

  //return to caller
  mass = m; 

  if(k1.Pt() > k2.Pt()){
    xp1 = x1; xp2 = x2;
  }else{
    xp1 = x2; xp2 = x1;
  }

  return true;
}

std::string BoolTox::replace_substr(std::string subject, const std::string& search, const std::string& replace){

  size_t pos = 0;
  while((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }

  return subject;
}

TauAnalysisTools::e_JETID BoolTox::TauJetBDTindex(const std::string &str){

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
  else if (str.find("JETBDTLOOSENOTTIGHT") != std::string::npos) 
    return TauAnalysisTools::JETBDTLOOSENOTTIGHT;
  else if (str.find("JETBDTMEDIUMNOTTIGHT") != std::string::npos) 
    return TauAnalysisTools::JETBDTMEDIUMNOTTIGHT;
  else
    Warning("TauAnalysisTools::e_JETID BoolTox::TauJetBDTindex()", "Unable to return value for option %s", str.c_str() );

  return TauAnalysisTools::JETIDNONE;

}

TauAnalysisTools::e_ELEID BoolTox::TauEleBDTindex(const std::string &str){
 
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
    Warning("TauAnalysisTools::e_ELEID BoolTox::TauEleBDTindex()", "Unable to return value for option %s", str.c_str() );
  
  return TauAnalysisTools::ELEIDNONE;
}
