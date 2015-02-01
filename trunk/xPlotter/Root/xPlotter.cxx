#include <xPlotter/xPlotter.h>

xPlotter::xPlotter(){

  m_ChainsM.clear();
  m_ChildHistosM.clear();
  m_ParentHistosM.clear();
  m_DataHistosM.clear();
  m_CanvasesM.clear();
  m_sample_category.clear();
  m_variables.clear();

  //hardcoded
  m_systematics = {"NOMINAL"}; //nominal should be always at the top

  m_systems = {"tau_muon"};

}

void xPlotter::Register(const string& type, const string &category, const string& wildcard, const int number){

  Register(type, category, wildcard, number, SIMULATION);

}

void xPlotter::Register(const string& type, const string &category, const string& wildcard, const int number, const DataFormat_t &df){

  Register(type, category, wildcard, to_string(number), df);

}

void xPlotter::Register(const string& type, const string &category, const string& wildcard, const string& number){

  Register(type, category, wildcard, number, SIMULATION);

}

void xPlotter::Register(const string& type, const string &category){

  Register(type, category, "", "", ESTIMATION);

}

void xPlotter::Register(const string& type, const string &category, const string& wildcard, const string& number, const DataFormat_t &df){

  if(number.size() && type.size() && wildcard.size())
    m_registry.emplace_back( number, type, wildcard, DataFormat(df)  );

  if (find(m_categories.begin(), m_categories.end(), category) == m_categories.end())
    m_categories.push_back(category);

  if (find(m_samples.begin(), m_samples.end(), number ) == m_samples.end())
    m_samples.push_back( number );

  if( m_sample_category.find( number ) == m_sample_category.end() )
    m_sample_category.emplace( number, category );

}

void xPlotter::TemplateMethod(const string& target_sample_category,
                              const string& source_sample_category,
                              initializer_list<string> subtraction_categories_list
                              ){

  m_apply_template_method = true;

  try{

    if (find(m_categories.begin(), m_categories.end(), target_sample_category) == m_categories.end())
      throw "A non-existing/mismatched 'target' category set to be calculated with the template method\n";
    else
      m_template_method_target_category = target_sample_category;

    if (find(m_categories.begin(), m_categories.end(), source_sample_category) == m_categories.end())
      throw "A non-existing/mismatched 'source' category set to be calculated with the template method\n";
    else
      m_template_method_source_category = source_sample_category;

    for(const auto &sub : subtraction_categories_list)
      if (find(m_categories.begin(), m_categories.end(), sub) == m_categories.end())
        throw "A non-existing/mismatched 'subtraction' category set to be calculated with the template method\n";

    m_template_method_subtraction_categories = subtraction_categories_list;

  } catch (exception& e) {
    cout << "TemplateMethod "  << e.what() << '\n';
  }


}

void xPlotter::Regions(initializer_list<string> list){

  m_regions = list;
}

void xPlotter::Variables(initializer_list<string> list){

  m_variables = list;
}

void xPlotter::StackColors(initializer_list<Color_t> list){

  m_stack_colors = list;
}


void xPlotter::StackOrder(initializer_list<string> list){

  m_stack_order = list;

  //todo : check if the input strings agree to the m_categories list - if there're differences then throw exception
  //todo: set a default in the Register

  /*
    try
    {


    }
    catch (std::exception& e)
    {
    std::cerr << "BackgroundStackOrder: exception caught: " << e.what() << '\n';
    }
  */

}

void xPlotter::StackNames(initializer_list<string> list){

  m_stack_names = list;
}

