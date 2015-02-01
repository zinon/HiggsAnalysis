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
      
  {"weight_mc_samplesum_init", -999},
  {"weight_mc_samplesum_fin", -999},
  
  
  {"actual_int_per_bunch_crossing", -999},
  {"average_int_per_bunch_crossing", -999},
  {"numPV",-999},

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
  {"leading_tau_met_mt", -999},

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
  {"leading_muon_met_mt", -999},

  {"subleading_muon", -999},
  {"subleading_muon_pt", -999},
  {"subleading_muon_eta", -999},
  {"subleading_muon_phi", -999},
  {"subleading_muon_Q", -999},
  {"subleading_muon_eff_sf", -999},
  
  //DAN
  {"electrons_N", -999},

  {"leading_electron", -999},
  {"leading_electron_pt", -999},
  {"leading_electron_eta", -999},
  {"leading_electron_phi", -999},
  {"leading_electron_Q", -999},
  {"leading_electron_eff_sf", -999},
  {"leading_electron_met_mt", -999},

  {"subleading_electron", -999},
  {"subleading_electron_pt", -999},
  {"subleading_electron_eta", -999},
  {"subleading_electron_phi", -999},
  {"subleading_electron_Q", -999},

  {"jets_N", -999},

  {"leading_jet", -999},
  {"leading_jet_pt", -999},
  {"leading_jet_eta", -999},
  {"leading_jet_phi", -999},
  {"leading_jet_y", -999},
  {"leading_jet_jvf", -999},

  {"subleading_jet", -999},
  {"subleading_jet_pt", -999},
  {"subleading_jet_eta", -999},
  {"subleading_jet_phi", -999},
  {"subleading_jet_y", -999},
  {"subleading_jet_jvf", -999},

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
  {"tau_muon_col", -999},
  {"tau_muon_m_col", -999},
  {"tau_muon_x1_col", -999},
  {"tau_muon_x2_col", -999},
  {"tau_muon_vect_sum_pt", -999},
  {"tau_muon_scal_sum_pt", -999},
  {"tau_muon_met_bisect", -999},
  {"tau_muon_met_min_dphi", -999},
  {"tau_muon_met_centrality", -999},

  {"tau_electron", -999},
  {"tau_electron_deta", -999},
  {"tau_electron_dphi", -999},
  {"tau_electron_dR", -999},
  {"tau_electron_cosalpha", -999},
  {"tau_electron_qxq", -999},
  {"tau_electron_m_vis", -999},
  {"tau_electron_col", -999},
  {"tau_electron_m_col", -999},
  {"tau_electron_x1_col", -999},
  {"tau_electron_x2_col", -999},
  {"tau_electron_vect_sum_pt", -999},
  {"tau_electron_scal_sum_pt", -999},
  {"tau_electron_met_bisect", -999},
  {"tau_electron_met_min_dphi", -999},
  {"tau_electron_met_centrality", -999},
  
  {"dimuon", -999},
  {"dimuon_deta", -999},
  {"dimuon_dphi", -999},
  {"dimuon_dR", -999},
  {"dimuon_cosalpha", -999},
  {"dimuon_qxq", -999},
  {"dimuon_m_vis", -999},
  {"dimuon_col", -999},
  {"dimuon_m_col", -999},
  {"dimuon_x1_col", -999},
  {"dimuon_x2_col", -999},
  {"dimuon_vect_sum_pt", -999},
  {"dimuon_scal_sum_pt", -999},
  {"dimuon_met_bisect", -999},
  {"dimuon_met_min_dphi", -999},
  {"dimuon_met_centrality", -999},

  //DAN
  {"dielectron", -999},
  {"dielectron_deta", -999},
  {"dielectron_dphi", -999},
  {"dielectron_dR", -999},
  {"dielectron_cosalpha", -999},
  {"dielectron_qxq", -999},
  {"dielectron_m_vis", -999},
  {"dielectron_col", -999},
  {"dielectron_m_col", -999},
  {"dielectron_x1_col", -999},
  {"dielectron_x2_col", -999},
  {"dielectron_vect_sum_pt", -999},
  {"dielectron_scal_sum_pt", -999},
  {"dielectron_met_bisect", -999},
  {"dielectron_met_min_dphi", -999},
  {"dielectron_met_centrality", -999},

  //DAN
  {"muon_electron", -999},
  {"muon_electron_deta", -999},
  {"muon_electron_dphi", -999},
  {"muon_electron_dR", -999},
  {"muon_electron_cosalpha", -999},
  {"muon_electron_qxq", -999},
  {"muon_electron_m_vis", -999},
  {"muon_electron_col", -999},
  {"muon_electron_m_col", -999},
  {"muon_electron_x1_col", -999},
  {"muon_electron_x2_col", -999},
  {"muon_electron_vect_sum_pt", -999},
  {"muon_electron_scal_sum_pt", -999},
  {"muon_electron_met_bisect", -999},
  {"muon_electron_met_min_dphi", -999},
  {"muon_electron_met_centrality", -999},

  
  {"ditau", -999},
  {"ditau_deta", -999},
  {"ditau_dphi", -999},
  {"ditau_dR", -999},
  {"ditau_cosalpha", -999},
  {"ditau_qxq", -999},
  {"ditau_m_vis", -999},
  {"ditau_col", -999},
  {"ditau_m_col", -999},
  {"ditau_x1_col", -999},
  {"ditau_x2_col", -999},
  {"ditau_vect_sum_pt", -999},
  {"ditau_scal_sum_pt", -999},
  {"ditau_met_bisect", -999},
  {"ditau_met_min_dphi", -999},
  {"ditau_met_centrality", -999},

  {"dijet", -999},
  {"dijet_deta", -999},
  {"dijet_dy", -999},
  {"dijet_dphi", -999},
  {"dijet_dR", -999},
  {"dijet_cosalpha", -999},
  {"dijet_m_vis", -999},
  {"dijet_vect_sum_pt", -999},
  {"dijet_scal_sum_pt", -999},
  {"dijet_etaxeta", -999},

  {"dummy", -999}

};


#endif