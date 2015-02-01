/* 
A job options class ZZ
 - designed to parse job options from a text input file to a map of strings 
 - strings are transformed according to the upstream call function, eg 
 - m_jo->get<bool>(xJobOptions::DO_PILEUP_FILE) 
*/

#ifndef xJobOptions_xJobOptions_H
#define xJobOptions_xJobOptions_H

#include <type_traits>
#include <iostream>
#include <utility>
#include <map>
#include <stdlib.h>  //atof, atoi
#include <stdio.h> //strcmp
#include <string.h>
#include <string>
#include <exception>


//EDM includes
#include "TauAnalysisTools/Enums.h"

//ASG includes
#include "AsgTools/AsgTool.h"


class mapexc
{

 public:
  /*
  mapexc();
  
   template<typename T> mapexc(const T &p) {
    
    c += static_cast<std::string>(p);
  }

  virtual ~mapexc() = default;

  virtual const char* what() const throw()
  {
    return c.c_str();
  }

 private:
  std::string c = "Map key not found : ";
  */

  virtual const char* what() const throw()
  {
    return "Job options map key not found !";
  }

} mapexc;

class xJobOptions {

 public:

  xJobOptions() {} // default constructor:  does not override the file initialization

  explicit xJobOptions(const std::string &filename) : m_filename(filename) {} //ctor with input

  virtual ~xJobOptions() = default; //auto generated dtor

  bool Configure(); //to be called

  //templated retrieve functions
  template<typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0, typename U>
    T get(const U& t)
    {
      if (std::is_same<T, float>::value){ //float
        return std::stof( mapConstRef( static_cast<OptionType>(t) ).c_str() );    //since C++11
      }
      if (std::is_same<T, double>::value){ //double
        return std::stod( mapConstRef( static_cast<OptionType>(t) ).c_str() ); //since C++11
      }
      return atof( mapConstRef( static_cast<OptionType>(t) ).c_str() );   //stdlib

    }

  template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0, typename U>
    T get(const U& t)
    {
      if (std::is_same<T, int>::value){ //integer_type, since C++11
        return std::stof( mapConstRef( static_cast<OptionType>(t) ).c_str() );
      }
      return atoi( mapConstRef( static_cast<OptionType>(t) ).c_str() );//stdlib
    }

  template <typename T, typename std::enable_if< std::is_same<T, std::string>::value, int >::type = 0, typename U>
    T get(const U t ){
    
    return mapConstRef( static_cast<OptionType>(t) );
  }

  /*this is ok also
    template <typename T, typename std::enable_if< std::is_same<T, int>::value, int >::type = 0, typename U>
    T foo(const U t ){
    std::cout << "int type with arg " <<t<< std::endl;
    std::string s = "12";
    return atoi(s.c_str());
    }
  */

  auto TauJetBDTindex(const std::string &) const -> TauAnalysisTools::e_JETID;
  auto TauEleBDTindex(const std::string &) const -> TauAnalysisTools::e_ELEID;


  typedef enum OptionType {
    DO_PILEUP_FILE,
    DO_TEST_RUN,
    DO_PILEUP_REWEIGHTING,
    DO_SYSTEMATICS,
    DO_HISTOS,
    DO_DERIVATION,

    JET_COLLECTION,
    JET_MIN_PT,
    JET_MAX_ABS_ETA,
    JET_MIN_ABS_JVF,
    JET_JVF_CUT_MAX_ABS_ETA,
    JET_RECALIBRATION,
    JET_JES_CORRECTION,
    JET_JER_CORRECTION,
    JET_CLEANING,
    JET_PRESELECT_SKIP_CALIB_LOW_PT,
    JET_PRESELECT_CALIB_MIN_PT,

    TAU_SELECTION_RECOMMENDED,
    TAU_MIN_PT,
    TAU_JET_BDT,
    TAU_ELE_BDT,
    TAU_JET_OVERLAP_DR,
    TAU_MUON_OVERLAP_DR,
    TAU_ELE_OVERLAP_DR,
    TAU_CORRECTION,

    MUON_MIN_PT,
    MUON_MAX_ISO,
    MUON_OVERLAP_MIN_PT,
    MUON_OVERLAP_MAX_ABS_ETA,
    MUON_CALIBRATION_SMEARING,
    MUON_JET_OVERLAP_DR,

    MET_CONTAINER,
    MET_JETCOL,
    MET_TAUCOL,
    MET_MUONCOL,
    MET_ELECOL,
    MET_JETTERM,
    MET_TAUTERM,
    MET_MUONTERM,
    MET_ELETERM,
    MET_SOFTTERM,
    MET_GAMMATERM,
    MET_OUTMETCONT,
    MET_OUTMETTERM,
    MET_JETPTCUT,
    MET_JETDOJVF,
    
    //DAN: electron JO
    ELE_ID,
    ELE_PT,
    ELE_ETA_CRACK_LO,
    ELE_ETA_CRACK_HI,
    ELE_ETA_MAX,
    ELE_ISO_ETCONE20,
    ELE_ISO_PTCONE40,
    ELE_CALIBRATION_SMEARING,
    ELE_MUON_OVERLAP_DR,
    ELE_ID_FOR_TAUOVERLAP,
    ELE_JET_OVERLAP_DR,

    DUMMY
  } OptionType;

