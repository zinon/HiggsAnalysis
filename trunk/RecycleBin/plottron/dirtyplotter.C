#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "THStack.h"
#include "TCut.h"
#include <iostream>
#include "TLegend.h"


//void plotHistos(TChain &sahmpell){
 
//  sahmpell->;  
  
//  }


void dirtyplotter() {
TFile *fztt= new TFile();

TChain *cztt=new TChain ("NOMINAL"); //0.031747590	

cztt->Add("./Out_Ztt/hist-ztt1.root");
cztt->Add("./Out_Ztt/hist-ztt2.root");
cztt->Add("./Out_Ztt/hist-ztt3.root");
cztt->Add("./Out_Ztt/hist-ztt4.root");
cztt->Add("./Out_Ztt/hist-ztt5.root");

//TChain *czmmIncl=new TChain ("NOMINAL");
//czmmIncl->Add("./Out_ZmmIncl/hist-zmmincl.root");


TChain *czmmBF=new TChain ("NOMINAL");
// 0.011452932
czmmBF->Add("./Out_Zmm/hist-zmmBF1.root");
czmmBF->Add("./Out_Zmm/hist-zmmBF2.root");
czmmBF->Add("./Out_Zmm/hist-zmmBF3.root");

TChain *czmmCFBV=new TChain ("NOMINAL");
// 0.151389286
czmmCFBV->Add("./Out_Zmm/hist-zmmCFBV1.root");
czmmCFBV->Add("./Out_Zmm/hist-zmmCFBV2.root");

TChain *czmmCVBV=new TChain ("NOMINAL");
// 0.307079469
czmmCVBV->Add("./Out_Zmm/hist-zmmCVBV1.root");
czmmCVBV->Add("./Out_Zmm/hist-zmmCVBV2.root");
czmmCVBV->Add("./Out_Zmm/hist-zmmCVBV3.root");

TChain *cttbar=new TChain ("NOMINAL"); //0.009238164
cttbar->Add("./Out_Ttbar/hist-ttbar1.root");
cttbar->Add("./Out_Ttbar/hist-ttbar2.root");


 TChain *cwmmunu=new TChain ("NOMINAL"); // 0.339 
 cwmmunu->Add("./Out_Wmunu/hist-wmmuv.root");
 
 TChain *cwpmunu=new TChain ("NOMINAL"); //0.426
 cwpmunu->Add("./Out_Wmunu/hist-wpmuv.root");

TChain *cdata=new TChain ("NOMINAL");
cdata->Add("./Out_Data/hist-data202798_1.root");
cdata->Add("./Out_Data/hist-data202798_2.root");
cdata->Add("./Out_Data/hist-data202798_3.root");
cdata->Add("./Out_Data/hist-data202798_4.root");
cdata->Add("./Out_Data/hist-data202798_5.root");




////plotHistos(czmm);

TString var="met_et";
//TString var="tau_muon_m_vis";
//TString var="leading_tau_pt";
//TString var="leading_jet_pt";
//TString var="leading_muon_pt";
//TString var="subleading_tau_pt";

//TString var="leading_tau_eta";
//TString var="tau_muon_dR";
//TString var="tau_muon_dphi";
//TString var="leading_tau_ntrks";
//TString var="leading_tau_JetBDT_score";

TString hname="hist";
TString hrange="(30,10,160)";
//TString hrange="(30,0,150)";
//TString hrange="(80,-4,4)";
//TString hrange="(40,0,1)";
//TString hrange="(50,0,5)";
TString hxtitle="met_et (GeV)";
//TString hxtitle="DPhi(tau,mu)"; 
//TString hxtitle="DR(tau,mu)"; 
//TString hxtitle="m_vis (GeV)";
//TString hxtitle="pT(jet) (GeV)";
//TString hxtitle="pT(tau) (GeV)";
//TString hxtitle="pT(muon) (GeV)";
//TString hxtitle="Jet BDT";
//TString hxtitle="N_tracks";
TString hytitle="#Entries";

TCut schnippelschnapp="(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon  && !subleading_tau)"; 

TFile *outfile=new TFile("out.root","RECREATE");


cdata->Draw(var+" >> data_"+hname+hrange,"(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon  && !subleading_tau  && leading_tau_JetBDT_tight)");
TH1D *hdata=(TH1D*) gDirectory->Get("data_"+hname);

//190000+489600+400000+420000+232500 = 1732100
 cztt->Draw(var+" >> ztt_"+hname+hrange, "0.031747590*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)"); //0.031747590		
TH1D *hztt=(TH1D*) gDirectory->Get("ztt_"+hname);

czmmBF->Draw(var+" >> zmmBF_"+hname+hrange, "0.0114529320*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
TH1D *hzmmBF=(TH1D*) gDirectory->Get("zmmBF_"+hname);

czmmCFBV->Draw(var+" >> zmmCFBV_"+hname+hrange, "0.1513892860*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
TH1D *hzmmCFBV=(TH1D*) gDirectory->Get("zmmCFBV_"+hname);

czmmCVBV->Draw(var+" >> zmmCVBV_"+hname+hrange, "0.3070794690*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
TH1D *hzmmCVBV=(TH1D*) gDirectory->Get("zmmCVBV_"+hname);

//Zttincl 1045000 -> *0.05
//czmmIncl->Draw(var+" >> zmmIncl_"+hname+hrange, "0.05*weight_total*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
//TH1D *hzmmIncl=(TH1D*) gDirectory->Get("zmmIncl_"+hname);

cwmmunu->Draw(var+" >> wmmunu_"+hname+hrange, "0.339*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
TH1D *hwmmunu=(TH1D*) gDirectory->Get("wmmunu_"+hname);

cwpmunu->Draw(var+" >> wpmunu_"+hname+hrange, "0.426*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
 TH1D *hwpmunu=(TH1D*) gDirectory->Get("wpmunu_"+hname);

cttbar->Draw(var+" >> ttbar_"+hname+hrange,"0.009238164*weight_total*leading_tau_JetBDT_eff_sf*leading_muon_eff_sf*(tau_muon && leading_tau_pt > 22 && leading_muon_pt > 25. && !subleading_muon && !subleading_tau  && leading_tau_JetBDT_tight)");
TH1D *httbar=(TH1D*) gDirectory->Get("ttbar_"+hname);

TH1D *hzmm=new TH1D();
hzmm=hzmmBF;
hzmm->Add(hzmmCFBV);
hzmm->Add(hzmmCVBV);

 TH1D *hwmunu=new TH1D();
 hwmunu=hwpmunu;
 hwmunu->Add(hwmmunu);

httbar->SetFillColor(kBlue);
hzmm->SetFillColor(kGreen);
//hzmmIncl->SetFillColor(kGreen);
hztt->SetFillColor(kMagenta);
hwmunu->SetFillColor(kCyan);

httbar->SetLineColor(kBlue+1);
hzmm->SetLineColor(kGreen+1);
//hzmmIncl->SetLineColor(kGreen+1);
hztt->SetLineColor(kMagenta+1);
hwmunu->SetLineColor(kCyan+1);

httbar->SetLineWidth(2.0);
hzmm->SetLineWidth(2.0);
//hzmmIncl->SetLineWidth(1.5);
hztt->SetLineWidth(2.0);
hwmunu->SetLineWidth(2.0);

 hdata->SetMarkerStyle(20);
 hdata->SetMarkerSize(1.0);


TLegend* l = new TLegend(0.6,0.6,0.89,0.89);

THStack *stack=new THStack("stack","xAOD EDM: "+var);
stack->Add(httbar);
stack->Add(hzmm);
stack->Add(hwmunu);
//stack->Add(hzmmIncl);
stack->Add(hztt);

l->AddEntry(hztt,"PowHegPythia8 Z#tau#tau","LP");
//l->AddEntry(hzmmIncl,"PowHegPythia8 Z#mu#mu","LP");
l->AddEntry(hzmm,"Sherpa Z#mu#mu","LP");
 l->AddEntry(hwmunu,"PowhegPythia W#mu#nu","LP");
l->AddEntry(httbar,"PowHegPythia8 ttbar","LP");
l->AddEntry(hdata,"Data","LP");


l->SetFillColor(0);
l->SetFillStyle(0);
l->SetShadowColor(0);
l->SetLineColor(0);
TCanvas *canv=new TCanvas();

hdata->Draw("ep");
stack->Draw("SAME");

stack->GetXaxis()->SetTitle(hxtitle);
stack->GetYaxis()->SetTitle(hytitle);
//httbar->Draw();
l->Draw("SAME");

hdata->Draw("ep SAME");
canv->SaveAs("PLots/"+var+".png","png");
outfile->cd();
hztt->Write();
hzmm->Write();
 hwmunu->Write();
//hzmmIncl->Write();
httbar->Write();
hdata->Write();
stack->Write();
// outfile.Close();
}
