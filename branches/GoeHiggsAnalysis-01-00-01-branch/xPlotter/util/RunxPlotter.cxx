#include "xPlotter/xPlotter.h"

//#include <memory>
//#include <iostream>
//#include <cstdlib>

#include <initializer_list>

using namespace std;

int main( int argc, char* argv[] ) {


  unique_ptr<xPlotter> p(new xPlotter());

  p->GenericFile("generalplots.root");

  p->SpecificFile("goodplots.root");

  p->Register("signal", "Ztautau", "tmpdata/Out_Ztt/out*root*", 147808);

  p->Register("background", "Wjets", "tmpdata/Out_Wmunu/out*167743*root*", 167743);
  p->Register("background", "Wjets", "tmpdata/Out_Wmunu/out*167744*root*", 167744);
  p->Register("background", "Wjets", "tmpdata/Out_Wmunu/out*167745*root*", 167745);

  p->Register("background", "Wjets", "tmpdata/Out_Wtaunu/out*167746*root*", 167746);
  p->Register("background", "Wjets", "tmpdata/Out_Wtaunu/out*167747*root*", 167747);
  p->Register("background", "Wjets", "tmpdata/Out_Wtaunu/out*167748*root*", 167748);

  p->StackOrder( {"QCDjets", "Wjets", "top", "Ztautau"} );
  

  p->Draw();


  cout<<"Done!"<<endl;

  return 0;

}