void xPlotter::Create(){

  cout<<"Create"<<endl;
  //create chains
  for ( const auto &registry : m_registry ){
    for ( const auto &systematic : m_systematics ){

      shared_ptr< TChain > fchain( new TChain( systematic.c_str()) );

      fchain->Add( get<2>(registry).c_str());

      if( CheckChain( registry, fchain) )
        m_ChainsM.emplace( forward_as_tuple( get<0>(registry), systematic, get<3>(registry)) , fchain );
      else
        throw std::invalid_argument("null or empty chain");
    }
  }

  //here we decise which regions to include: a chosen subset or all of them - if empty then consider all predefined regions
  if( m_regions.empty() ) m_regions = RegionsV();
  for(auto r : m_regions) cout<<"using region: "<<r<<endl;

  //here we decise which histos to plot: a chosen subset or all of them - if empty plot eveything
  if( m_variables.empty() ) m_variables = HistoNamesV();
  for(auto var : m_variables) cout<<"using variable: "<<var<<endl;

  //print categories
  for(auto c : m_categories) cout<<"using categories: "<<c<<endl;

  //template method: define variables, systems, systematics, etc
  if(m_apply_template_method){

    // --- not user-defined yet. Perhaps, user does not wish a template estimate for Z tau el: this can be set in a later version

    m_template_method_regions = TemplateRegions();

    m_template_method_systematics = m_systematics;

    m_template_method_systems =  m_systems;

    m_template_method_variables = m_variables;

    // ---

    cout<<"template method using systematics ";
    for(auto el : m_template_method_systematics)
      cout<<" : "<<el;
    cout<<endl;

    cout<<"template method using systems ";
    for(auto el : m_template_method_systems)
      cout<<" : "<<el;
    cout<<endl;

    cout<<"template method using variables ";
    for(auto el : m_template_method_variables)
      cout<<" : "<<el;
    cout<<endl;

    cout<<"template method using subtraction category ";
    for(auto el : m_template_method_subtraction_categories)
      cout<<" : "<<el;
    cout<<endl;

  }


  //create child histograms
  m_ChildHistosM = ChildHistograms(m_variables, m_samples, m_systematics, m_regions, m_systems);

  cout<<"children size "<<m_ChildHistosM.size()<<endl;

  for ( auto &child : m_ChildHistosM )
    cout<<"child histo booked : "<<get<0>(child.first)<<" : "<<get<1>(child.first)<<" : "<<get<2>(child.first)<<" : "<<child.second<<" : "<<child.second->GetName()<<endl;

  //create parent histograms
  m_ParentHistosM = ParentHistograms(m_variables, m_categories, m_systematics, m_regions, m_systems);

  cout<<"parent size "<<m_ParentHistosM.size()<<endl;

  for ( auto &parent : m_ParentHistosM )
    cout<<"parent histo booked : "<<get<0>(parent.first)<<" : "<<get<1>(parent.first)<<" : "<<get<2>(parent.first)<<" : "<<parent.second<<" : "<<parent.second->GetName()<<endl;

  //modify the stack vector
  for(unsigned int i = 1; i < m_stack_order.size(); i++ )
    m_stack_order[i] = m_stack_order[i-1] + "_" + m_stack_order[i];

  for( std::string ele : m_stack_order)
    std::cout<<"stack: "<< ele <<std::endl;

  //create stack histograms
  m_StackHistosM = StackHistograms(m_variables, m_stack_order, m_systematics, m_regions, m_systems);

  for ( auto &stack : m_StackHistosM )
    cout<<"stack histo booked : "<<get<0>(stack.first)<<" : "<<get<1>(stack.first)<<" : "<<get<2>(stack.first)<<" : "<<stack.second<<" : "<<stack.second->GetName()<<endl;

  //create data histograms
  m_DataHistosM = DataHistograms(m_variables, m_regions, m_systems);

  for ( auto &data : m_DataHistosM )
    cout<<"data histo booked : "<<get<0>(data.first)<<" : "<<get<1>(data.first)<<" : "<<get<2>(data.first)<<" : "<<data.second<<" : "<<data.second->GetName()<<endl;



}
void xPlotter::Iterate(){

  //fill child histograms
  for (const auto &chain : m_ChainsM ){
    for ( auto &child : m_ChildHistosM ){
      if( get<0>(chain.first) == get<0>(child.first) && get<1>(chain.first) == get<1>(child.first) ){
        try{
          //      string categ = m_sample_category[get<0>(child.first)];
          //      for_each(cmd.begin(), cmd.end(), [](char& in){ in = ::toupper(in); });
          //      bool isData = (categ.find("data") != std::string::npos)
          cout<<"draw chain: "<<get<0>(chain.first)<<" : "<<get<1>(chain.first)<<" : "<<get<2>(chain.first)<<" : "<<child.second->GetName() <<" : "<<child.second->GetEntries()<<endl;
          cout<<"draw child: "<<get<0>(child.first)<<" : "<<get<1>(child.first)<<" : "<<get<2>(child.first)<<" : "<<get<3>(child.first)<<" : "<<get<4>(child.first)<<" : "<<endl;
          chain.second -> Draw( (string( child.second->GetTitle()) + " >> " + string(child.second->GetName()) ).c_str(), //draw
                                (get<2>(chain.first) == "data" ? CutData( child.first ) : CutMC( child.first ) ),  // child cuts
                                "goff" ); //switch off graphics
        } catch (std::exception& e){
          cout<<"Exception in drawing chain: "<<e.what()<<endl;
          PrintChild( child.first );
          //throw e; //re-throw exception for the higher up call stack
        }

      }
    }
  }

  //fill out the parent histograms
  for ( auto &child : m_ChildHistosM ){
    cout<<"filling parent: "<<m_sample_category[get<0>(child.first)]<<" for case "
        <<get<0>(child.first)<<" : "<<get<1>(child.first)<<" : "<<get<2>(child.first)<<" : "<<get<3>(child.first)<<" : "<<get<4>(child.first)<<endl;
    m_ParentHistosM[ forward_as_tuple( m_sample_category[get<0>(child.first)],
                                       get<1>(child.first),
                                       get<2>(child.first),
                                       get<3>(child.first),
                                       get<4>(child.first)) ] ->Add( child.second.get(), 1.);

  }

  //data histograms
  for ( auto &parent : m_ParentHistosM ){
    if( get<0>(parent.first).find("data") != string::npos ){
      cout<<"filling data parent:  "<<get<0>(parent.first)<<" : "<<get<1>(parent.first)<<" : "<<get<2>(parent.first)<<" : "<<get<3>(parent.first)<<" : "<<get<4>(parent.first)<<" "<<
        parent.second.get() << " --> data histo "<< m_DataHistosM[ forward_as_tuple(get<4>(parent.first),
                                                                                    get<3>(parent.first),
                                                                                    get<2>(parent.first)) ] <<endl;

      m_DataHistosM[ forward_as_tuple(get<4>(parent.first),
                                      get<3>(parent.first),
                                      get<2>(parent.first)) ] ->Add( parent.second.get(), 1.);
    }
  }

  //distributions calculated by alternative methods, eg template method for QCD jets etc
  if(m_apply_template_method)
    for(const auto & systematic : m_template_method_systematics)
      for(const auto & system : m_template_method_systems)
        for(const auto & variable : m_template_method_variables)
          ApplyTemplateMethod(m_template_method_target_category,
                              m_template_method_source_category,
                              m_template_method_subtraction_categories,
                              m_template_method_regions,
                              systematic,
                              system,
                              variable,
                              m_ParentHistosM);

  //stack histograms
  for ( auto &parent : m_ParentHistosM ){
    for ( auto &stack : m_stack_order ){
      if( stack.find(get<0>(parent.first)) != string::npos ){
        cout<<"filling stack - parent "
            <<get<0>(parent.first)<<" : "<<get<1>(parent.first)<<" : "<<get<2>(parent.first)<<" : "<<get<3>(parent.first)<<" : "<<get<4>(parent.first)
            <<" stack "<<stack
            <<" templated sample category "<<TemplatedSampleCategory( get<1>(parent.first) )
            <<endl;

        auto weight = TemplatedSampleCategory( get<1>(parent.first) )  ? 1. : m_luminosity; //1. is for templated samples

        m_StackHistosM[ forward_as_tuple( stack,
                                          get<1>(parent.first),
                                          get<2>(parent.first),
                                          get<3>(parent.first),
                                          get<4>(parent.first)) ] ->Add( parent.second.get(), weight);
      }
    }
  }

}

