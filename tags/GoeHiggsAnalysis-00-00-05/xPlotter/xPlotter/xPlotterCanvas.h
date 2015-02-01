
#ifndef xPlotterCanvas_xPlotterCanvas_H
#define xPlotterCanvas_xPlotterCanvas_H

#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"

#include <memory>

#include <xPlotter/xPlotterTuples.h>

class xPlotterCanvas {

 public:

  xPlotterCanvas() {};

  virtual ~xPlotterCanvas() = default;

  /*  inline Canvas(const CanvasCaseT &t){

  //map<HistoT, shared_ptr<TCanvas> > m;

  for( auto &e : v)
  m.emplace_back( get<0>()),


  }
  */

 private:

  //options canvas
  int m_canvas_x = 700;
  int m_canvas_y = 800;
  double m_pad_midpoint = 0.2;
  int m_font = 82;
  double m_xfactor_top_margin = 0.4;
  double m_xfactor_bottom_margin = 3.5;
  double m_xfactor_right_margin = 0.4;
  double m_xfactor_left_margin = 1.2;

  //common options
  double m_x_axis_min_x = 0;
  double m_x_axis_max_x = 1;
  bool m_x_axis_set_range = false;
  int m_x_axis_ndivisions = 0; //505 for tracks

  //options pad 1
  double m_y_axis_1_title_size = 0.05;
  double m_y_axis_1_title_offset = 1.3;
  bool m_y_axis_1_log = false;
  std::string m_y_axis_1_title = "events";
  bool m_auto_adjust_max_y_1_axis = true;
  double m_y_axis_1_min_y = 0.0;
  double m_y_axis_1_max_y = 1.0;

  //options pad 2
  double m_y_axis_2_min_y = 0.5;
  double m_y_axis_2_max_y = 1.5;
  double m_x_axis_2_label_size = 0.14;
  std::string m_x_axis_2_title = "";
  double m_x_axis_title_offset = 0.9;
  double m_x_axis_2_title_size = 0.17;
  double m_y_axis_2_label_size = 0.14;
  std::string m_pad_2_title = "";
  double m_y_axis_2_title_size = 0.17;
  double m_y_axis_2_title_offset = 0.34;
  bool m_show_horizontal_ratio_error_bars = false;
  bool m_show_ratio_lines = false;

