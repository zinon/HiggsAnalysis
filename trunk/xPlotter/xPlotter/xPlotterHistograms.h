
#ifndef xPlotterHistograms_xPlotterHistograms_H
#define xPlotterHistograms_xPlotterHistograms_H

#include <xPlotter/xPlotterTuples.h>


class xPlotterHistograms  {

 public:

  xPlotterHistograms() {};

  ~xPlotterHistograms() = default;


 private:

  vector< HistoT > HistosV {

    /* 0. name,
       1. x-axis label, 
       2. N bins
       3. low x range
       4. high x range
       5. y-axis log scale,
    */
    HistoT( "leading_tau_pt", "p_{T}(leading #tau) [GeV]", 20, 0, 200, true)//,
      //HistoT( "leading_tau_eta", "#eta", 25, -2.5, 2.5, false)
      //HistoT( "tau_muon_m_vis", "M_{vis}(#tau, #mu) [GeV]", 40, 0, 200, false)

  };

  const auto Histo(const string &name) const -> HistoT {

    auto it = find_if(HistosV.begin(), HistosV.end(), 
		      [&name] (const HistoT& t) { 
			return !get<0>(t).compare(name) ; 
		      } 
		      );

    if( it != HistosV.end() ) return *it;
      
    throw ("xPlotterHistograms: Histo "+name+" not present in list of histograms").c_str();
  }

 public:

  const auto LogYaxis(const string &name) const -> bool {

    return get<5>( Histo(name) );

  }

  auto HistoNamesV() -> vector<string> {

    vector<string> v;

    for (const auto &h : HistosV)
      v.push_back( get<0>(h) );
    
    return v;
  }

  const auto  HistoDefs() const -> vector< HistoT >  {

    return HistosV;
  }

  auto ChildHistograms (const vector<string> &variables,
			const vector<string> &samples,
                        const vector<string> &systematics,
                        const vector<string> &regions,
                        const vector<string> &systems) -> map<HistoVarCasesT, shared_ptr<TH1D> >
    {

      map<HistoVarCasesT, shared_ptr<TH1D> > mhistos;
      for (const  auto &variable : variables)
	for (const auto &sample : samples )
	  for (const  auto &systematic : systematics)
	    for (const  auto &region : regions)
	      for (const  auto &system : systems)
                mhistos.emplace( forward_as_tuple( sample, systematic, region, system, variable ),
                                 source( "h_child", forward_as_tuple( sample, systematic, region, system ), variable ) );


      return mhistos;
    }

  auto ParentHistograms (const vector<string> &variables,
			 const vector<string> &categories,
			 const vector<string> &systematics,
			 const vector<string> &regions,
			 const vector<string> &systems) -> map<HistoVarCasesT, shared_ptr<TH1D> >
    {

      map<HistoVarCasesT, shared_ptr<TH1D> > mhistos;
      for (const  auto &variable : variables)
	for (const auto &category : categories )
	  for (const  auto &systematic : systematics)
	    for (const  auto &region : regions)
	      for (const  auto &system : systems)
                mhistos.emplace( forward_as_tuple( category, systematic, region, system, variable ),
                                 source( "h_parent", forward_as_tuple( category, systematic, region, system ), variable ) );


      return mhistos;
    }


  auto StackHistograms (const vector<string> &variables,
			const vector<string> &stacks,//before order
			const vector<string> &systematics,
			const vector<string> &regions,
			const vector<string> &systems) -> map<HistoVarCasesT, shared_ptr<TH1D> >
    {

      map<HistoVarCasesT, shared_ptr<TH1D> > mhistos;
      for (const  auto &variable : variables)
	for (const auto &stack : stacks )
	  for (const  auto &systematic : systematics)
	    for (const  auto &region : regions)
	      for (const  auto &system : systems)
                mhistos.emplace( forward_as_tuple( stack, systematic, region, system, variable ),
                                 source( "h_stack", forward_as_tuple( stack, systematic, region, system ), variable ) );


      return mhistos;
    }

  auto DataHistograms (const vector<string> &variables,
			const vector<string> &regions,
			const vector<string> &systems) -> map<DataCasesT, shared_ptr<TH1D> >
    {

      map<DataCasesT, shared_ptr<TH1D> > mhistos;
      for (const  auto &variable : variables)
	for (const  auto &region : regions)
	  for (const  auto &system : systems)
	    mhistos.emplace( forward_as_tuple( variable, system, region ),
			     source( "h_data", forward_as_tuple( region, system ), variable ) );


      return mhistos;
    }


  auto source(const string &prefix, const HistoGenCasesT &c, const string &variable) -> unique_ptr<TH1D>
  {

    auto h = Histo(variable);

    unique_ptr<TH1D> H = unique_ptr<TH1D>( new TH1D( (prefix + "_" + get<0>(c) + "_" +  get<1>(c) + "_" +  get<2>(c) + "_" + get<3>(c) + "_" + get<0>(h)).c_str(),//name
						     get<0>(h).c_str(), //title same as name
						     get<2>(h), //bins
						     get<3>(h), //low limit
						     get<4>(h)) ); //high limit

    H->SetXTitle( get<1>(h).c_str() );

    return H;
  }

  auto source(const string &prefix, const HistoGenReducedCasesT &c, const string &variable) -> unique_ptr<TH1D>
  {

    auto h = Histo(variable);

    unique_ptr<TH1D> H = unique_ptr<TH1D>( new TH1D( (prefix + "_" + get<0>(c) + "_" +  get<1>(c) + "_" + get<0>(h)).c_str(),//name
						     get<0>(h).c_str(), //title same as name
						     get<2>(h), //bins
						     get<3>(h), //low limit
						     get<4>(h)) ); //high limit

    H->SetXTitle( get<1>(h).c_str() );

    return H;
  }


};

#endif