void xPlotter::Plot(){

  if(m_StackHistosM.empty()) throw "Plot : histo stack collection is empty - nothing to do";

  if(m_variables.empty()) throw "Plot : couldn't retrieve plotting variables from stacked collection";

  //for each variable we create a canvas

  shared_ptr< TFile > fCanvFile = make_shared<TFile>( m_canvases_output_file_name.c_str(), "RECREATE" );

  //fCanvFile->cd();

  for(const auto &variable : m_variables) //the variable can be read from the stack plots.second -> get title : so, not necessary
    for(const auto &system : m_systems)
      for(const auto &region : m_regions)
        //m_CanvasesM.emplace( forward_as_tuple(variable, system, region), Canvas( forward_as_tuple(variable, system, region) ) );
        Canvas( forward_as_tuple(variable, system, region),
                m_StackHistosM,
                m_stack_order,
                m_stack_colors,
                m_stack_names,
                LogYaxis(variable),
                m_DataHistosM,
                fCanvFile );

  fCanvFile->Close();
}


void xPlotter::Draw( ){ //top-level call function

  try
    {
      Create();
    }
  catch (std::exception& e)
    {
      std::cerr << "Create: exception caught: " << e.what() << '\n';
    }

  try
    {
      Iterate();
    }
  catch (std::exception& e)
    {
      std::cerr << "Iterate: exception caught: " << e.what() << '\n';
    }

  try
    {
      Plot();
    }
  catch (std::exception& e)
    {
      std::cerr << "Plot: exception caught: " << e.what() << '\n';
    }

  Store();
}

