
#ifndef xPlotterTools_xPlotterTools_H
#define xPlotterTools_xPlotterTools_H

#include <xPlotter/xPlotterTuples.h>


class xPlotterTools  {

 public:

  xPlotterTools() {};

  ~xPlotterTools() = default;

  auto EmptyCopy (const std::shared_ptr<TH1D> h, const string &name) -> decltype(h) {

    double n = h->GetNbinsX();
    double x0 = h->GetBinLowEdge(1);
    double x1 = h->GetBinLowEdge(n) + h->GetBinWidth(1);
    
    std::shared_ptr<TH1D> hnew = std::make_shared<TH1D>( name.c_str(), "", n, x0, x1);

    hnew->Sumw2();

    return hnew;
  
  }

  void Xaxis(const std::shared_ptr<TH1D> h, int &n, double &x0, double &x1, string &title) {

    n = h->GetNbinsX();
    x0 = h->GetBinLowEdge(1);
    x1 = h->GetBinLowEdge(n) + h->GetBinWidth(1);
    title = static_cast<std::string>(h->GetXaxis()->GetTitle());

  }

 private:


};

#endif
