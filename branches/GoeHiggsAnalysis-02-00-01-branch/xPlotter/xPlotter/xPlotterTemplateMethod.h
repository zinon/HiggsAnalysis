
#ifndef xPlotterTemplateMethod_xPlotterTemplateMethod_H
#define xPlotterTemplateMethod_xPlotterTemplateMethod_H

#include <xPlotter/xPlotterTuples.h>

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

    /*
    const string variable = "leading_tau_pt";
    const string systematic = "NOMINAL";
    const string system = "tau_muon";
    */

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

    std::shared_ptr<TH1D> hR = std::make_shared<TH1D>( ("h_R_"+xcase).c_str(), "", n, x0, x1);

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
    hB -> Add( hBtotal.get(), 1.);  hB -> Add( hBsubtract.get(), -1.);
    hC -> Add( hBtotal.get(), 1.);  hC -> Add( hCsubtract.get(), -1.);
    hD -> Add( hBtotal.get(), 1.);  hD -> Add( hDsubtract.get(), -1.);

    hR->Divide(hC.get(), hD.get(), 1., 1., "");

    hR->SaveAs("hR.C");

    hA -> Add( hB.get(), 0.8);
    
    /*

      for(const auto &category : categories){

      if( category.compare(target_category) ) //skip target category
      continue;

      if( ! category.compare(build_from_category) )
      hBtotal-> Add( hm[ forward_as_tuple(category,
      systematic,
      region_B,
      system,
      variable) ].get(),
      1.);
      else

      }
    */


    /*

      for (auto &entry : hm){



      cout<<"template:  "<<get<0>(entry.first)
      <<" : "<<get<1>(entry.first)
      <<" : "<<get<2>(entry.first)
      <<" : "<<get<3>(entry.first)
      <<" : "<<get<4>(entry.first)
      <<" : "<<entry.second->GetName()
      <<endl;

      if( get<0>(entry.first).find(category) != string::npos ){
      cout<<"\ttemplate category:  "<<get<0>(entry.first)
      <<" : "<<get<1>(entry.first)
      <<" : "<<get<2>(entry.first)
      <<" : "<<get<3>(entry.first)
      <<" : "<<get<4>(entry.first)
      <<" : "<<entry.second->GetName()
      <<endl;

      }

      if( get<0>(entry.first).find("data") != string::npos &&  get<2>(entry.first).find("SSIDRegion") != string::npos ){
      cout<<"\ttemplate adding:  "<<get<0>(entry.first)
      <<" : "<<get<1>(entry.first)
      <<" : "<<get<2>(entry.first)
      <<" : "<<get<3>(entry.first)
      <<" : "<<get<4>(entry.first)
      <<endl;

      hm[ forward_as_tuple(category,
      get<1>(entry.first),
      "SignalRegion",
      get<3>(entry.first),
      get<4>(entry.first)) ] ->Add( entry.second.get(), 1.);


      }

      }
    */

    hm[ forward_as_tuple(target_category,
                         systematic,
                         region_A,
                         system,
                         variable) ] ->Add( hA.get(), 1.);



  }

 private:




};

#endif
