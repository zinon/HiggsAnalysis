
#ifndef xPlotterCanvas_xPlotterCanvas_H
#define xPlotterCanvas_xPlotterCanvas_H

#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TColor.h"
#include "TLegend.h"

#include <memory>

#include <xPlotter/xPlotterTuples.h>

#include <xPlotter/xPlotterTools.h>

//implementation for C++14
#include "CxxUtils/make_unique.h"

class xPlotterCanvas : virtual public xPlotterTools{

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

  //legend
  std::unique_ptr<TLegend> m_legend;

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
  double m_y_axis_offset_factor = 1.20;

  //options pad 2
  double m_y_axis_2_min_y = 0.5;
  double m_y_axis_2_max_y = 1.5;
  double m_x_axis_2_label_size = 0.14;
  std::string m_x_axis_2_title = "";
  double m_x_axis_title_offset = 0.9;
  double m_x_axis_2_title_size = 0.17;
  double m_y_axis_2_label_size = 0.14;
  std::string m_y_axis_2_title = "Data/Model";
  double m_y_axis_2_title_size = 0.17;
  double m_y_axis_2_title_offset = 0.34;
  bool m_show_horizontal_ratio_error_bars = false;
  bool m_show_ratio_lines = false;

  int m_y_axis_2_ndivisions = 205; 

  int m_data_marker_style = 20;
  double m_data_marker_size = 1.5;
  Color_t m_data_marker_color = kBlack;
  int m_data_line_style = 1;
  double m_data_line_width = 1.5;
  Color_t m_data_line_color = kBlack;

  int m_ratio_marker_style = 20;
  double m_ratio_marker_size = 1.5;
  Color_t m_ratio_marker_color = kBlack;
  int m_ratio_line_style = 1;
  double m_ratio_line_width = 1.5;
  Color_t m_ratio_line_color = kBlack;

  double m_legend_x1 = 0.7;
  double m_legend_y1 = 0.0;
  double m_legend_x2 = 0.9;
  double m_legend_y2 = 0.9;

  double m_legend_relative_font_size = 1.2;
  double m_legend_entry_vertical_size = 0.05;

