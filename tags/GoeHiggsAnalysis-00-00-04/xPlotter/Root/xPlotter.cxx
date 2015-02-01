#include <xPlotter/xPlotter.h>



xPlotter::xPlotter(){ 

  //  m_ChainV.clear();

  //std::unique_ptr<int> p1(new int(5));

  //std::unique_ptr< TFile > fOutputFile( TFile::Open( "test", "RECREATE" ) );

  m_ChainsM.clear();
  m_ChildHistosM.clear();
  m_ParentHistosM.clear();
  m_sample_category.clear();

  //hardcoded
  m_systematics = {"NOMINAL"};

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

  //create child histograms
  m_ChildHistosM = ChildHistograms(m_samples, m_systematics, m_regions, m_systems);

  cout<<m_ChildHistosM.size()<<endl;
  
  //create parent histograms
  m_ParentHistosM = ParentHistograms(m_categories, m_systematics, m_regions, m_systems);

  cout<<m_ParentHistosM.size()<<endl;
  //   decltype(systematic)  name(

  //create stack histograms
  m_StackHistosM = StackHistograms(m_stack_order, m_systematics, m_regions, m_systems);

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
    //    m_ChildHistosM[]
    
  }


  cout<<"ciao"<<endl;
  for ( auto &parent : m_ParentHistosM ){
    
    cout<< parent.second->GetName()<<endl;
  }


  for ( auto &child : m_ChildHistosM ){
    
    m_ParentHistosM[ forward_as_tuple( m_sample_category[get<0>(child.first)], 
				       get<1>(child.first),
				       get<2>(child.first), 
				       get<3>(child.first), 
				       get<4>(child.first)) ] ->Add( child.second.get(), 1.);

  }


  /*
  string systematic = "NOMINAL";
  string system = "tau_muon";
  string region = "SignalRegion";

  vector<string> regions { "SignalRegion", "RegionA"};
  vector<string> systematics { "NOMINAL", "TESup"};

  map<CasesT, shared_ptr<TH1D> >  mhistos = ParentHistograms(systematics, regions); // to-do : insert vectors for all arguments

  //child histograms
  for (auto & chain : m_ChainV ){
    PrintChain(chain);
   
    for (auto &histo : get<5>(chain) )
      get<4>(chain) -> Draw( (string(histo->GetTitle()) + " >> " + string(histo->GetName())).c_str(), //draw
			     Cut( get<0>(chain), string(histo->GetTitle()), system, region ),  // number, variable, system, region x sample weight
			     "goff" ); //switch off graphics
    			     
  }

  
  //parent histograms
  for (auto & chain : m_ChainV ){
    for (auto &histo : get<5>(chain) ){
      Fill( get<> )
    }

  }
  */
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
      <<"\t varible: "<< get<4>(chain)<<endl
      <<endl;
}

void xPlotter::Store() {

  unique_ptr< TFile > fGenFile( new TFile( m_generic_output_file_name.c_str(), "RECREATE" ) );
  
  fGenFile->cd();

  for ( auto &child : m_ChildHistosM )
      child.second->Write();

  fGenFile->Close();


  unique_ptr< TFile > fSpecFile( new TFile( m_specific_output_file_name.c_str(), "RECREATE" ) );
  
  fSpecFile->cd();

  for ( auto &parent : m_ParentHistosM )
      parent.second->Write();

  fSpecFile->Close();

}