 public:
  //canvas maker
  void Canvas(const CanvasCasesT &t,
              map< HistoVarCasesT, shared_ptr<TH1D> > &hm,
              const vector<string> &stack_order,
              shared_ptr<TFile> f) {

    std::string xcase = get<2>(t) + "_" + get<1>(t) + "_" + get<0>(t);

    shared_ptr<TH1D> href( hm[ forward_as_tuple(stack_order.back(), "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ] );

    //back layer histos
    double n = href->GetNbinsX();
    double x0 = href->GetBinLowEdge(1);
    double x1 = href->GetBinLowEdge(n) + href->GetBinWidth(1);

    std::shared_ptr<TH1D> hEmpty1 = std::make_shared<TH1D>( ("h_empty1_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hEmpty2 = std::make_shared<TH1D>( ("h_empty2_"+xcase).c_str(), "", n, x0, x1);

    //canvas formatting
    std::shared_ptr<TCanvas> c = std::make_shared<TCanvas>( ("c"+xcase).c_str(), "Stack Plots", 10, 10, m_canvas_x, m_canvas_y);
    SetCanvas(c);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    //upper pad
    c->cd();
    std::shared_ptr<TPad> pad1 =  std::make_shared<TPad>("pad1","pad1", 0, m_pad_midpoint, 1, 1);
    SetPad(pad1);
    pad1->SetTopMargin( c->GetTopMargin() * m_xfactor_top_margin );
    pad1->SetBottomMargin(0);
    pad1->Draw();
    pad1->cd();

    //x
    if(m_x_axis_set_range) hEmpty1->GetXaxis()->SetRangeUser(m_x_axis_min_x, m_x_axis_max_x);
    if(m_x_axis_ndivisions) hEmpty1->GetXaxis()->SetNdivisions( m_x_axis_ndivisions );
    hEmpty1->GetXaxis()->SetLabelSize(0.0);

    //y
    hEmpty1->GetYaxis()->SetLabelSize(0.04);
    hEmpty1->GetYaxis()->SetLabelFont(m_font);
    hEmpty1->GetYaxis()->SetTitle(m_y_axis_1_title.c_str());
    hEmpty1->GetYaxis()->SetTitleSize(m_y_axis_1_title_size);
    hEmpty1->GetYaxis()->SetTitleOffset(m_y_axis_1_title_offset);
    hEmpty1->GetYaxis()->SetTitleFont(m_font);

    //draw
    hEmpty1->Draw();
    for ( auto &order : stack_order ){
      hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->Draw("same");
      cout<<hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->GetName()<<endl;
      cout<<hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->GetIntegral()<<endl;
    }

    //max y
    double maxim_y = href -> GetBinContent( href -> GetMaximumBin() );
    double maxim_y_new = maxim_y * 1.10;
    if(m_auto_adjust_max_y_1_axis){
      hEmpty1->SetMaximum(maxim_y_new);
      cout<<"maxY "<<m_y_axis_1_max_y<<" -> "<<maxim_y_new<<endl;
    }else{
      if(m_y_axis_1_max_y > 0) hEmpty1->SetMaximum(m_y_axis_1_max_y);
      
    }
    if(m_y_axis_1_min_y > 0) hEmpty1->SetMinimum(m_y_axis_1_min_y);
    


      // cout<<hm[ forward_as_tuple(get<2>(t),get<2>(t),get<2>(t),get<2>(t),get<2>(t)) ];//->Draw("same");

    if(m_y_axis_1_log) gPad->SetLogy();
    gPad->RedrawAxis();

    //lower pad
    c->cd();
    std::shared_ptr<TPad> pad2 =  std::make_shared<TPad>("pad2","pad2", 0, 0, 1, m_pad_midpoint);
    SetPad(pad2);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(c->GetBottomMargin() * m_xfactor_bottom_margin );
    pad2->SetFillStyle(4000); //transparent
    pad2->Draw();
    pad2->cd();

    //x
    if(m_x_axis_set_range) hEmpty2->GetXaxis()->SetRangeUser(m_x_axis_min_x, m_x_axis_max_x);
    hEmpty2->GetXaxis()->SetLabelSize(m_x_axis_2_label_size);
    hEmpty2->GetXaxis()->SetLabelFont(m_font);
    hEmpty2->GetXaxis()->SetTitle(m_x_axis_2_title.c_str());
    hEmpty2->GetXaxis()->SetTitleOffset(m_x_axis_title_offset);
    hEmpty2->GetXaxis()->SetTitleFont(m_font);
    hEmpty2->GetXaxis()->SetTitleSize(m_x_axis_2_title_size);

    //y
    hEmpty2->SetMinimum(m_y_axis_2_min_y);
    hEmpty2->SetMaximum(m_y_axis_2_max_y);
    hEmpty2->GetYaxis()->SetLabelSize(m_y_axis_2_label_size);
    hEmpty2->GetYaxis()->SetLabelFont(m_font);
    hEmpty2->GetYaxis()->SetTitle(m_pad_2_title.c_str());
    hEmpty2->GetYaxis()->SetTitleSize(m_y_axis_2_title_size);
    hEmpty2->GetYaxis()->SetTitleOffset(m_y_axis_2_title_offset);
    hEmpty2->GetYaxis()->SetTitleFont(m_font);
    hEmpty2->GetYaxis()->CenterTitle(1);
    if(m_x_axis_ndivisions) hEmpty2->GetXaxis()->SetNdivisions( m_x_axis_ndivisions );

    hEmpty2->Draw();
    /*
      if(m_show_HorizontalErrorBarsInRatioPlot){

      int ci5 = kCyan - 5;
      int ci10 = kCyan - 8;
      int ci20 = kCyan - 10;

      MinX -=  kAdjustLeftBin         ? BinW : 0;
      MaxX +=  kAdjustRightBin        ? BinW : 0;
      TBox *box5 = new TBox(m_minX, 0.95, m_maxX, 1.05);
      box5->SetFillColor(ci5);

      TBox *box10 = new TBox(MinX, 0.90, MaxX, 1.1);
      box10->SetFillColor(ci10);

      TBox *box20 = new TBox(MinX, 0.80, MaxX, 1.2);
      box20->SetFillColor(ci20);

      box20->Draw("same");
      box10->Draw("same");
      box5->Draw();
      }

      if( m_show_ratio_lines ){

      TLine *line1 = new TLine( hEmpty2->GetXaxis()->GetXmin(), 1, hEmpty2->GetXaxis()->GetXmax(), 1);
      line1->SetLineStyle(7);
      line1->SetLineColor(1);
      line1->Draw("same");
      }

      hc.SetStyle(h_Data_over_BKG, "ratio");

      gPad->RedrawAxis();
    */
    
    string a(c->GetName());
    c->SaveAs( (a+".png").c_str() );

    f->cd();
    c->Write();

  }

  inline void SetCanvas(std::shared_ptr<TCanvas> c ){
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(0);
    c->SetFrameBorderMode(0);
  }

  inline void SetPad(std::shared_ptr<TPad> p ){
    p->SetTicky(1);
    p->SetTickx(1);
    p->SetGridx();
    p->SetGridy();
    gStyle->SetGridColor(kGray);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    p->SetRightMargin( p->GetRightMargin() * m_xfactor_right_margin );
    p->SetLeftMargin( p->GetLeftMargin() * m_xfactor_left_margin ); 

  }


};

#endif

