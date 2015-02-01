
#ifndef xPlotterCuts_xPlotterCuts_H
#define xPlotterCuts_xPlotterCuts_H

#include <xPlotter/xPlotterBank.h>

#include <xPlotter/xPlotterTuples.h>

class xPlotterCuts : virtual public xPlotterBank {

 public:
  
  xPlotterCuts() {};
  
  ~xPlotterCuts() = default;

  auto Cut( const HistoVarCasesT &var ) -> TCut {
    
    //int number; string variable; string system; string region;
    //    std::tie () = tup;
    /*
    typedef tuple<int, string> tup_t;
    map<tup_t, TCut> m;
    m[std::make_tuple(1, "test")] = "tau_muon";
    auto itr = m.find(std::make_tuple(1, "test"));
    if( itr != m.end() ) cout<<itr->second<<endl;
    */

    //    auto itr = m.find(std::make_tuple(1, "test"));
    //if( itr != m.end() ) cout<<itr->second<<endl;

    /*
  23  string, //type (signal or background) or sample id                                                                                                                           
  24  string, //systematic variation                                                                                                                                               
  25  string, //region                                                                                                                                                             
  26  string, //system                                                                                                                                                             
  27  string  //variable                                                                                                                                                           
  28  >;
    */

  TCut c = Weight( get<0>(var) ) 
    * m_CutsM[ forward_as_tuple( get<4>(var), get<3>(var), get<2>(var) ) ] 
    * m_WeightsM[ forward_as_tuple( get<4>(var), get<3>(var)) ];

    cout<<"tcut "<<c.GetTitle()<<endl;

    return c;
  };

 private:

  map<CutCasesT, TCut> m_CutsM {
    
    {
      make_tuple( "leading_tau_pt", "tau_muon", "SignalRegion"),
      "tau_muon && tau_muon_qxq < 0 && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon  && !subleading_tau  && leading_tau_JetBDT_tight"
    }


  };

  map<WeightCaseT, TCut> m_WeightsM {
    
    {
      make_tuple( "leading_tau_pt", "tau_muon"),
      "weight_total *leading_tau_JetBDT_eff_sf * leading_muon_eff_sf"
    }

  };





};

#endif
