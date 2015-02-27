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
