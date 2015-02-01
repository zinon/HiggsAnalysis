The xJobOptions is a parser class. It can read a two-column text file and assign parsed values into an enumerated map .
 
The idea behind all this story is that now we are able to use a pointer to this class instead of inheriting its public attributes,
 which is at the end of the day a bad practice in C++ programming.

A text-to-string parser function is used to parse the input job options to a private  map<enumerated value, string> map. 
The upstream call templated function "get" can then retrieve any parsed variable and transform it according to the needs of the main program.

How it works:

in the xJobOptions/xJobOptions/xJobOptions.h you insert your enumerated cuts
  typedef enum OptionType {
    DO_PILEUP_FILE,
    DO_TEST_RUN,
    DO_PILEUP_REWEIGHTING
...

and the default values in a map
  std::map<OptionType, std::string> m_options {
   
    {DO_PILEUP_FILE, "0"},
    {DO_TEST_RUN, "0"},
    {DO_PILEUP_REWEIGHTING, "0"},
    {DO_SYSTEMATICS, "0"},
    {DO_HISTOS, "0"},
...


All variables are parsed as strings to the local variables
 try{

    setJobOptions(jobOptFile, mapRef(DO_PILEUP_FILE), "createPUfile");
    setJobOptions(jobOptFile, mapRef(DO_TEST_RUN), "testRun");
    setJobOptions(jobOptFile, mapRef(DO_PILEUP_REWEIGHTING), "doPileupReweighting");
    setJobOptions(jobOptFile, mapRef(DO_SYSTEMATICS), "doSystematics");
    setJobOptions(jobOptFile, mapRef(DO_HISTOS), "doHistos");
...

and transformed to any type according to your desire via the templated enable_if "get" functions
 //templated retrieve functions
  template<typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0, typename U>
    T get(const U& t)
    {
...

as follows

  if(m_jo->get<bool>(xJobOptions::DO_PILEUP_FILE)){
..

or

  if( m_jo->get<double>(xJobOptions::JET_MIN_PT) > 15.){
...

etc.

The advantage is that now you can retrieve a variable in any type, eg double -> string

m_jo->get<std::string>(xJobOptions::JET_MIN_PT)

etc...

In case of problems, drop an email to zenon@cern.ch or edrechsler.work@gmail.com
