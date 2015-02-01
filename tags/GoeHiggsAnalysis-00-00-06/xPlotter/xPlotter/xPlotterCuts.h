
#ifndef xPlotterCuts_xPlotterCuts_H
#define xPlotterCuts_xPlotterCuts_H

#include <xPlotter/xPlotterBank.h>

#include <xPlotter/xPlotterTuples.h>

class xPlotterCuts : virtual public xPlotterBank {

 public:
  
  xPlotterCuts() {};
  
  ~xPlotterCuts() = default;

  auto CutMC( const HistoVarCasesT &var ) -> TCut {

    TCut c = Weight( get<0>(var) ) // sample weights, cross-section, ...
      * m_CutsM[ forward_as_tuple( get<4>(var), get<3>(var), get<2>(var) ) ] //cuts
      * m_WeightsM[ forward_as_tuple( get<4>(var), get<3>(var)) ]; // analysis weights, scale factors, ...

    cout<<"xPlotterCuts - mc tcut weight: "<<c.GetTitle()<<endl;
    
    return c;
  };

  auto CutData( const HistoVarCasesT &var ) -> TCut {

    TCut c =  m_CutsM[ forward_as_tuple( get<4>(var), get<3>(var), get<2>(var) ) ]; //cuts

    cout<<"xPlotterCuts - data tcut : "<<c.GetTitle()<<endl;
    
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
