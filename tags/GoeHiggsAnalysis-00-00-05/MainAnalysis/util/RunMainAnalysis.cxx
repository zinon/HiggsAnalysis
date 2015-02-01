#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoopGrid/PrunDriver.h"
#include "EventLoopGrid/GridDriver.h"
#include "SampleHandler/DiskListLocal.h"
#include "SampleHandler/DiskListEOS.h"

#include "MainAnalysis/MainAnalysis.h"

#include <TSystem.h>

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <iostream>


static void show_usage(const std::string &name)
{
  std::cerr << "Usage: " << name << " <option(s)> SOURCES\n" 
            << "Options:\n"
            << "\t-h or --help : help\n"
            << "\t-d or --destination : specifies the destination path for the ouput stuff\n"
	    << "\t--lsd or --local-sample-directory : specifies the local directory with input samples\n"
	    << "\t-e or --eos : reads from EOS\n"
	    << "\t-esd or --eos-sample-directory : defines the sample directory on EOS to be read\n"
	    << "\t-ep or --eos-path : define a full path to EOS"
            << std::endl;
}

static std::string get_current_time_and_date()
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  //ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
  return ss.str();
}

int main( int argc, // Number of strings in array argv
          char* argv[] // Array of command-line argument strings
          ) {

  //std::cout<<get_current_time_and_date()<<std::endl;

  //list of arguments
  std::vector <std::string> sources;
  bool runGrid = false;
  bool readEOS = false;
  std::string destDir = "testRun";
  std::string jobOptFileName ="default";
  std::string jobOptFilePath(gSystem->GetFromPipe("echo $ROOTCOREBIN/data/MainAnalysis/JobOptions/"));
  std::string user(gSystem->GetFromPipe("whoami"));
  std::string userInitial = user.substr(0,1);
 // std::string localSampleDir = "/afs/desy.de/user/b/blumen/Scratch/xAOD/higg4d1/";
 // std::string localSampleDir = "/nfs/dust/atlas/user/edrechsl/dc14/mc_TauDer/";
  std::string localSampleDir = "./xAODs/r5591/";
  std::string eosPath = "/eos/atlas/user/"+userInitial+"/"+user+"/";
  std::string eosSampleDir;
  std::string gridSamplesList = "";
  int maxProcEvents = -1;

  //source
  std::string arg0 = argv[0];
  std::string input;
  //loop over arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if ((arg == "-h") || (arg == "--help")) {
      show_usage( arg0 );
      return 0;
    }
    //
    else if ( (arg == "-d") || (arg == "--destination")) {
      input = argv[++i];
      if (i  < argc + 1 && input.substr(0,1) != "-" ) { 
	destDir = input;
      } else { 
        std::cerr << "--destination / -d  option requires one argument." << std::endl;
        return 1;
      }
    } 
    //
    else if ( arg == "-e" || arg == "--eos") {
      if (i  < argc) { 
        readEOS = true;
      }
    } 
    //
    else if ( (arg == "-ep") || (arg == "--eos-path")) {
      input = argv[++i];
      if (i  < argc + 1 && input.substr(0,1) != "-" ) { 
	eosPath = input;
        readEOS = true;
      } else { 
        std::cerr << "--eos-path / -ep  option requires one argument." << std::endl;
        return 1;
      }
    } 
    //
    else if ( (arg == "-esd") || (arg == "--eos-sample-directory")) {
      input = argv[++i];
      if (i  < argc + 1 && input.substr(0,1) != "-" ) { 
	eosSampleDir = input;
        readEOS = true;
      } else { 
        std::cerr << "--eos-sample-directory / -esd  option requires one argument." << std::endl;
        return 1;
      }
    } 
    //
    else if ( (arg == "-lsd") || (arg == "--local-sample-directory")) {
      input = argv[++i];
      if (i  < argc + 1 && input.substr(0,1) != "-" ) { 
	localSampleDir = input;
      } else { 
        std::cerr << "--local-sample-directory / -lsd  option requires one argument." << std::endl;
        return 1;
      }
    } 
    //

      //
    else {
      sources.push_back(argv[i]);
    }

  }
  
  //add trailing slash if needed
  if (! eosPath.empty() && eosPath.back() != '/')
    eosPath += '/';

  std::string eosFullPath = eosPath + eosSampleDir;
  //removing trailing slash if any
  eosFullPath.erase(std::find_if(eosFullPath.rbegin(), eosFullPath.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), eosFullPath.end());


  std::cout<< "Run on grid : "<< runGrid <<std::endl;
  std::cout<< "Read EOS : "<< readEOS <<std::endl;
  std::cout<< "Sample directory : "<< destDir <<std::endl;
  std::cout<< "Destination directory : "<< destDir <<std::endl;
  std::cout<< "Job options file : "<< jobOptFileName <<std::endl;
  std::cout<< "Job options path : "<< jobOptFilePath <<std::endl;
  std::cout<< "User name : "<< user <<std::endl;
  std::cout<< "Usern initial : "<< userInitial <<std::endl;
  std::cout<< "Local sample directory : "<< localSampleDir <<std::endl;
  std::cout<< "EOS path : "<< eosPath <<std::endl;
  std::cout<< "EOS sample directory : "<< eosSampleDir <<std::endl;
  std::cout<< "EOS full path : "<< eosFullPath <<std::endl;

  gSystem->Exec("ls $ROOTCOREBIN/data/MainAnalysis/JobOptions/");

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on
  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SampleHandler
  // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/D3PDTools/SampleHandler/trunk
  SH::SampleHandler sh;

  if(runGrid){
    //read single file from command line or a files list from a txt file and assign strings to a vector
  }else if( readEOS  ){
    std::cout<<"Reading from EOS ..."<<std::endl;
    //SH::DiskListEOS eoslist ("/eos/atlas/user/z/zenon", "root://eosatlas//eos/atlas/user/z/zenon");
    SH::DiskListEOS list ( eosFullPath.c_str(), ("root://eosatlas/"+eosFullPath).c_str() );
    SH::scanDir (sh, list);
    //SH::scanDir (sh, eoslist, "*root*");
  } else{
    SH::DiskListLocal list ( localSampleDir.c_str() );
    SH::scanDir( sh, list);
  }

  // Set the name of the input TTree. It's always "CollectionTree" for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );

  //jobs options file
  //Read JobOptions
  std::string jobOptionsFile = jobOptFilePath + jobOptFileName;

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
    driver.submitOnly( job, destDir );

  } else { //direct driver for local runs
    std::cout<<"RunMainAnalysis : Dear "<<user<<", you are running locally ..."<<std::endl;

    EL::DirectDriver driver;

    driver.submit( job, destDir );
  }
  

  return 0;
}

