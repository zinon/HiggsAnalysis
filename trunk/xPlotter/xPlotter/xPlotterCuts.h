
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

  auto RegionsV( ) -> vector<string> {

    vector<string> v;

    for(const auto &el : m_CutsM) {
      auto region = get<2>(el.first);
      cout<<"RegionsV "<<region<<endl;
      if ( find(v.begin(), v.end(), region ) == v.end())
	v.push_back(region);
    }

    return v;
  }


  auto TemplateRegions() -> vector<string> {

    return m_template_regions;
  }

 private:

  TCut c_core = "tau_muon && leading_tau_pt > 25 && leading_muon_pt > 22. && !subleading_muon  && !subleading_tau";
  TCut c_tau_muon_A = "tau_muon_qxq < 0 && leading_tau_JetBDT_tight" + c_core;
  TCut c_tau_muon_B = "tau_muon_qxq > 0 && leading_tau_JetBDT_tight" + c_core;
  TCut c_tau_muon_C = "tau_muon_qxq < 0 && !leading_tau_JetBDT_tight" + c_core;
  TCut c_tau_muon_D = "tau_muon_qxq > 0 && !leading_tau_JetBDT_tight" + c_core;

  vector<string> m_template_regions { // in order: signal region, shape region, normalization enumerator, normalization denominator
    "SignalRegion", "SSIDRegion", "OSAntiIDRegion", "SSAntiIDRegion"
  };

  map<CutCasesT, TCut> m_CutsM {

    // tau-muon : tau pt
    {
      make_tuple( "leading_tau_pt", "tau_muon", "SignalRegion"),
	c_tau_muon_A
	},


    {
      make_tuple( "leading_tau_pt", "tau_muon", "SSIDRegion"),
	c_tau_muon_B
	},

    {
      make_tuple( "leading_tau_pt", "tau_muon", "OSAntiIDRegion"),
	c_tau_muon_C
	},

    {
      make_tuple( "leading_tau_pt", "tau_muon", "SSAntiIDRegion"),
	c_tau_muon_D
    }

    // tau-muon : tau 


  };

  map<WeightCaseT, TCut> m_WeightsM {
    
    {
      make_tuple( "leading_tau_pt", "tau_muon"),
      "weight_total *leading_tau_JetBDT_eff_sf * leading_muon_eff_sf"
    }

  };





};

#endif
