#ifndef xPlotter_xPlotter_H
#define xPlotter_xPlotter_H

using namespace std;

#include <iostream>
#include <map>
#include <cstdlib>
#include <memory> //smart pointers
#include <typeinfo> // operator typeid, std::bad_typeid
#include <exception>
#include <tuple>  // std::tuple, std::get, std::tie, std::ignore
#include <utility>
#include <algorithm>
#include <initializer_list>
#include <vector>

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TCut.h"
#include "TH1D.h"

#include <xPlotter/xPlotterTuples.h>

#include <xPlotter/xPlotterHistograms.h>
//#include <xPlotter/xPlotterBank.h>
#include <xPlotter/xPlotterCuts.h>


/*using ChainT = tuple <
  int, //0. sample number/identifier
  string, //1. sample type: signal or bkg
  double, //2. sample weight
  string, //3. systematic name
  shared_ptr< TChain >, //4. file chain
  vector< shared_ptr<TH1D> > //5. histograms
  >;
*/

class xPlotter
: public xPlotterHistograms,
  public xPlotterCuts
//virtual public xPlotterBank ,
{

 public:
  xPlotter();
  ~xPlotter() = default; //auto generated

 private:

  string m_generic_output_file_name = "general_plots.root";

  string m_specific_output_file_name = "specific_plots.root";


  //  unique_ptr<impl> pimpl;

  //<unique_ptr<node>> children;

  //  map<int, shared_ptr<TFile> > m_files;

  //  vector<ChainT> m_ChainV;

  //unique_ptr<Foo> p1(new Foo);

  map< ChainCasesT, shared_ptr<TChain> > m_ChainsM;

  map< HistoVarCasesT, shared_ptr<TH1D> > m_ChildHistosM;

  map< HistoVarCasesT, shared_ptr<TH1D> > m_ParentHistosM;

  map< HistoVarCasesT, shared_ptr<TH1D> > m_StackHistosM;

  vector< RegistryCasesT > m_registry;

  vector<string> m_samples;

  vector<string> m_regions;

  vector<string> m_systems;

  vector<string> m_systematics;

  vector<string> m_categories;

  map<string, string > m_sample_category; 

  vector<string> m_stack_order;

  template<typename T> void CheckPtr(T s) { //Exception thrown on typeid of null pointer
    try
      {
        cerr << typeid(*s).name()<<endl; // throws a bad_typeid exception
        //(0 != s);
        //throw std::runtime_error("invalid/zero input");

      }
    catch (std::bad_typeid& bt)
      {
        std::cerr << "bad_typeid caught: " << bt.what() << '\n';
      }
  }

  /*
    try {
    (0 != s);
    cerr<<"checking input "<<n<<endl;
    throw std::runtime_error("invalid/zero input");
    } catch (const std::invalid_argument& e) {
    std::cerr << "exception caught: " << e.what() << " for input "<<n<<"\n";
    }
  */
  //}



  bool CheckChain( const RegistryCasesT &, shared_ptr<TChain>);

  void PrintChild( const HistoVarCasesT &) const;

  void Create(); 

  void Iterate(); // to be private

  void Store(); // to be private

 public:

  void GenericFile(const string &s){ m_generic_output_file_name = s; }

  void SpecificFile(const string &s){ m_specific_output_file_name = s; }

  void Register(const string&, const string&, const string&, const int);

  void StackOrder(initializer_list<string>); 

  void Draw(); //contains all above

};

#endif
