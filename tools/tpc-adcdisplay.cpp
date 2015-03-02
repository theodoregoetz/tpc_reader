#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "TROOT.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TH1.h"

#include "tpc/data.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string mapfile = argv[1];
    string infile = argv[2];

    ifstream fin(infile.c_str());
    if (!fin.good())
    {
        cerr << "Could not open file: " << infile << endl;
        return 1;
    }
    tpc::Data tpc_data;
    tpc_data.load_map(mapfile);

    size_t npads_max = *max_element(tpc::npads,tpc::npads+tpc::nrings);

    TApplication theApp("theApp", &argc, argv);

    TCanvas can("can");
    TH1I hist("hist","hist",
        tpc::ncells, -0.5, float(tpc::ncells) - 0.5);
    hist.Draw();

    clog << "Hit enter to continue to next frame.\n";
    while (tpc_data.read(fin))
    {
        const vector<int>& adc = tpc_data.adc(1,11);
        for (int cell = 0; cell < tpc::ncells; cell++)
        {
            hist.SetBinContent(cell,double(adc[cell]));
        }
        can.Update();
        can.WaitPrimitive();
        tpc_data.clear();
    }
    theApp.Run(true);
}
