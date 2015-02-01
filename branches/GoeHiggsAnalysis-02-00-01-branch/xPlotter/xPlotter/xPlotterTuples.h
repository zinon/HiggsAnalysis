#ifndef xPlotterTuples_xPlotterTuples_H
#define xPlotterTuples_xPlotterTuples_H

using RegistryCasesT = tuple <
  string, //0. sample id, eg 104023, periodB
  string, //1. type, eg "signal", "background"
  string, //2. wildcard, eg *root*
  string  //3. data format, eg data, simulation, embedding
  >;

using ChainCasesT = tuple <
  string, //type (signal or background) or sample id or category
  string, //systematic variation
  string  //data format
  >;

using HistoGenCasesT = tuple <
  string, //type (signal or background) or sample id or category
  string, //systematic variation
  string, //region
  string  //system
  >;

using HistoGenReducedCasesT = tuple <
  string, //region
  string  //system
  >;

using HistoVarCasesT = tuple <
  string, //type (signal or background) or sample id or category
  string, //systematic variation
  string, //region
  string, //system
  string  //variable
  >;

using CutCasesT = tuple <
  string, //variable
  string, //system
  string //region
  >;

using DataCasesT = CutCasesT;

using CanvasCasesT = CutCasesT;

using WeightCaseT = tuple <
  string, //variable
  string //system
  >;

using BankT = tuple <
  string, //sample id
  string, //process
  int, //number of events
  double, //Sum of weighted events
  double, //cross section
  double, //kfactor
  double, //generator efficiency
  double //multiplication factor
  >;

using HistoT = tuple <
  string, //histoname
  string, //title
  int, //number of bins
  double, //low range
  double, //high range
  bool // y-axis log scale
  >;


#endif
