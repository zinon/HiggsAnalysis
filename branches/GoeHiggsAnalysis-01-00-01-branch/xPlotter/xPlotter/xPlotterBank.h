
#ifndef xPlotterBank_xPlotterBank_H
#define xPlotterBank_xPlotterBank_H

#include <xPlotter/xPlotterTuples.h>

//barn to femto
#define b2fb 1e15
//milli to femto
#define mb2fb 1e12
//micro to femto
#define ub2fb 1e9
//nano to femto
#define nb2fb 1e6
//pico to fempto
#define pb2fb 1e3


class xPlotterBank {

 public:

  xPlotterBank() {};

  ~xPlotterBank() = default;



 private:

  vector< BankT > BankV // nested list-initialization
    {

      /*sample ID, process details, N events, xsec, kFactor, generator or filter eff, multiplication factor*/
      
      make_tuple("147808", "PowhegPythia8_AU2CT10_Ztautau", 19973800, 1.1099/*nb*/, 1.1, 1., nb2fb),

      make_tuple("167743", "Sherpa_CT10_WmunuMassiveCBPt0_BFilter", 14989485, 10973/*pb*/, 1.1, 0.012794, pb2fb),
      make_tuple("167744", "Sherpa_CT10_WmunuMassiveCBPt0_CJetFilterBVeto", 9992484, 10974/*pb*/, 1.1, 0.042507, pb2fb),
      make_tuple("167745", "Sherpa_CT10_WmunuMassiveCBPt0_CJetVetoBVeto", 49846965, 10976/*pb*/, 1.1, 0.94458, pb2fb),

      make_tuple("167746", "Sherpa_CT10_WtaunuMassiveCBPt0_BFilter", 14925982, 10973/*pb*/, 1.1, 0.012791, pb2fb),
      make_tuple("167747", "Sherpa_CT10_WtaunuMassiveCBPt0_CJetFilterBVeto", 9993984, 10974/*pb*/, 1.1, 0.04615, pb2fb),
      make_tuple("167748", "Sherpa_CT10_WtaunuMassiveCBPt0_CJetVetoBVeto", 49920968, 10976/*pb*/, 1.1, 0.94091, pb2fb),

      
      };

 public:

  double Lumi(const string& number)
  {

    auto it = find_if( BankV.begin(),
                       BankV.end(),
                       //C++11 lambda function:
                       [number](const BankT &tup) -> bool
                       { return !(number.compare(get<0>(tup))); }
                       );

    if(it != BankV.end())
      return  static_cast<double>( get<2>( *it ) ) / ( get<3>( *it ) * get<4>( *it ) * get<5>( *it ) * get<6>( *it ) );
    else
      throw invalid_argument( "information not found in bank for received sample" );
    //cout<<"Coundn't find information for dataset "<<number<<", returning a zero value ..."<<endl;


    return 0;

  }

  double InvLumi(const string & number){auto L = Lumi(number); return L > 0 ? 1./Lumi(number) : 0; }

  auto Weight(const string& number) -> TCut
  {

    return TCut( to_string( InvLumi(number) ).c_str() );

  }
  
};

#endif
