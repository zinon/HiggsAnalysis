#include <xPlotter/xPlotter.h>



xPlotter::xPlotter(){ 

  m_ChainsM.clear();
  m_ChildHistosM.clear();
  m_ParentHistosM.clear();
  m_CanvasesM.clear();
  m_sample_category.clear();
  m_variables.clear();

  //hardcoded
  m_systematics = {"NOMINAL"}; //nominal should be always at the top

  m_regions = {"SignalRegion"};

  m_systems = {"tau_muon"};

}


void xPlotter::Register(const string& type, const string &category, const string& wildcard, const int number){

  m_registry.emplace_back( to_string(number), type, wildcard  );

  if (find(m_samples.begin(), m_samples.end(), to_string(number) ) ==m_samples.end())
    m_samples.push_back( to_string(number) );
 
  if (find(m_categories.begin(), m_categories.end(), category) == m_categories.end())
    m_categories.push_back(category);

  if( m_sample_category.find( to_string(number) ) == m_sample_category.end() )
    m_sample_category.emplace( to_string(number), category );
      
}

void xPlotter::Variables(initializer_list<string> list){

  m_variables = list;
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

void xPlotter::Create(){

  //create chains
  for ( const auto &reg : m_registry ){
    for ( const auto &syst : m_systematics ){
      shared_ptr< TChain > fchain( new TChain( syst.c_str()) );
      fchain->Add( get<2>(reg).c_str());
      
      if( CheckChain( reg, fchain) )
	m_ChainsM.emplace( forward_as_tuple( get<0>(reg), syst) , fchain );
      else
	throw std::invalid_argument("null or empty chain");

    }
  }

  //here we decise which histos to plot: a chosen subset or all of them - if empty plot eveything
  if( m_variables.empty() ) m_variables = HistoNamesV();

  //create child histograms
  m_ChildHistosM = ChildHistograms(m_variables, m_samples, m_systematics, m_regions, m_systems);

  cout<<"children size "<<m_ChildHistosM.size()<<endl;
  
  //create parent histograms
  m_ParentHistosM = ParentHistograms(m_variables, m_categories, m_systematics, m_regions, m_systems);

  cout<<"parent size "<<m_ParentHistosM.size()<<endl;

  //modify the stack vector 
  for(unsigned int i = 1; i < m_stack_order.size(); i++ )
    m_stack_order[i] = m_stack_order[i-1] + "_" + m_stack_order[i];
  
  for( std::string ele : m_stack_order)
    std::cout<<"stack: "<< ele <<std::endl;

  //create stack histograms
  m_StackHistosM = StackHistograms(m_variables, m_stack_order, m_systematics, m_regions, m_systems);

}



void xPlotter::Iterate(){

  //fill child histograms
  for (const auto &chain : m_ChainsM ){
    for ( auto &child : m_ChildHistosM ){
      if( get<0>(chain.first) == get<0>(child.first) && get<1>(chain.first) == get<1>(child.first) ){
	try{
	  chain.second -> Draw( (string( child.second->GetTitle()) + " >> " + string(child.second->GetName()) ).c_str(), //draw
				Cut( child.first ),  // child cuts
				"goff" ); //switch off graphics
	} catch (std::exception& e){
	  PrintChild( child.first );
	  //throw e; //re-throw exception for the higher up call stack 
	}

      }
    }
  }

  //fill out the parent histograms
  for ( auto &child : m_ChildHistosM ){
    cout<<"parent -> "<<m_sample_category[get<0>(child.first)]<<endl;
    m_ParentHistosM[ forward_as_tuple( m_sample_category[get<0>(child.first)], 
				       get<1>(child.first),
				       get<2>(child.first), 
				       get<3>(child.first), 
				       get<4>(child.first)) ] ->Add( child.second.get(), 1.);

  }

  //stack histograms
  for ( auto &parent : m_ParentHistosM ){
    for ( auto &stack : m_stack_order ){
      cout<<"parent --> "<<get<0>(parent.first)<<" stack "<<stack<<endl;
      if( stack.find(get<0>(parent.first)) != string::npos )
	m_StackHistosM[ forward_as_tuple( stack, 
					  get<1>(parent.first),
					  get<2>(parent.first), 
					  get<3>(parent.first), 
					  get<4>(parent.first)) ] ->Add( parent.second.get(), 1.);
      
    }
  }
  
}

void xPlotter::Plot(){

  if(m_StackHistosM.empty()) throw "Plot : histo stack collection is empty - nothing to do";

  //get variables if list is empty
  // th list of variable wil be automaticall propagated and included into the maps
  /*cout<<"Plot: to be done in Create() with m_variables = get list of histo names"<<endl;

  if(m_variables.empty()){
    for ( auto &stack : m_StackHistosM ){
      cout<<"stacked plot: "<<stack.second->GetName()<<endl;
      cout << "\t " << get<0>(stack.first) << endl
	   << "\t " << get<1>(stack.first) << endl
	   << "\t " << get<2>(stack.first) << endl
	   << "\t " << get<3>(stack.first) << endl
	   << "\t " << get<4>(stack.first) << endl;
      string var(stack.second->GetTitle());
      if ( find(m_variables.begin(), m_variables.end(), var) == m_variables.end()){
	  m_variables.push_back(var);
	  cout<<var<<endl;
      }
    }
  }
  */

  if(m_variables.empty()) throw "Plot : couldn't retrieve plotting variables from stacked collection";

  //for each variable we create a canvas

  shared_ptr< TFile > fCanvFile = make_shared<TFile>( m_canvases_output_file_name.c_str(), "RECREATE" );
  
  //fCanvFile->cd();

  //for( auto &canvas : m_CanvasesM )
  //canvas.second->Write();

  for(const auto &variable : m_variables) //the variable can be read from the stack plots.second -> get title : so, not necessary
    for(const auto &system : m_systems)
      for(const auto &region : m_regions)
	//m_CanvasesM.emplace( forward_as_tuple(variable, system, region), Canvas( forward_as_tuple(variable, system, region) ) );
	Canvas( forward_as_tuple(variable, system, region),  m_StackHistosM, m_stack_order, fCanvFile);
  
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

  unique_ptr< TFile > fChildFile( new TFile( m_childrenplots_output_file_name.c_str(), "RECREATE" ) );
  
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
