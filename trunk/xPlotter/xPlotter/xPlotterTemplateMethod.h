
#ifndef xPlotterTemplateMethod_xPlotterTemplateMethod_H
#define xPlotterTemplateMethod_xPlotterTemplateMethod_H

#include <xPlotter/xPlotterTuples.h>

#include "TF1.h"

class xPlotterTemplateMethod  {

 public:

  xPlotterTemplateMethod() {};

  ~xPlotterTemplateMethod() = default;


  void ApplyTemplateMethod(const string &target_category, // category to be built, eg. QCD jets
                           const string &source_category, // category used to build the target category
                           const vector<string> &subtraction_categories, // sample categories to be subtracted - if null then no subtraction
                           const vector<string> &template_regions, // required template regions with their prefilled histograms
                           const string &systematic,
                           const string &system,
                           const string &variable,
                           map< HistoVarCasesT, shared_ptr<TH1D> > hm) // parent histograms
  {

    

    //if(template_regions.size < 4) throw "ApplyTemplateMethod - insufficient number of template regions";

    auto region_A = template_regions[0]; // signal region
    auto region_B = template_regions[1]; // shape region
    auto region_C = template_regions[2]; // normalization region, numerator
    auto region_D = template_regions[3]; // normalization region, denominator

    std::string xcase = systematic + "_" + system + "_" + variable;

    cout<<"apply template method case: "<<xcase<<endl;

    //reference plot
    shared_ptr<TH1D> href( hm[ forward_as_tuple( target_category, systematic, region_A, system, variable) ] );

    //template regions
    double n = href->GetNbinsX();
    double x0 = href->GetBinLowEdge(1);
    double x1 = href->GetBinLowEdge(n) + href->GetBinWidth(1);

    std::shared_ptr<TH1D> hBtotal = std::make_shared<TH1D>( ("h_Btotal_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hBsubtract = std::make_shared<TH1D>( ("h_Bsubtract_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hB = std::make_shared<TH1D>( ("h_B_"+xcase).c_str(), "", n, x0, x1);

    std::shared_ptr<TH1D> hCtotal = std::make_shared<TH1D>( ("h_Ctotal_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hCsubtract = std::make_shared<TH1D>( ("h_Csubtract_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hC = std::make_shared<TH1D>( ("h_C_"+xcase).c_str(), "", n, x0, x1);

    std::shared_ptr<TH1D> hDtotal = std::make_shared<TH1D>( ("h_Dtotal_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hDsubtract = std::make_shared<TH1D>( ("h_Dsubtract_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hD = std::make_shared<TH1D>( ("h_D_"+xcase).c_str(), "", n, x0, x1);

    //ratio plot for normalization
    std::shared_ptr<TH1D> hR = std::make_shared<TH1D>( ("h_R_"+xcase).c_str(), "", n, x0, x1);

    //derived signal region: shape + normalization
    std::shared_ptr<TH1D> hA = std::make_shared<TH1D>( ("h_A_"+xcase).c_str(), "", n, x0, x1);

    //build total source categories
    hBtotal-> Add( hm[ forward_as_tuple(source_category,
                                        systematic,
                                        region_B,
                                        system,
                                        variable) ].get(),
                   1.);

    hCtotal-> Add( hm[ forward_as_tuple(source_category,
                                        systematic,
                                        region_C,
                                        system,
                                        variable) ].get(),
                   1.);

    hDtotal-> Add( hm[ forward_as_tuple(source_category,
                                        systematic,
                                        region_D,
                                        system,
                                        variable) ].get(),
                   1.);


    //build distributions for subtraction
    for ( const auto &cat : subtraction_categories ){
      hBsubtract-> Add( hm[ forward_as_tuple(cat,
                                             systematic,
                                             region_B,
                                             system,
                                             variable) ].get(),
                        1.);

      hCsubtract-> Add( hm[ forward_as_tuple(cat,
                                             systematic,
                                             region_C,
                                             system,
                                             variable) ].get(),
                        1.);

      hDsubtract-> Add( hm[ forward_as_tuple(cat,
                                             systematic,
                                             region_D,
                                             system,
                                             variable) ].get(),
                        1.);


    }

    //subtract regions
    hB -> Add( hBtotal.get(), 1.);  
    hC -> Add( hCtotal.get(), 1.);
    hD -> Add( hDtotal.get(), 1.);

    if(subtraction_categories.size()){
      cout<<"template method - subtracting categories "<<endl;
      hB -> Add( hBsubtract.get(), -1.);
      hC -> Add( hCsubtract.get(), -1.);
      hD -> Add( hDsubtract.get(), -1.);
    }

    //ratio
    hR->Divide(hC.get(), hD.get(), 1., 1., "");

    cout<<"template method statistics "<<endl;
    cout<<"\t B "<<hB->Integral() <<" = "<< hBtotal->Integral() << " - " << hBsubtract->Integral()<<endl;
    cout<<"\t C "<<hC->Integral() <<" = "<< hCtotal->Integral() << " - " << hCsubtract->Integral()<<endl;
    cout<<"\t D "<<hD->Integral() <<" = "<< hDtotal->Integral() << " - " << hDsubtract->Integral()<<endl;


    //normalization factor
    auto fitfn = "fit_"+xcase;
    std::shared_ptr<TF1> fit = std::shared_ptr<TF1>( new TF1( fitfn.c_str(),
							      "pol0",
							      x0,
							      x1)
						     );
    fit->SetFillColor(19);
    fit->SetFillStyle(0);
    fit->SetLineColor(2);
    fit->SetLineWidth(2);
    fit->SetParameter(0, 1);
    fit->SetParError(0, 1);
    fit->SetParLimits(0, 0, 2);
    hR->Fit(fitfn.c_str(), "R", "goff");
    double par0 = fit->GetParameter(0);
    cout<<"template method ratio par0 = "<<par0<<endl;
    hR->SaveAs("hR.C");

    par0 = 0.38;     cout<<"template method ratio par0 hacked to be  = "<<par0<<endl;

    hA -> Add( hB.get(), par0);


    //archive the results
    hm[ forward_as_tuple(target_category,
                         systematic,
                         region_A,
                         system,
                         variable) ] ->Add( hA.get(), 1.);

    hm[ forward_as_tuple(target_category,
                         systematic,
                         region_B,
                         system,
                         variable) ] ->Add( hBsubtract.get(), 1.);

    hm[ forward_as_tuple(target_category,
                         systematic,
                         region_C,
                         system,
                         variable) ] ->Add( hCsubtract.get(), 1.);

    hm[ forward_as_tuple(target_category,
                         systematic,
                         region_D,
                         system,
                         variable) ] ->Add( hDsubtract.get(), 1.);


    fit.reset(); fit = nullptr;
  }

 private:




};

#endif
