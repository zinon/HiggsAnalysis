
#include <xTools/xTools.h>

//namespace BT {
xTools::xTools(){


}



//xTools::~xTools() = default; //pure and auto generated

void xTools::PtSort(xAOD::IParticleContainer *vp){

  if(!vp->empty()) std::sort(vp->begin(), vp->end(),
                             [](const xAOD::IParticle *a, const xAOD::IParticle *b) -> bool 
			     { return a->pt() > b->pt(); } //C++11 lambda function
                             //PtSortPredicate
                              );
}

bool xTools::Goodness(const xAOD::IParticle *p, std::vector<std::string> &v){

  for (auto itr = begin(v); itr != end(v); ++itr)
    if( ! p->auxdata< int >( itr->c_str() ) )
      return false;


  return true;
}

bool xTools::TauOverlapsWithMuon( const xAOD::TauJet *tau, const xAOD::MuonContainer *muons, double dR = 0.2){

  for( const xAOD::Muon *muon : *muons )
    if( muon->auxdata< int >( "overlap" ) )
      if( DeltaR( tau, muon) < dR)
	return true;
  
  return false;
}

bool xTools::JetOverlapsWithTau( const xAOD::Jet *jet, const xAOD::TauJetContainer *taus, double dR = 0.2){

  for( const xAOD::TauJet *tau : *taus )
    if( DeltaR( jet, tau) < dR)
      return true;
  
  return false;
}

bool xTools::JetOverlapsWithGoodTau( const xAOD::Jet *jet, const xAOD::TauJetContainer *taus, double dR = 0.2){

  for( const xAOD::TauJet *tau : *taus )
    if( tau->auxdata< int >( "good" ) ) 
      if( DeltaR( jet, tau) < dR)
	return true;
  
  return false;
}

void xTools::TagTausOverlapMuons( xAOD::TauJetContainer *taus, const xAOD::MuonContainer *muons, double dR = 0.2){
  //tags taus if they overlap with selected muons tagged as "overlap"

  for( xAOD::TauJet *tau : *taus )
    tau->auxdata< int >( "no_muon_overlap" ) = 1;

  for( xAOD::TauJet *tau : *taus )
    for( const xAOD::Muon *muon : *muons )
      if( muon->auxdata< int >( "overlap" ) )
	if( DeltaR( tau, muon) < dR)
	  tau->auxdata< int >( "no_muon_overlap" ) = 0;

}

void xTools::TagJetsOverlapTaus( xAOD::JetContainer *jets, const xAOD::TauJetContainer *taus, double dR = 0.2){
  //tags jets if they overlap with good taus 

  for(auto jet_itr = jets->begin(); jet_itr != jets->end(); jet_itr++)
    ( *jet_itr )->auxdata< int >( "no_tau_overlap" ) = 1;

  for(auto tau_itr = taus->begin(); tau_itr != taus->end(); tau_itr++)
    if( ( *tau_itr )->auxdata< int >( "good" ) ) 
      for(auto jet_itr = jets->begin(); jet_itr != jets->end(); jet_itr++)
        if( DeltaR( *jet_itr, *tau_itr) > dR)
          ( *jet_itr )->auxdata< int >( "no_tau_overlap" ) = 0;

}


double xTools::DeltaR(const xAOD::IParticleContainer *pc, unsigned int i, unsigned int j){

  if( i < static_cast<unsigned int>(pc->size() ) &&  j < static_cast<unsigned int>(pc->size() ) )
    return (*pc)[i]->p4().DeltaR((*pc)[j]->p4());
  else
    Warning("xTools::DeltaR", "elements %i or %i over the range of  the container ... ", i, j );

  return -1;
}

double xTools::DeltaR(const xAOD::IParticleContainer *pc){

  if( pc->size()  > 1 )
    return (*pc)[0]->p4().DeltaR((*pc)[1]->p4());
  else
    Warning("xTools::DeltaR", "too few elements in the container ");

  return -1;

}

