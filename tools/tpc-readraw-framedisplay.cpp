#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "TROOT.h"
#include "TH2.h"

#include "tpc/data.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string mapfile = argv[1];
    string infile = argv[2];

    ifstream fin(infile);
    if (!fin.good())
    {
        cerr << "Could not open file: " << infile << endl;
        return 1;
    }
    tpc::Data tpc_data;
    tpc_data.load_map(mapfile);

    size_t npads_max = max_element(tpc::npads.begin(),tpc::npads.end());
    TH2I hist("hist","hist",
        nrings,    -0.5, float(nrings)    - 0.5,
        npads_max, -0.5, float(npads_max) - 0.5);
    hist.Draw();

    clog << "Hit enter to continue to next frame.\n";
    while (tpc_data.read(fin, 1))
    {
        for (size_t ring=0; ring<tpc:nrings; ring++)
        {
            for (size_t pad=0; pad<tpc::npads[ring]; pad++)
            {
                hist.SetBinContent(ring,pad,tpc_data.adc(ring,pad));
            }
        }
        hist.Update();
        cin.ignore();
    }
}
