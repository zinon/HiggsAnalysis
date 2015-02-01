#ifndef MainAnalysis_MainAnalysisIncludes_H
#define MainAnalysis_MainAnalysisIncludes_H

//event info
#include "xAODEventInfo/EventInfo.h"

//============
//STD includes
//============
#include <iostream>
#include <string>
#include <utility> //std::move
//#include <memory>

#include <stdio.h> //fseek

#include <chrono>
#include <ctime>

//============
//STL includes
//============
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <memory>


//implementation for C++14
#include "CxxUtils/make_unique.h"

//============
//EDM includes
//============
//#include "AsgTools/AsgTool.h"
//#include "PATCore/IAsgSelectionTool.h"

//============
//EDM includes
//============
#include "xAODBase/IParticleHelpers.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetAttributes.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/Muon.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingET.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"


//========
//CP tools
//========
// GRL
#include "GoodRunsLists/GoodRunsListSelectionTool.h"

// PileUpReweighting

#include "PileupReweighting/PileupReweightingTool.h" 

//Jet Tools 
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetResolution/JERTool.h"
#include "JetResolution/JERSmearingTool.h"
#include "JetCalibTools/JetCalibrationTool.h"
#include "JetUncertainties/JetUncertaintiesTool.h"

//TauAnalysisTools
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

//MuonTools
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

//ElectronTools
//#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
//#include "ElectronIsolationSelection/ElectronIsolationSelectionTool.h"

//MET Tools
#include "METUtilities/METRebuilder.h"
//#include "METUtilities/METHelper.h"

// header for systematics:
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h" 
#include "PATInterfaces/SystematicsUtil.h"

//TStore for CP tools
#include "xAODRootAccess/TStore.h"

// shallow copy of containers
#include "xAODCore/ShallowCopy.h"


//==============
//handmade stuff
//==============
#include "xTools/xTools.h"
#include "xJobOptions/xJobOptions.h"



//=====================
// object declarations
//=====================

//EDM containers
//std::shared_ptr<xAOD::JetContainer> m_ViewElemJetCont;
xAOD::JetContainer *m_ViewElemJetCont;
xAOD::TauJetContainer* m_ViewElemTauCont;
xAOD::MuonContainer* m_ViewElemMuonCont;

std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > m_JetContShallowCopy;
std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* > m_TauJetContShallowCopy;
std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > m_MuonContShallowCopy;
std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > m_EleContShallowCopy;

// Containers for rebuilt MET, these get initialized by METRebuilder
const xAOD::MissingETContainer* m_MissingETCalibCont;
const xAOD::MissingETAuxContainer* m_MissingETAuxCalibCont;

//Common Tools
std::unique_ptr<GoodRunsListSelectionTool> m_grl;
std::unique_ptr<CP::PileupReweightingTool> m_pileup;

std::unique_ptr<JetCleaningTool> m_jetCleaning;
std::unique_ptr<JERTool> m_JERTool;
std::unique_ptr<JERSmearingTool> m_JERSmearingTool;
std::unique_ptr<JetCalibrationTool> m_jetCalibTool;
std::unique_ptr<JetUncertaintiesTool> m_JESUncertaintyTool;

std::unique_ptr<TauAnalysisTools::TauSelectionTool> m_tauSelTool; //lives in the TauAnalysisTools:: namespace
std::unique_ptr<TauAnalysisTools::TauEfficiencyCorrectionsTool> m_tauEffTool; //lives in the TauAnalysisTools:: namespace
std::unique_ptr<TauAnalysisTools::TauSmearingTool> m_tauSmearTool; //lives in the TauAnalysisTools:: namespace

std::unique_ptr<CP::MuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool;
std::unique_ptr<CP::MuonSelectionTool> m_muonSelectionTool;
std::unique_ptr<CP::MuonEfficiencyScaleFactors> m_muonEfficiencyScaleFactorsTool;

// list of systematics
std::vector<CP::SystematicSet> m_sysList;

//MET tools
std::unique_ptr<met::METRebuilder> m_metRebuilder;

// Needed for METRebuilder constituent map
SG::AuxElement::Accessor< ElementLink<xAOD::IParticleContainer> > m_objLinkAcc("originalObjectLink");

// Transient object store. Needed for the CP tools and MET
std::unique_ptr<xAOD::TStore> m_store;

//helper class
std::unique_ptr<xTools> m_t;

//job options class
std::unique_ptr<xJobOptions> m_jo;

//time measures
std::chrono::time_point<std::chrono::system_clock> start, end, start_initialize, end_initialize, start_execute, end_execute, start_finalize, end_finalize;  

#endif // MainAnalysis_MainAnalysisIncludes_H