bool xPlotter::CheckChain(const RegistryCasesT &info, shared_ptr<TChain> f){

  CheckPtr(f);

  if( ! f->GetNtrees() ) {
    cerr<<"Empty chain - no trees chained :"<<endl;
    cerr<<"\t"<<get<0>(info)<<endl;
    cerr<<"\t"<<get<1>(info)<<endl;
    cerr<<"\t"<<get<2>(info)<<endl;
    return false;
  }

  if( ! f->GetEntries() ) {
    cerr<<"Empty chain - no entries :"<<endl;
    cerr<<"\t"<<get<0>(info)<<endl;
    cerr<<"\t"<<get<1>(info)<<endl;
    cerr<<"\t"<<get<2>(info)<<endl;
    return false;
  }

  return true;
}

void xPlotter::PrintChild(const HistoVarCasesT &chain) const {


  cout<<"Child:"<<endl
      <<"\t id: "<< get<0>(chain)<<endl
      <<"\t variation: "<< get<1>(chain)<<endl
      <<"\t region:"<< get<2>(chain)<<endl
      <<"\t system: "<< get<3>(chain)<<endl
      <<"\t variable: "<< get<4>(chain)<<endl
      <<endl;
}

void xPlotter::Store() {

  unique_ptr< TFile > fChildFile( new TFile( m_childplots_output_file_name.c_str(), "RECREATE" ) );

  fChildFile->cd();

  for ( auto &child : m_ChildHistosM )
    child.second->Write();

  fChildFile->Close();


  unique_ptr< TFile > fParentFile( new TFile( m_parentplots_output_file_name.c_str(), "RECREATE" ) );

  fParentFile->cd();

  for ( auto &parent : m_ParentHistosM )
    parent.second->Write();

  fParentFile->Close();


  unique_ptr< TFile > fStackFile( new TFile( m_stackplots_output_file_name.c_str(), "RECREATE" ) );

  fStackFile->cd();

  for( auto &stack : m_StackHistosM )
    stack.second->Write();

  fStackFile->Close();

}
