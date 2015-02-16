#include <fstream>
#include <iostream>
#include <string>

#include "tpc_data.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string mapfile = argv[1];
    string infile = argv[2];

    ifstream fin(infile);
    tpc::Data tpc_data;
    tpc_data.load_map(mapfile);

    int count = 0;
    while (tpc_data.read(fin))
    {
        for (auto x : tpc_data.adc(tpc::Side::LEFT,2,20))
        {
            cout << ", " << x;
        }
        cout << endl;
        if (count++ > 10) { break; }
    }
}