double xTools::DeltaR(const xAOD::IParticle *particle1, const xAOD::IParticle *particle2){

  return particle1->p4().DeltaR(particle2->p4());

}

double xTools::DeltaR(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if( static_cast<unsigned int>(pci->size() ) &&  static_cast<unsigned int>(pcj->size() ) )
    return (*pci)[0]->p4().DeltaR((*pcj)[0]->p4());
  else
    Warning("xTools::DeltaR", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

double xTools::DeltaR(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return (*pci)[i]->p4().DeltaR((*pcj)[j]->p4());
  else
    Warning("xTools::DeltaR", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double xTools::DeltaEta(const xAOD::IParticle *particle1, const xAOD::IParticle *particle2){
  return std::fabs(particle1->eta() - particle2->eta());
}

double xTools::DeltaEta(const xAOD::IParticleContainer *pc, unsigned int i,  unsigned int j){
  if( i < static_cast<unsigned int>(pc->size() ) &&  j < static_cast<unsigned int>(pc->size() ) )
    return std::fabs( (*pc)[i]->eta() - (*pc)[j]->eta() );
  else
    Warning("xTools::DeltaEta", "elements %i, %i over the range of  the container ", i, j );
  return -1;
}

double xTools::DeltaEta(const xAOD::IParticleContainer *pc){
  if( pc->size()  > 1 )
    return std::fabs( (*pc)[0]->eta() - ((*pc)[1]->eta()) );
  else
    Warning("xTools::DeltaEta", "too few elements in the container ");

  return -1;
}

double xTools::DeltaEta(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return std::fabs( (*pci)[0]->eta() - (*pcj)[0]->eta() );
  else
    Warning("xTools::DeltaEta", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

double xTools::DeltaEta(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::fabs( (*pci)[i]->eta() - (*pcj)[j]->eta() );
  else
    Warning("xTools::DeltaEta", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double xTools::DeltaRapidity(const xAOD::IParticleContainer *pc){
  if( pc->size()  > 1 )
    return std::fabs( (*pc)[0]->rapidity() - (*pc)[1]->rapidity() );
  else
    Warning("xTools::DeltaEta", "too few elements in the container ");

  return -1;
}

double xTools::DeltaPhi(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if( static_cast<unsigned int>(pci->size() ) &&  static_cast<unsigned int>(pcj->size() ) )
    return std::fabs( (*pci)[0]->p4().DeltaPhi((*pcj)[0]->p4()) );
  else
    Warning("xTools::DeltaPhi", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -11;
}

double xTools::DeltaPhi(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::fabs( (*pci)[i]->p4().DeltaPhi((*pcj)[j]->p4()) );
  else
    Warning("xTools::DeltaPhi", "elements %i or %i over the range of the input containers... ", i, j );

  return -11;
}

double xTools::DeltaPhi(const xAOD::IParticleContainer *pc){
  if( pc->size()  > 1 )
    return std::fabs( (*pc)[0]->p4().DeltaPhi((*pc)[1]->p4()) );
  else
    Warning("xTools::DeltaPhi", "too few elements in the container : %lui ", pc->size() );

  return -11;
}

double xTools::DeltaPhi(const double phi1, const double phi2){
  return TMath::Pi() - std::fabs( std::fabs( phi1 - phi2 ) - TMath::Pi() );
}


double xTools::CosAlpha(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return std::cos( (*pci)[0]->p4().Angle( (*pcj)[0]->p4().Vect() ) );
  else
    Warning("xTools::CosAlpha", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -11;
}

double xTools::CosAlpha(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::cos( (*pci)[i]->p4().Angle( (*pcj)[j]->p4().Vect() ) );
  else
    Warning("xTools::CosAlpha", "elements %i or %i over the range of the input containers... ", i, j );

  return -11;
}

double xTools::CosAlpha(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1 ) 
    return std::cos( (*pc)[0]->p4().Angle( (*pc)[1]->p4().Vect() ) );
  else
    Warning("xTools::CosAlpha", "too few elements for the input containers %lui ", pc->size() );

  return -11;
}

double xTools::ScalarSumPt(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return (*pci)[0]->pt() +  (*pcj)[0]->pt();
  else
    Warning("xTools::CosAlpha", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

double xTools::ScalarSumPt(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return (*pci)[i]->pt() +  (*pcj)[j]->pt();
  else
    Warning("xTools::ScalarSumPt", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double xTools::ScalarSumPt(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1 )
    return (*pc)[0]->pt() +  (*pc)[1]->pt();
  else
    Warning("xTools::ScalarSumPt", "too few elements (%lui) for the input container ", pc->size() );

  return -1;
}

double xTools::VectorSumPt(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return ( (*pci)[0]->p4() +  (*pcj)[0]->p4() ).Pt();
  else 
    Warning("xTools::VectorSumPt", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

double xTools::VectorSumPt(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return ( (*pci)[i]->p4() +  (*pcj)[j]->p4() ).Pt();
  else 
    Warning("xTools::VectorSumPt", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double xTools::VectorSumPt(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1 )
    return ( (*pc)[0]->p4() +  (*pc)[1]->p4() ).Pt();
  else 
    Warning("xTools::VectorSumPt", "too few elements (%lui) for the input container  ", pc->size());

  return -1;
}

double xTools::MassVisible(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj){

  if(static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return ( (*pci)[0]->p4() +  (*pcj)[0]->p4() ).M();
  else
    Warning("xTools::MassVisible", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

double xTools::MassVisible(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return ( (*pci)[i]->p4() +  (*pcj)[j]->p4() ).M();
  else
    Warning("xTools::MassVisible", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double xTools::MassVisible(const xAOD::IParticleContainer *pc){

  if( pc->size() > 1) 
    return ( (*pc)[0]->p4() +  (*pc)[1]->p4() ).M();
  else
    Warning("xTools::MassVisible", "too few elements for the input container %lui ", pc->size() );

  return -1;
}

double xTools::MassVisible(const xAOD::IParticleContainer *pc, const std::vector<unsigned int> &entries){

  return CombinedVector(pc, entries).M();
}

double xTools::METmindeltaphi(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj, const xAOD::MissingET* met){

  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return std::min( DeltaPhi( (*pci)[0]->phi(), met->phi() ), DeltaPhi( (*pcj)[0]->phi(), met->phi() ) ); 
  else
    Warning("xTools::METmindeltaphi", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

double xTools::METmindeltaphi(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j, const xAOD::MissingET* met){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return std::min( DeltaPhi( (*pci)[i]->phi(), met->phi() ), DeltaPhi( (*pcj)[j]->phi(), met->phi() ) ); 
  else
    Warning("xTools::METbisect", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

double xTools::METmindeltaphi(const xAOD::IParticleContainer *pc, const xAOD::MissingET* met){

  if( pc->size() > 1 )
    return std::min( DeltaPhi( (*pc)[0]->phi(), met->phi() ), DeltaPhi( (*pc)[1]->phi(), met->phi() ) ); 
  else
    Warning("xTools::METbisect", "too few elements (%lui) for the input container ", pc->size() );

  return -1;
}

bool xTools::METbisect(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj, const xAOD::MissingET* met){

  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) ){

    double dphi = DeltaPhi( (*pci)[0]->phi(), (*pcj)[0]->phi() );
    double dphi1 = DeltaPhi( (*pci)[0]->phi(), met->phi() );
    double dphi2 = DeltaPhi( (*pcj)[0]->phi(), met->phi() );

    return (std::max(dphi1, dphi2)<=dphi) && (dphi1+dphi2 <= TMath::Pi() );

  }else
    Warning("xTools::METbisect", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1;
}

bool xTools::METbisect(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j, const xAOD::MissingET* met){

  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) ){

    double dphi = DeltaPhi( (*pci)[i]->phi(), (*pcj)[j]->phi() );
    double dphi1 = DeltaPhi( (*pci)[i]->phi(), met->phi() );
    double dphi2 = DeltaPhi( (*pcj)[j]->phi(), met->phi() );

    return (std::max(dphi1, dphi2)<=dphi) && (dphi1+dphi2 <= TMath::Pi() );

  }else
    Warning("xTools::METbisect", "elements %i or %i over the range of the input containers... ", i, j );

  return -1;
}

bool xTools::METbisect(const xAOD::IParticleContainer *pc, const xAOD::MissingET* met){

  if( pc->size() > 1){

    double dphi = DeltaPhi( (*pc)[0]->phi(), (*pc)[1]->phi() );
    double dphi1 = DeltaPhi( (*pc)[0]->phi(), met->phi() );
    double dphi2 = DeltaPhi( (*pc)[1]->phi(), met->phi() );

    return (std::max(dphi1, dphi2)<=dphi) && (dphi1+dphi2 <= TMath::Pi() );

  }else
    Warning("xTools::METbisect", "too few elements (%lui) for the input container", pc->size() );

  return -1;
}

double xTools::METcentrality(const xAOD::IParticleContainer *pci, const xAOD::IParticleContainer *pcj, const xAOD::MissingET* met){


  if( static_cast<unsigned int>(pci->size() ) && static_cast<unsigned int>(pcj->size() ) )
    return METcentrality( (*pci)[0], (*pcj)[0], met );
  else
    Warning("xTools::METcentrality", "at least one container has no elements, %lui or %lui  ", pci->size(), pcj->size());

  return -1234;
}

double xTools::METcentrality(const xAOD::IParticleContainer *pci, unsigned int i, const xAOD::IParticleContainer *pcj, unsigned int j, const xAOD::MissingET* met){


  if( i < static_cast<unsigned int>(pci->size() ) &&  j < static_cast<unsigned int>(pcj->size() ) )
    return METcentrality( (*pci)[i], (*pcj)[j], met );
  else
    Warning("xTools::METcentrality", "elements %i or %i over the range of the input containers... ", i, j );

  return -1234;
}

double xTools::METcentrality(const xAOD::IParticleContainer *pc, const xAOD::MissingET* met){


  if( pc->size() > 1  )
    return METcentrality( (*pc)[0], (*pc)[1], met );
  else
    Warning("xTools::METcentrality", "too few elements (%lui) for the input container ", pc->size() );

  return -1234;
}

double xTools::METcentrality(const xAOD::IParticle *p1, const xAOD::IParticle *p2, const xAOD::MissingET* met){

  double d = std::sin(p2->phi() - p1->phi());
  if ( d != 0 ){
    double A = std::sin(met->phi() - p1->phi())/d;
    double B = std::sin(p2->phi() - met->phi())/d;
    return (A+B)/std::sqrt(A*A + B*B);
  }
  
  return -1234;
}

TLorentzVector xTools::CombinedVector(const xAOD::IParticleContainer *pc, const std::vector<unsigned int> &entries ){

  TLorentzVector v(0,0,0,0);
  for(const auto i : entries)
    if( i < static_cast<unsigned int>(pc->size() ) )
      v+=(*pc)[i]->p4();
    else
      Warning("xTools::CombinedVector", "element %i exceeds size of container ", i );

  return v;

}

double xTools::MT(const xAOD::IParticle *p, const xAOD::MissingET* met){
  
  return std::sqrt( 2 * p->pt() * met->met() * (1 - DeltaPhi( p->phi(), met->phi())) ); 

}

bool xTools::MassCollinear(const xAOD::TauJetContainer *pci, unsigned int i, //particle container 
			    const xAOD::MuonContainer *pcj, unsigned int j, //particle container 
			    const xAOD::MissingET* met, //met
			    const bool kMMCsynchronize, //mmc sychronization
			    double &mass, double &xp1, double &xp2){ //result

  if( i >= static_cast<unsigned int>(pci->size()) ){
    Warning("xTools::MassCollinear", "element %i exceeds size of tau container ", i );
    return false;
  }
  if( j >= static_cast<unsigned int>(pcj->size()) ){
    Warning("xTools::MassCollinear", "element %i exceeds size of muon container ", j );
    return false;
  }

  TLorentzVector k1 = std::move( (*pci)[i]->p4() );   
  TLorentzVector k2 = std::move( (*pcj)[j]->p4() );
        
  ///redefine tau vectors if necessary - MMC sychronization
  if(kMMCsynchronize){
    k1.SetPtEtaPhiM( k1.Pt(), k1.Eta(), k1.Phi(), (*pci)[i]->nTracks() < 3 ? 800. : 1200. ); //MeV
    //k2.SetPtEtaPhiM( k2.Pt(), k2.Eta(), k2.Phi(), (*pcj)[j]->nTracks() < 3 ? 800. : 1200. ); //Muon: TO-DO
  }

   return MassCollinearCore( k1, k2, met->mpx(), met->mpy(), mass, xp1, xp2);
}

bool xTools::MassCollinear(const xAOD::TauJetContainer *pc,//particle container 
			    const xAOD::MissingET* met, //met
			    const bool kMMCsynchronize, //mmc sychronization
			    double &mass, double &xp1, double &xp2){ //result

  if( pc->size() < 1 ){
    Warning("xTools::MassCollinear", "insufficient tau container size %lui", pc->size() );
    return false;
  }

  TLorentzVector k1 = std::move( (*pc)[0]->p4() );   
  TLorentzVector k2 = std::move( (*pc)[1]->p4() );
        
  ///redefine tau vectors if necessary - MMC sychronization
  if(kMMCsynchronize){
    k1.SetPtEtaPhiM( k1.Pt(), k1.Eta(), k1.Phi(), (*pc)[0]->nTracks() < 3 ? 800. : 1200. ); //MeV
    k2.SetPtEtaPhiM( k2.Pt(), k2.Eta(), k2.Phi(), (*pc)[1]->nTracks() < 3 ? 800. : 1200. ); //Muon: TO-DO
  }

   return MassCollinearCore( k1, k2, met->mpx(), met->mpy(), mass, xp1, xp2);
}

bool xTools::MassCollinear(const xAOD::MuonContainer *pc,//particle container 
			    const xAOD::MissingET* met, //met
			    const bool kMMCsynchronize, //mmc sychronization
			    double &mass, double &xp1, double &xp2){ //result

  if( pc->size() < 1 ){
    Warning("xTools::MassCollinear", "insufficient muon container size %lui", pc->size() );
    return false;
  }

  TLorentzVector k1 = std::move( (*pc)[0]->p4() );   
  TLorentzVector k2 = std::move( (*pc)[1]->p4() );
        
  ///redefine tau vectors if necessary - MMC sychronization
  if(kMMCsynchronize){
    //k1.SetPtEtaPhiM( k1.Pt(), k1.Eta(), k1.Phi(), (*pc)[0]->nTracks() < 3 ? 800. : 1200. ); //MeV
    //k2.SetPtEtaPhiM( k2.Pt(), k2.Eta(), k2.Phi(), (*pc)[1]->nTracks() < 3 ? 800. : 1200. ); //Muon: TO-DO what?
  }

   return MassCollinearCore( k1, k2, met->mpx(), met->mpy(), mass, xp1, xp2);
}

bool xTools::MassCollinear(const xAOD::TauJetContainer *pci, //particle container 
			    const xAOD::MuonContainer *pcj, //particle container 
			    const xAOD::MissingET* met, //met
			    const bool kMMCsynchronize, //mmc sychronization
			    double &mass, double &xp1, double &xp2){ //result

  return MassCollinear(pci, 0, pcj, 0, met, kMMCsynchronize, mass, xp1, xp2);
}
bool xTools::MassCollinearCore(const TLorentzVector &k1, const TLorentzVector &k2, //particles 
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

std::string xTools::replace_substr(std::string subject, const std::string& search, const std::string& replace){

  size_t pos = 0;
  while((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }

  return subject;
}