 protected:

  auto getFileName() const -> std::string { return m_filename;}

  void setJobOptions(FILE *, std::string &, const char *);

 private:

  const std::string & mapConstRef(const OptionType&) const;

  std::string & mapRef(const OptionType&);

  //C++11 allows non-static data members to be initialized where it is declared (in their class)
  std::string m_filename = "$ROOTCOREBIN/data/MainAnalysis/JobOptions/default";

  std::map<OptionType, std::string> m_options {
   
    {DO_PILEUP_FILE, "0"},
    {DO_TEST_RUN, "0"},
    {DO_PILEUP_REWEIGHTING, "0"},
    {DO_SYSTEMATICS, "0"},
    {DO_HISTOS, "0"},
    {DO_DERIVATION, "0"},

    {JET_COLLECTION, "AntiKt4LCTopo"},
    {JET_MIN_PT, "20."},
    {JET_MAX_ABS_ETA, "4.5"},
    {JET_MIN_ABS_JVF, "0.25"},
    {JET_JVF_CUT_MAX_ABS_ETA, "2.4"},
    {JET_RECALIBRATION, "1"},
    {JET_JES_CORRECTION, "1"},
    {JET_JER_CORRECTION, "1"},
    {JET_CLEANING, "VeryLooseBad"},
    {JET_PRESELECT_SKIP_CALIB_LOW_PT, "1"},
    {JET_PRESELECT_CALIB_MIN_PT, "15."},

    {TAU_SELECTION_RECOMMENDED, "1"},
    {TAU_MIN_PT, "20"},
    {TAU_JET_BDT, "JETIDBDTMEDIUM"},
    {TAU_ELE_BDT, "ELEIDBDTLOOSE"},
    {TAU_JET_OVERLAP_DR, "0.2"},
    {TAU_MUON_OVERLAP_DR, "0.2"},
    {TAU_ELE_OVERLAP_DR, "0.2"},
    {TAU_CORRECTION, "0"},

    {MUON_MIN_PT, "20."},
    {MUON_MAX_ISO, "0.1"},
    {MUON_OVERLAP_MIN_PT, "2.0"},
    {MUON_OVERLAP_MAX_ABS_ETA, "2.5"},
    {MUON_CALIBRATION_SMEARING, "1"},
    {MUON_JET_OVERLAP_DR, "0.2"},

    {MET_CONTAINER, "MET_RefFinal"},
    {MET_JETCOL, "AntiKt4LCTopoCalibJets"},
    {MET_TAUCOL, "CalibTaus"},
    {MET_MUONCOL, "CalibMuons"},
    {MET_ELECOL, "CalibElectrons"},
    {MET_JETTERM, "RefJet"},
    {MET_TAUTERM, "RefTau"},
    {MET_MUONTERM, "Muons"},
    {MET_ELETERM, "RefEle"},
    {MET_SOFTTERM, "SoftClus"},
    {MET_GAMMATERM, "RefGamma"},
    {MET_OUTMETCONT, "MET_Calib"},
    {MET_OUTMETTERM, "Final"},
    {MET_JETPTCUT, "0."},
    {MET_JETDOJVF, "0"},
    
    //DAN: electron JO
    {ELE_ID,                    "Medium"},
    {ELE_PT,                    "15"},
    {ELE_ETA_CRACK_LO,          "1.37"},
    {ELE_ETA_CRACK_HI,          "1.52"},
    {ELE_ETA_MAX,               "2.47"},
    {ELE_ISO_ETCONE20,          "0.09"},
    {ELE_ISO_PTCONE40,          "0.17"},
    {ELE_CALIBRATION_SMEARING,  "1"},
    {ELE_MUON_OVERLAP_DR,       "0.2"},
    {ELE_ID_FOR_TAUOVERLAP,     "Loose"},
    {ELE_JET_OVERLAP_DR,        "0.2"}

  };

};

#endif
