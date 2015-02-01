#ifndef MainAnalysis_MainAnalysisVariables_H
#define MainAnalysis_MainAnalysisVariables_H

std::map<std::string, double> m_tree_var  {

  /*  {"leading_tau_pt", double()},
  {"leading_tau_eta", double()},
  {"subleading_tau_pt", double()},
  {"subleading_tau_eta", double()},
  */

  {"weight_mc", -999},
  {"weight_pileup", -999},
  {"weight_total", -999},

  {"actual_int_per_bunch_crossing", -999},
  {"average_int_per_bunch_crossing", -999},


  {"taus_N", -999},

  {"leading_tau", -999},
  {"leading_tau_pt", -999},
  {"leading_tau_eta", -999},
  {"leading_tau_phi", -999},
  {"leading_tau_Q", -999},
  {"leading_tau_ntrks", -999},
  {"leading_tau_JetBDT_score", -999},
  {"leading_tau_JetBDT_loose", -999},
  {"leading_tau_JetBDT_medium", -999},
  {"leading_tau_JetBDT_tight", -999},
  {"leading_tau_JetBDT_eff_sf", -999},

  {"subleading_tau", -999},
  {"subleading_tau_pt", -999},
  {"subleading_tau_eta", -999},
  {"subleading_tau_phi", -999},
  {"subleading_tau_Q", -999},
  {"subleading_tau_ntrks", -999},
  {"subleading_tau_JetBDT_score", -999},
  {"subleading_tau_JetBDT_loose", -999},
  {"subleading_tau_JetBDT_medium", -999},
  {"subleading_tau_JetBDT_tight", -999},
  {"subleading_tau_JetBDT_eff_sf", -999},

  {"muons_N", -999},

  {"leading_muon", -999},
  {"leading_muon_pt", -999},
  {"leading_muon_eta", -999},
  {"leading_muon_phi", -999},
  {"leading_muon_Q", -999},
  {"leading_muon_eff_sf", -999},

  {"subleading_muon", -999},
  {"subleading_muon_pt", -999},
  {"subleading_muon_eta", -999},
  {"subleading_muon_phi", -999},
  {"subleading_muon_Q", -999},
  {"subleading_muon_eff_sf", -999},

  {"jets_N", -999},

  {"leading_jet", -999},
  {"leading_jet_pt", -999},
  {"leading_jet_eta", -999},
  {"leading_jet_phi", -999},

  {"subleading_jet", -999},
  {"subleading_jet_pt", -999},
  {"subleading_jet_eta", -999},
  {"subleading_jet_phi", -999},

  {"met_et", -999},
  {"met_phi", -999},
  {"met_sumet", -999},

  {"tau_muon", -999},
  {"tau_muon_deta", -999},
  {"tau_muon_dphi", -999},
  {"tau_muon_dR", -999},
  {"tau_muon_cosalpha", -999},
  {"tau_muon_qxq", -999},
  {"tau_muon_m_vis", -999},
  {"tau_muon_m_eff", -999},
  {"tau_muon_m_col", -999},
  {"tau_muon_vect_sum_pt", -999},
  {"tau_muon_scal_sum_pt", -999},
  {"tau_muon_met_bisect", -999},
  {"tau_muon_met_min_dphi", -999},

  {"dimuon", -999},
  {"dimuon_deta", -999},
  {"dimuon_dphi", -999},
  {"dimuon_dR", -999},
  {"dimuon_cosalpha", -999},
  {"dimuon_qxq", -999},
  {"dimuon_m_vis", -999},
  {"dimuon_m_eff", -999},
  {"dimuon_m_col", -999},
  {"dimuon_vect_sum_pt", -999},
  {"dimuon_scal_sum_pt", -999},
  {"dimuon_met_bisect", -999},
  {"dimuon_met_min_dphi", -999},

  {"ditau", -999},
  {"ditau_deta", -999},
  {"ditau_dphi", -999},
  {"ditau_dR", -999},
  {"ditau_cosalpha", -999},
  {"ditau_qxq", -999},
  {"ditau_m_vis", -999},
  {"ditau_m_eff", -999},
  {"ditau_col", -999},
  {"ditau_m_col", -999},
  {"ditau_x1_col", -999},
  {"ditau_x2_col", -999},
  {"ditau_vect_sum_pt", -999},
  {"ditau_scal_sum_pt", -999},
  {"ditau_met_bisect", -999},
  {"ditau_met_min_dphi", -999},

  {"dummy", -999}

};


#endif
