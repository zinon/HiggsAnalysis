
#ifndef xPlotterBank_xPlotterBank_H
#define xPlotterBank_xPlotterBank_H

#include <xPlotter/xPlotterTuples.h>
//#include <xPlotter/xPlotterBankData.h>

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

      /* 0. sample ID, 
	 1. process details, 
	 2. N events, 
	 3. Sum of weighted events, 
	 4. xsec, 
	 5. kFactor, 
	 6. generator or filter eff, 
	 7. multiplication factor 

      */

      //make_tuple("", "", , , /*nb*/, 1., 1., ?2fb),
      
      make_tuple("147808", "PowhegPythia8_AU2CT10_Ztautau", 19973800, 19973800, 1.1099/*nb*/, 1.1, 1., nb2fb),

	make_tuple("167740", "Sherpa_CT10_WenuMassiveCBPt0_BFilter", 14997980, 3234679.32, 10972/*pb*/, 1.1, 0.012791, pb2fb),
	make_tuple("167741", "Sherpa_CT10_WenuMassiveCBPt0_CJetFilterBVeto", 9998989, 2085568.31, 10972/*pb*/, 1.1, 0.049019, pb2fb),
	make_tuple("167742", "Sherpa_CT10_WenuMassiveCBPt0_CJetVetoBVeto", 49885967, 25997502.9, 10975/*pb*/, 1.1, 0.93804, pb2fb),

	make_tuple("167743", "Sherpa_CT10_WmunuMassiveCBPt0_BFilter", 14989485, 3232288.22, 10973/*pb*/, 1.1, 0.012794, pb2fb),
	make_tuple("167744", "Sherpa_CT10_WmunuMassiveCBPt0_CJetFilterBVeto", 9992484, 1862765.41, 10974/*pb*/, 1.1, 0.042507, pb2fb),
	make_tuple("167745", "Sherpa_CT10_WmunuMassiveCBPt0_CJetVetoBVeto", 49846965, 25952383.2, 10976/*pb*/, 1.1, 0.94458, pb2fb),

	make_tuple("167746", "Sherpa_CT10_WtaunuMassiveCBPt0_BFilter", 14925982, 3220212.12, 10972/*pb*/, 1.1, 0.012791, pb2fb),
	make_tuple("167747", "Sherpa_CT10_WtaunuMassiveCBPt0_CJetFilterBVeto", 9993984, 1987689.41, 10974/*pb*/, 1.1, 0.04615, pb2fb),
	make_tuple("167748", "Sherpa_CT10_WtaunuMassiveCBPt0_CJetVetoBVeto", 49920968, 26007588.1, 10976/*pb*/, 1.1, 0.94091, pb2fb),

	make_tuple("167749", "Sherpa_CT10_ZeeMassiveCBPt0_BFilter", 2726074, 820468.506, 1110/*pb*/, 1.12, 0.027969, pb2fb),
	make_tuple("167750", "Sherpa_CT10_ZeeMassiveCBPt0_CFilterBVeto", 2999995, 1274614.4, 1109.6/*pb*/, 1.12, 0.28322, pb2fb),
	make_tuple("167751", "Sherpa_CT10_ZeeMassiveCBPt0_CVetoBVeto", 4978999, 2607626.4, 1101/*pb*/, 1.12, 0.6886, pb2fb),

	make_tuple("167752", "Sherpa_CT10_ZmumuMassiveCBPt0_BFilter", 3997997, 819826.006, 1110/*pb*/, 1.12, 0.02796, pb2fb),
	make_tuple("167753", "Sherpa_CT10_ZmumuMassiveCBPt0_CFilterBVeto", 2937995, 1249066.5, 1110.3/*pb*/, 1.12, 0.28353, pb2fb),
	make_tuple("167754", "Sherpa_CT10_ZmumuMassiveCBPt0_CVetoBVeto", 4993999, 2614120.4, 1110/*pb*/, 1.12, 0.68872, pb2fb),

	make_tuple("110101", "AcerMCPythia_P2011CCTEQ6L1_singletop_tchan_l", 59962956, 51345222, 87.76/*pb*/, 1., 0.324, pb2fb),
	make_tuple("110119", "PowhegPythia_P2011C_st_schan_lep", 5995993, 5995993, 5.61/*pb*/, 1., 0.324, pb2fb),
	make_tuple("110140", "PowhegPythia_P2011C_st_Wtchan_incl_DR", 19937980, 19937980, 22.37/*pb*/, 1., 1., pb2fb),

	make_tuple("117050", "PowhegPythia_P2011C_ttbar", 99930891, 99930891, 252.89/*pb*/, 1., 0.54298, pb2fb)
      
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
      return  static_cast<double>( get<3>( *it ) ) / ( get<4>( *it ) * get<5>( *it ) * get<6>( *it ) * get<7>( *it ) );
    else
      throw std::invalid_argument( ("information not found in bank for received sample "+number).c_str() );

    return 0;

  }

  double InvLumi(const string & number){auto L = Lumi(number); return L > 0 ? 1./Lumi(number) : 0; }

  auto Weight(const string& number) -> TCut
  {

    return TCut( to_string( InvLumi(number) ).c_str() );

  }
  
};

#endif