 public:
  //canvas maker
  void Canvas(const CanvasCasesT &t,
              map< HistoVarCasesT, shared_ptr<TH1D> > &hm,
              const vector<string> &stack_order,
              const vector<Color_t> &stack_colors,
              const vector<string> &stack_names,
	      bool kLogYaxis,
              map< DataCasesT, shared_ptr<TH1D> > &hd,
              shared_ptr<TFile> f) {

    std::string xcase = get<2>(t) + "_" + get<1>(t) + "_" + get<0>(t);

    //pick up the top stack histogram
    std::shared_ptr<TH1D> hRef( hm[ forward_as_tuple( stack_order.back(), "NOMINAL", get<2>(t), get<1>(t), get<0>(t) ) ] ); 

    m_y_axis_1_log = kLogYaxis; //LogYaxis( "");// string(hRef->GetTitle()) );

    //back layer histos
    //double n = hRef->GetNbinsX();
    //double x0 = hRef->GetBinLowEdge(1);
    //double x1 = hRef->GetBinLowEdge(n) + hRef->GetBinWidth(1);
    
    int n; double x0; double x1; Xaxis(hRef, n, x0, x1, m_x_axis_2_title);
    
    std::shared_ptr<TH1D> hEmpty1 = EmptyCopy(hRef, "h_empty1_"+xcase); //std::make_shared<TH1D>( ("h_empty1_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hEmpty2 = EmptyCopy(hRef, "h_empty2_"+xcase); //std::make_shared<TH1D>( ("h_empty2_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hRatio = EmptyCopy(hRef, "h_ratio_"+xcase); //std::make_shared<TH1D>( ("h_ratio_"+xcase).c_str(), "", n, x0, x1);
    std::shared_ptr<TH1D> hData = EmptyCopy(hRef, "h_data_"+xcase); //std::make_shared<TH1D>( ("h_data_"+xcase).c_str(), "", n, x0, x1);

    //canvas formatting
    std::shared_ptr<TCanvas> c = std::make_shared<TCanvas>( ("c"+xcase).c_str(), "Stack Plots", 10, 10, m_canvas_x, m_canvas_y);
    SetCanvas(c);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    //upper pad =================================================================================
    c->cd();
    std::shared_ptr<TPad> pad1 =  std::make_shared<TPad>("pad1","pad1", 0, m_pad_midpoint, 1, 1);
    SetPad(pad1);
    pad1->SetTopMargin( c->GetTopMargin() * m_xfactor_top_margin );
    pad1->SetBottomMargin(0);
    pad1->Draw();
    pad1->cd();

    //x axis
    if(m_x_axis_set_range) hEmpty1->GetXaxis()->SetRangeUser(m_x_axis_min_x, m_x_axis_max_x);
    if(m_x_axis_ndivisions) hEmpty1->GetXaxis()->SetNdivisions( m_x_axis_ndivisions );
    hEmpty1->GetXaxis()->SetLabelSize(0.0);

    //y axis
    hEmpty1->GetYaxis()->SetLabelSize(0.04);
    hEmpty1->GetYaxis()->SetLabelFont(m_font);
    hEmpty1->GetYaxis()->SetTitle(m_y_axis_1_title.c_str());
    hEmpty1->GetYaxis()->SetTitleSize(m_y_axis_1_title_size);
    hEmpty1->GetYaxis()->SetTitleOffset(m_y_axis_1_title_offset);
    hEmpty1->GetYaxis()->SetTitleFont(m_font);

    //draw first layer
    hEmpty1->Draw();

    //legendary legend
    m_legend = Legend();

    //draw stack histograms
    int istack(stack_order.size()-1);
    for (auto itr = stack_order.rbegin(); itr != stack_order.rend(); ++itr, --istack){
    //    for ( auto &order : stack_order ){
      auto order = *itr;
      SetStack(hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ], stack_colors[istack]);

      hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->Draw("hist same");
      
      cout<<"drawing "<<istack<<" "<<order<<" : NOMINAL : "<< get<2>(t)<<":"<< get<1>(t) <<" : "<< get<0>(t)<<" logy "<<m_y_axis_1_log<<endl;
      cout<<"\t"<<hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->GetName()<<endl;
      cout<<"\t"<<hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->GetTitle()<<endl;
      cout<<"\t"<<hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ]->Integral()<<endl;

