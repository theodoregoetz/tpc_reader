/**
 * compile with:

rootcint -f main_window_dict.cpp -c main_window.hpp main_window_linkdef.hpp

g++ -o test tpc-readraw-framedisplay.cpp main_window.cpp main_window_dict.cpp `root-config --cflags --glibs`

 *
 **/

#include "TApplication.h"
#include "TGClient.h"

#include "main_window.hpp"

using namespace std;

int main(int argc, char** argv)
{
    TApplication app("app", &argc, argv);
    MainWindow win(gClient->GetRoot(),200,200);
    app.Run();
}
