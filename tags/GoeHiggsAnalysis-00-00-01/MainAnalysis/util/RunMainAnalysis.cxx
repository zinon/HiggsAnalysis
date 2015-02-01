#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoopGrid/PrunDriver.h"
#include "EventLoopGrid/GridDriver.h"

#include "MainAnalysis/MainAnalysis.h"


//testing tau deriv

#include "SampleHandler/DiskListLocal.h"
#include <TSystem.h>

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  int runGrid = 0;
  std::string submitDir = "submitDir";
  std::string sample = "sample";
  std::string jobOptFileName="./MainAnalysis/share/JobOptions/default"; 
  std::string user(gSystem->GetFromPipe("whoami"));
  
  if( argc > 1 ) submitDir = argv[ 1 ];
  if( argc > 2 ) runGrid =std::stoi( argv[ 2 ]);
  if( argc > 3 ) jobOptFileName = argv[ 3 ];
  if( argc > 4 ) sample = argv[ 4 ];
  const char* mysample = sample.c_str();
  



  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  SH::SampleHandler sh;
  
  if(runGrid){
    //   SH::scanDQ2 (sh, "data12_8TeV.periodB.physics_Muons.PhysCont.AOD.repro16_v01/");
    //   SH::scanDQ2 (sh, "mc14_8TeV.167749.Sherpa_CT10_ZeeMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::scanDQ2 (sh, "mc14_8TeV.147808.PowhegPythia8_AU2CT10_Ztautau.recon.AOD.e2372_s1933_s1911_r5591/");
    SH::addGrid(sh, "mc14_8TeV.147808.PowhegPythia8_AU2CT10_Ztautau.merge.AOD.e2372_s1933_s1911_r5591_r5625/"               );
    //SH::addGrid(sh, "mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.merge.AOD.e1727_s1933_s1911_r5591_r5625/"                   );
    //   SH::addGrid(sh, "mc14_8TeV.110140.PowhegPythia_P2011C_st_Wtchan_incl_DR.merge.AOD.e1743_s1933_s1911_r5591_r5625/"       );
    //   SH::addGrid(sh, "mc14_8TeV.110101.AcerMCPythia_P2011CCTEQ6L1_singletop_tchan_l.merge.AOD.e2096_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.110119.PowhegPythia_P2011C_st_schan_lep.merge.AOD.e1720_s1933_s1911_r5591_r5625/"            );
    //   SH::addGrid(sh, "mc14_8TeV.167750.Sherpa_CT10_ZeeMassiveCBPt0_CFilterBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"    );
    //   SH::addGrid(sh, "mc14_8TeV.167753.Sherpa_CT10_ZmumuMassiveCBPt0_CFilterBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"  );
    //   SH::addGrid(sh, "mc14_8TeV.167741.Sherpa_CT10_WenuMassiveCBPt0_CJetFilterBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167744.Sherpa_CT10_WmunuMassiveCBPt0_CJetFilterBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167747.Sherpa_CT10_WtaunuMassiveCBPt0_CJetFilterBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167749.Sherpa_CT10_ZeeMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167751.Sherpa_CT10_ZeeMassiveCBPt0_CVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167752.Sherpa_CT10_ZmumuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167754.Sherpa_CT10_ZmumuMassiveCBPt0_CVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167740.Sherpa_CT10_WenuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167742.Sherpa_CT10_WenuMassiveCBPt0_CJetVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167743.Sherpa_CT10_WmunuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167745.Sherpa_CT10_WmunuMassiveCBPt0_CJetVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167746.Sherpa_CT10_WtaunuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167748.Sherpa_CT10_WtaunuMassiveCBPt0_CJetVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/"); 
    //   SH::addGrid(sh, "mc14_8TeV.167749.Sherpa_CT10_ZeeMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167751.Sherpa_CT10_ZeeMassiveCBPt0_CVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167752.Sherpa_CT10_ZmumuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167754.Sherpa_CT10_ZmumuMassiveCBPt0_CVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167740.Sherpa_CT10_WenuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167742.Sherpa_CT10_WenuMassiveCBPt0_CJetVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167743.Sherpa_CT10_WmunuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167745.Sherpa_CT10_WmunuMassiveCBPt0_CJetVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167746.Sherpa_CT10_WtaunuMassiveCBPt0_BFilter.merge.AOD.e1585_s1933_s1911_r5591_r5625/");
    //   SH::addGrid(sh, "mc14_8TeV.167748.Sherpa_CT10_WtaunuMassiveCBPt0_CJetVetoBVeto.merge.AOD.e1585_s1933_s1911_r5591_r5625/");

  }else if( argc > 4 ){
   SH::scanDir (sh, mysample);

  }else SH::scanDir( sh, "./xAODs/r5591/" );

  
  
  
  // Set the name of the input TTree. It's always "CollectionTree" for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );

  // Add our analysis to the job:
  MainAnalysis* alg = new MainAnalysis();


  //------------------
  // Read joboptions |
  //------------------
  
    std::cout << "##########################################################" << std::endl;
    std::cout << "#####           Retrieving jobOptions, yeaha         #####" << std::endl;
    std::cout << "##########################################################" << std::endl;
  //Read JobOptions
 if (!(argc > 3)) {
    std::cout << "No jobOption file specified, default settings incorporated" << std::endl;
    jobOptFileName="default";
  }
  
  std::cout << "JobOptions kindly provided by :" << ("./MainAnalysis/share/JobOptions/"+jobOptFileName).c_str() << std::endl;
  
  FILE *jobOptFile = fopen(("./MainAnalysis/share/JobOptions/"+jobOptFileName).c_str(),"rt");
  
  if (!jobOptFile) {
    std::cout << "This JobOptionFile does not want to be read: " << jobOptFileName << std::endl;
    std::cout << "Using defaults instead" << std::endl;
  }
  
  std::cout<<"Setting job options"<<std::endl;
  std::cout<<"*******************"<<std::endl;
 
  std::cout<<"####### General Job options"<<std::endl;
  alg->setJobOptions(jobOptFile,alg->jobOptions_createPUfile, "createPUfile");
  alg->setJobOptions(jobOptFile,alg->jobOptions_doPileupReweighting, "doPileupReweighting");
  alg->setJobOptions(jobOptFile,alg->jobOptions_doSystematics, "doSystematics");
  alg->setJobOptions(jobOptFile,alg->jobOptions_testRun, "testRun");
  alg->jobOptions_runGrid=runGrid;
  alg->setJobOptions(jobOptFile,alg->jobOptions_doHistos, "doHistos");
  
  std::cout<<"####### jet related"<<std::endl;
  //jets
  alg->setJobOptions(jobOptFile,alg->jobOptions_jet_collection, "jet_collection");
  alg->setJobOptions(jobOptFile,alg->jobOptions_jet_min_pt, "jet_min_pt");
  alg->setJobOptions(jobOptFile,alg->jobOptions_jet_max_abs_eta, "jet_max_abs_eta");
  alg->setJobOptions(jobOptFile,alg->jobOptions_apply_jet_recalibration, "apply_jet_recalibration");
  alg->setJobOptions(jobOptFile,alg->jobOptions_apply_JES_correction , "apply_JES_correction");
  alg->setJobOptions(jobOptFile,alg->jobOptions_apply_JER_correction , "apply_JER_correction");
  alg->setJobOptions(jobOptFile,alg->jobOptions_jet_cleaning , "jet_cleaning"); //VeryLooseBad MediumBad
  alg->setJobOptions(jobOptFile,alg->jobOptions_jet_jvfcut , "jet_jvfcut"); 

  std::cout<<"####### tau related"<<std::endl;
  //taus
  alg->setJobOptions(jobOptFile,alg->jobOptions_tau_jet_overal_dR, "tau_jet_overal_dR");
  alg->setJobOptions(jobOptFile,alg->jobOptions_tau_muon_overal_dR, "tau_muon_overal_dR");
  alg->setJobOptions(jobOptFile,alg->jobOptions_tau_selection_recommended , "tau_selection_recommended");
  alg->setJobOptions(jobOptFile,alg->jobOptions_apply_tau_correction , "apply_tau_correction");
  alg->setJobOptions(jobOptFile,alg->jobOptions_tau_min_pt , "tau_min_pt");
  alg->setJobOptions(jobOptFile,alg->jobOptions_tau_jet_bdt , "tau_jet_bdt");
  alg->setJobOptions(jobOptFile,alg->jobOptions_tau_ele_bdt , "tau_ele_bdt");

  std::cout<<"####### muon related"<<std::endl;
  // muons
  alg->setJobOptions(jobOptFile,alg->jobOptions_apply_muon_calibration_and_smearing, "apply_muon_calibration_and_smearing");
  alg->setJobOptions(jobOptFile,alg->jobOptions_muon_min_pt , "muon_min_pt"); 
  alg->setJobOptions(jobOptFile,alg->jobOptions_muon_max_iso, "muon_max_iso");  
  alg->setJobOptions(jobOptFile,alg->jobOptions_muon_OR_min_pt, "muon_OR_min_pt");  
  alg->setJobOptions(jobOptFile,alg->jobOptions_muon_OR_max_abs_eta, "muon_OR_max_abs_eta");  


  std::cout<<"####### MET related"<<std::endl;
  //met
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_container, "met_container");

  alg->setJobOptions(jobOptFile,alg->jobOptions_met_jetColl , "met_jetColl");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_muonColl, "met_muonColl");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_tauColl , "met_tauColl");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_eleColl , "met_eleColl");

  alg->setJobOptions(jobOptFile,alg->jobOptions_met_eleTerm , "met_eleTerm");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_gammaTerm,"met_gammaTerm");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_tauTerm , "met_tauTerm");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_jetTerm , "met_jetTerm"); //RefJet RefJet_JVFCut
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_muonTerm, "met_muonTerm");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_softTerm,"met_softTerm"); // SoftClus PVSoftTrk
                                                                           
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_outMETCont,"met_outMETCont");
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_outMETTerm,"met_outMETTerm");
                                                                           
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_jetPtCut , "met_jetPtCut"); //GeV
  alg->setJobOptions(jobOptFile,alg->jobOptions_met_jetDoJvf, "met_jetDoJvf");
  
  // create reweighting file for input sample

  
  job.algsAdd( alg );
  //job.options()->setDouble (EL::Job::optMaxEvents, 10); //job option - only 10 events processed

  // Run the job using different drivers
  if(runGrid){ //if Gridrun specified in main arguments
    
    std::cout<<user<<" you are submitting jobs to GRID ..."<<std::endl;

    EL::PrunDriver driver; //PRun Grid driver //Ganga also avaiable
    //EL::GridDriver driver; //PRun Grid driver //Ganga also avaiable
    
    //grid run options - like prun options but with "nc_*" prefixed
    //driver.options()->setString("nc_nFiles", "1"); //Files to be processed from data set
    driver.options()->setString("nc_outputSampleName", ("user."+user+".%in:name[2]%.rundata.02").c_str()); //name of output dataset
    
    //driver.submit( job, submitDir );
    driver.submitOnly( job, submitDir );
  }
  else{ //direct driver for local runs
    EL::DirectDriver driver;
   driver.submit( job, submitDir );
  }


 
  
  return 0;
}
