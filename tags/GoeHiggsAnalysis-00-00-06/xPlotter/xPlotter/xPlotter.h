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
#include "TColor.h"

#include <xPlotter/xPlotterTuples.h>

#include <xPlotter/xPlotterHistograms.h>
#include <xPlotter/xPlotterCanvas.h>
#include <xPlotter/xPlotterCuts.h>

class xPlotter
: public xPlotterHistograms,
  public xPlotterCuts,
  public xPlotterCanvas
  //virtual public xPlotterBank ,
{

 public:
  xPlotter();
  ~xPlotter() = default; //auto generated

  typedef enum DataFormat_t {
    DATA,
    SIMULATION,
    EMBEDDING
    
  } DataFormat_t;

 private:

  string m_childplots_output_file_name = "child_plots.root";

  string m_parentplots_output_file_name = "parent_plots.root";

  string m_stackplots_output_file_name = "stack_plots.root";

  string m_canvases_output_file_name = "canvases.root";


  //  unique_ptr<impl> pimpl;

  //<unique_ptr<node>> children;

  //  map<int, shared_ptr<TFile> > m_files;

  //  vector<ChainT> m_ChainV;

  //unique_ptr<Foo> p1(new Foo);

  map< ChainCasesT, shared_ptr<TChain> > m_ChainsM;

  map< HistoVarCasesT, shared_ptr<TH1D> > m_ChildHistosM;

  map< HistoVarCasesT, shared_ptr<TH1D> > m_ParentHistosM;

  map< HistoVarCasesT, shared_ptr<TH1D> > m_StackHistosM;

  map< DataCasesT, shared_ptr<TH1D> > m_DataHistosM;

  map< CanvasCasesT, shared_ptr<TCanvas> > m_CanvasesM;

  vector< RegistryCasesT > m_registry;

  vector<string> m_samples;

  vector<string> m_regions;

  vector<string> m_systems;

  vector<string> m_systematics;

  vector<string> m_variables;

  vector<string> m_categories;

  map<string, string > m_sample_category; 

  vector<string> m_stack_order;

  vector<Color_t> m_stack_colors;

  double m_luminosity = 0;

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

  void Iterate();

  void Plot();  

  void Store(); 

  auto DataFormat(const DataFormat_t &df) ->string {

    switch ( df )
      {
      case DATA:
	return "data";
      case SIMULATION:
	return "simulation";
      case EMBEDDING:
	return "embedding";
      default:
	throw std::invalid_argument( ("unknown data format "+to_string(df)).c_str() );
      }

    return "";
  }

 public:
  
  void ChildrenPlotsFile(const string &s){ m_childplots_output_file_name = s; }

  void ParentPlotsFile(const string &s){ m_parentplots_output_file_name = s; }

  void StackPlotsFile(const string &s){ m_stackplots_output_file_name = s; };

  void CanvasesFile(const string &s){ m_canvases_output_file_name = s; }

  void Register(const string&, const string&, const string&, const string&);

  void Register(const string&, const string&, const string&, const int);

  void Register(const string&, const string&, const string&, const string&, const DataFormat_t&);

  void Register(const string&, const string&, const string&, const int, const DataFormat_t&);

  void StackOrder(initializer_list<string>); 

  void StackColors(initializer_list<Color_t>); 

  void Variables(initializer_list<string>); 

  void Luminosity(double x){ m_luminosity = x; }

  void Draw(); // upstream call function

};

#endif
