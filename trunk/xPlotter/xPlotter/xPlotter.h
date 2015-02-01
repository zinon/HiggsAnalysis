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
#include <xPlotter/xPlotterTemplateMethod.h>

class xPlotter
: public xPlotterHistograms,
  public xPlotterCuts,
  public xPlotterCanvas,
  public xPlotterTemplateMethod
  //virtual public xPlotterBank ,
{

 public:
  xPlotter();
  ~xPlotter() = default; //auto generated

  typedef enum DataFormat_t {
    DATA,
    SIMULATION,
    ESTIMATION,
    EMBEDDING
    
  } DataFormat_t;

 private:

  string m_childplots_output_file_name = "child_plots.root";

  string m_parentplots_output_file_name = "parent_plots.root";

  string m_stackplots_output_file_name = "stack_plots.root";

  string m_canvases_output_file_name = "canvases.root";

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

  vector<string> m_stack_names;

  bool m_apply_template_method = false;

  string m_template_method_target_category = "";
  
  string m_template_method_source_category = "";
 
  vector<string> m_template_method_subtraction_categories;

  vector<string> m_template_method_regions;

  vector<string> m_template_method_systematics;

  vector<string> m_template_method_systems;

  vector<string> m_template_method_variables;


  double m_luminosity = 0;

  template<typename T> void CheckPtr(T s) { //Exception thrown on typeid of null pointer
    try
      {
        //cerr << typeid(*s).name()<<endl; // throws a bad_typeid exception
        //(0 != s);
        //throw std::runtime_error("invalid/zero input");

      }
    catch (std::bad_typeid& bt)
      {
        std::cerr << "bad_typeid caught: " << bt.what() << '\n';
      }
  }


  bool CheckChain( const RegistryCasesT &, shared_ptr<TChain>);

  void PrintChild( const HistoVarCasesT &) const;

  void Create(); 

  void Iterate();

  void Plot();  

  void Store(); 

  bool TemplatedSampleCategory(const string &s){

    return s.find( m_template_method_target_category ) != string::npos;

  }

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

  void Register(const string&, const string&);

  void Register(const string&, const string&, const string&, const string&);

  void Register(const string&, const string&, const string&, const int);

  void Register(const string&, const string&, const string&, const string&, const DataFormat_t&);

  void Register(const string&, const string&, const string&, const int, const DataFormat_t&);

  void TemplateMethod(const string&, const string&, initializer_list<string>);

  void StackOrder(initializer_list<string>); 

  void StackNames(initializer_list<string>); 

  void StackColors(initializer_list<Color_t>);

  void Regions(initializer_list<string>);  

  void Variables(initializer_list<string>); 

  void Luminosity(double x){ m_luminosity = x; }

  void Draw(); // the upstream call function

};

#endif
