#ifndef xPlotterTuples_xPlotterTuples_H
#define xPlotterTuples_xPlotterTuples_H

using RegistryCasesT = tuple <
  string, //0. sample id
  string, //1. type
  string  //2. wildcard
  >;

using ChainCasesT = tuple <
  string, //type (signal or background) or sample id or category
  string //systematic variation
  >;

using HistoGenCasesT = tuple <
  string, //type (signal or background) or sample id or category
  string, //systematic variation
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

using HistoT = tuple <
  string, //histoname
  string, //title
  int, //number of bins
  double, //low range
  double //high range
  >;

using CutCaseT = tuple <
  string, //variable
  string, //system
  string //region
  >;

using WeightCaseT = tuple <
  string, //variable
  string //system
  >;

using BankT = tuple <
  string, //sample id
  string, //process
  int, //number of events
  double, //cross section
  double, //kfactor
  double, //generator efficiency
  double //multiplication factor
  >;


#endif
