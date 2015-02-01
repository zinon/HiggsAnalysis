void Run (const std::string& submitDir)
{
  // Load the libraries for all packages
  gROOT->Macro("$ROOTCOREDIR/scripts/load_packages.C");

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // create a new sample handler to describe the data files we use
  SH::SampleHandler sh;

  // scan for datasets in the given directory
  // this works if you are on lxplus, otherwise you'd want to copy over files
  // to your local machine and use a local path.  if you do so, make sure
  // that you copy all subdirectories and point this to the directory
  // containing all the files, not the subdirectories.
  //SH::scanDir (sh, "/afs/cern.ch/work/z/zenon/GoeHiggsAnalysis/xAODs/r5591/"); 
  SH::scanDir (sh, "xAODs/r5591/"); 

  // or for data:
  //SH::scanDir (sh, "/afs/cern.ch/atlas/project/PAT/xAODs/r5597/");

  // set the name of the tree in our files
  // in the xAOD the TTree containing the EDM containers is "CollectionTree"
  sh.setMetaString ("nc_tree", "CollectionTree");

  // further sample handler configuration may go here

  // print out the samples we found
  sh.print ();

  // this is the basic description of our job
  EL::Job job;
  job.sampleHandler (sh);

  // add our algorithm to the job
  MainAnalysis *alg = new MainAnalysis;

  // later on we'll add some configuration options for our algorithm that go here

  job.algsAdd (alg);

  // make the driver we want to use:
  // this one works by running the algorithm directly:
  EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.

  // process the job using the driver
  driver.submit (job, submitDir);

}
