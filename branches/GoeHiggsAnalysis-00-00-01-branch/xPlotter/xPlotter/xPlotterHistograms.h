
#ifndef xPlotterHistograms_xPlotterHistograms_H
#define xPlotterHistograms_xPlotterHistograms_H

#include <xPlotter/xPlotterTuples.h>


class xPlotterHistograms  {

 public:

  xPlotterHistograms() {};

  ~xPlotterHistograms() = default;


 private:

  vector< HistoT > HistosV {

    HistoT( "leading_tau_pt", "", 20, 0, 200),
      HistoT( "leading_tau_eta", "", 25, -2.5, 2.5)

      };


 public:

  /*
    auto ChildHistograms (int number, const string &systematic, const string &region) -> vector< shared_ptr<TH1D> >
    {

    decltype(systematic)  name("h_"+ to_string(number) + "_" + systematic + "_" + region + "_" );

    vector< shared_ptr<TH1D> > vhistos;

    for( auto &histo : HistosV)
    vhistos.push_back( source( name, histo ) );


    return vhistos;
    }
  */

  auto ChildHistograms (const vector<string> &samples,
                        const vector<string> &systematics,
                        const vector<string> &regions,
                        const vector<string> &systems) -> map<HistoVarCasesT, shared_ptr<TH1D> >
    {

      map<HistoVarCasesT, shared_ptr<TH1D> > mhistos;
      for (const auto &sample : samples )
        for (const  auto &systematic : systematics)
          for (const  auto &region : regions)
            for (const  auto &system : systems)
              for (const  auto &histo : HistosV)
                mhistos.emplace( forward_as_tuple( sample, systematic, region, system, get<0>(histo) ),
                                 source( forward_as_tuple( sample, systematic, region, system ), histo ) );


      return mhistos;
    }

  auto ParentHistograms (const vector<string> &categories,
			 const vector<string> &systematics,
			 const vector<string> &regions,
			 const vector<string> &systems) -> map<HistoVarCasesT, shared_ptr<TH1D> >
    {

      map<HistoVarCasesT, shared_ptr<TH1D> > mhistos;
      for (const auto &category : categories )
        for (const  auto &systematic : systematics)
          for (const  auto &region : regions)
            for (const  auto &system : systems)
              for (const  auto &histo : HistosV)
                mhistos.emplace( forward_as_tuple( category, systematic, region, system, get<0>(histo) ),
                                 source( forward_as_tuple( category, systematic, region, system ), histo ) );


      return mhistos;
    }


  auto StackHistograms (const vector<string> &order,
			 const vector<string> &systematics,
			 const vector<string> &regions,
			 const vector<string> &systems) -> map<HistoVarCasesT, shared_ptr<TH1D> >
    {


      vector<string> stacks(order); 
      /*      for(auto i(0); i < order.size(); i++ )
	for(auto j(i+1); j < order.size(); j++ )
	  cout<<i <<" "<<j<<endl;
      */
      //     for( auto & el : stacks)
      //	el = 
      for(unsigned int i = 1; i < stacks.size(); i++ )
	stacks[i] = stacks[i-1] + "_" + stacks[i];
   
   
      for( std::string ele : stacks)
      	std::cout<<"stack: "<< ele <<std::endl;

      map<HistoVarCasesT, shared_ptr<TH1D> > mhistos;
      for (const auto &stack : stacks )
        for (const  auto &systematic : systematics)
          for (const  auto &region : regions)
            for (const  auto &system : systems)
              for (const  auto &histo : HistosV)
                mhistos.emplace( forward_as_tuple( stack, systematic, region, system, get<0>(histo) ),
                                 source( forward_as_tuple( stack, systematic, region, system ), histo ) );


      return mhistos;
    }


  auto source(const HistoGenCasesT &c, const HistoT &h) -> unique_ptr<TH1D>
  {

    //    decltype(systematic)  name("h_" + get<0>(c) + "_" +  get<1>(c) + "_" +  get<1>(c) + "_" + get<0>(h) );
    
    return unique_ptr<TH1D>( new TH1D( ("h_" + get<0>(c) + "_" +  get<1>(c) + "_" +  get<2>(c) + "_" + get<3>(c) + "_" + get<0>(h)).c_str(),
                                       get<0>(h).c_str(),
                                       get<2>(h),
                                       get<3>(h),
                                       get<4>(h)) );
  }

};

#endif
