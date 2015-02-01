#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoopGrid/PrunDriver.h"
#include "EventLoopGrid/GridDriver.h"
#include "SampleHandler/DiskListLocal.h"

#include "MainAnalysis/MainAnalysis.h"

#include <TSystem.h>

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  int runGrid = 0;
  std::string submitDir = "submitDir";
  std::string sample = "sample";
  std::string jobOptFileName="default"; 
  std::string jobOptFilePath(gSystem->GetFromPipe("echo $ROOTCOREBIN/data/MainAnalysis/JobOptions/")); 
  std::string user(gSystem->GetFromPipe("whoami"));
  
  if( argc > 1 ) submitDir = argv[ 1 ];
  if( argc > 2 ) runGrid =std::stoi( argv[ 2 ]);
  if( argc > 3 ) jobOptFileName = argv[ 3 ];
  if( argc > 4 ) sample = argv[ 4 ];
  const char* mysample = sample.c_str();
  

  gSystem->Exec("ls $ROOTCOREBIN/data/MainAnalysis/JobOptions/");

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

  //jobs options file
  //Read JobOptions
  if (!(argc > 3)) {
    std::cout << "RunMainAnalysis: No jobOption file specified, default settings incorporated" << std::endl;
    jobOptFileName="default";
  }
  std::string jobOptionsFile = jobOptFilePath+jobOptFileName;
  std::cout << "RunMainAnalysis : JobOptions kindly provided by " << jobOptionsFile << std::endl;

  // Add our analysis to the job:
  MainAnalysis* alg = new MainAnalysis( );

  alg->JobOptionsFile( jobOptionsFile );
  
  // create reweighting file for input sample
  
  job.algsAdd( alg );
  //job.options()->setDouble (EL::Job::optMaxEvents, 10); //job option - only 10 events processed

  // Run the job using different drivers
  if(runGrid){ //if Gridrun specified in main arguments
    
    std::cout<<"RunMainAnalysis : Dear "<<user<<", you are submitting jobs to GRID ..."<<std::endl;

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