      AddLegend( hm[ forward_as_tuple(order, "NOMINAL", get<2>(t), get<1>(t), get<0>(t)) ], stack_names[istack], "lf" );
    }

    //draw data
    hData->Add( hd[ forward_as_tuple(get<0>(t), get<1>(t), get<2>(t)) ].get(), 1.);
    cout<<"drawing data "<<hData<<" : NOMINAL : "<< get<0>(t)<<" : "<< get<1>(t) <<" : "<< get<2>(t) <<endl;
    cout<<"\t"<<hData->GetName()<<endl;
    cout<<"\t"<<hData->GetEntries()<<endl;

    SetData( hData );

    hData->Draw("EP same");

    AddLegend( hData, "Data", "lp" );

    //y axis range
    double maxim_y = std::max( hRef -> GetBinContent( hRef -> GetMaximumBin() ), 
			       hData -> GetBinContent( hData -> GetMaximumBin() ) );
    double maxim_y_new = maxim_y * m_y_axis_offset_factor;
    if(m_auto_adjust_max_y_1_axis){
      hEmpty1->SetMaximum(maxim_y_new);
      cout<<"maxY "<<m_y_axis_1_max_y<<" -> "<<maxim_y_new<<endl;
    }else{
      if(m_y_axis_1_max_y > 0) hEmpty1->SetMaximum(m_y_axis_1_max_y);
      
    }
    if(m_y_axis_1_min_y > 0) hEmpty1->SetMinimum(m_y_axis_1_min_y);
    
    //log y axis
    if(m_y_axis_1_log) gPad->SetLogy();
    
    //adjust the size of the legend
    AdjustLegend();

    //display legend
    m_legend->Draw();
    cout<<"legend y1 final "<<m_legend->GetY1NDC()<<endl;

    //refresh axis
    gPad->RedrawAxis();

    //lower pad ==========================================================================================
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
    hEmpty2->GetYaxis()->SetTitle(m_y_axis_2_title.c_str());
    hEmpty2->GetYaxis()->SetTitleSize(m_y_axis_2_title_size);
    hEmpty2->GetYaxis()->SetTitleOffset(m_y_axis_2_title_offset);
    hEmpty2->GetYaxis()->SetTitleFont(m_font);
    hEmpty2->GetYaxis()->CenterTitle(1);
    if(m_x_axis_ndivisions) hEmpty2->GetXaxis()->SetNdivisions( m_x_axis_ndivisions );
    if(m_y_axis_2_ndivisions) hEmpty2->GetYaxis()->SetNdivisions( m_y_axis_2_ndivisions );
    
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

    */

    //compute ratio
    hRatio->Divide( hData.get(), hRef.get(), 1., 1., "");
    SetRatio( hRatio );
    
    hRatio->Draw("EP same");
    
    gPad->RedrawAxis();



    
    string a(c->GetName());
    c->SaveAs( (a+".png").c_str() );

    f->cd();
    c->Write();

    cout<<"xPlotterCanvas -to do: don't forget to reset and delete the shared_ptr's"<<endl;

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

  inline void SetData(std::shared_ptr<TH1D> h ){
    
    h->SetMarkerStyle(m_data_marker_style);
    h->SetMarkerSize(m_data_marker_size);
    h->SetMarkerColor(m_data_marker_color);

    h->SetLineStyle(m_data_line_style); 
    h->SetLineWidth(m_data_line_width); 
    h->SetLineColor(m_data_line_color); 

  }

  inline void SetStack(std::shared_ptr<TH1D> h, const Color_t &col ){
    
    h->SetLineWidth(1.5);
    h->SetFillColor(col);
    h->SetLineColor(col+2);

  }


  inline void SetRatio(std::shared_ptr<TH1D> h){

    h->SetMarkerStyle(m_ratio_marker_style);
    h->SetMarkerSize(m_ratio_marker_size);
    h->SetMarkerColor(m_ratio_marker_color);

    h->SetLineStyle(m_ratio_line_style); 
    h->SetLineWidth(m_ratio_line_width); 
    h->SetLineColor(m_ratio_line_color); 

  }

  auto Legend( ) -> std::unique_ptr<TLegend> {
 
    //    std::shared_ptr<TLegend> leg ( new TLegend(x0, y0, x1, y1) );

    std::unique_ptr<TLegend> legend (  new TLegend(m_legend_x1,
						   m_legend_y1, 
						   m_legend_x2, 
						   m_legend_y2,
						   NULL,
						   "brNDC")
				       );
					
    legend->SetBorderSize(0);
    legend->SetLineColor(0);
    legend->SetLineStyle(0);
    legend->SetLineWidth(0);
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->SetTextSize( legend->GetTextSize() * m_legend_relative_font_size );

    return legend;
  }

  inline void AddLegend(std::shared_ptr<TH1D> h, const string &description, const string &showup){

    m_legend->AddEntry( h.get(), description.c_str() , showup.c_str());

  }

  inline void AdjustLegend(){

    auto i = m_legend->GetNRows();

    auto y1 = m_legend_y2 - m_legend_entry_vertical_size * i;

    cout<<"LeGend y from "<<m_legend->GetY1NDC()<<" to "<<y1<<" for "<<i<<" rows "<<endl;
    
    m_legend->SetY1NDC(y1);

  }

};

#endif

