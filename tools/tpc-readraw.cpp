#include <fstream>
#include <iostream>
#include <string>

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

    int count = 0;
    while (tpc_data.read(fin))
    {
        for (auto x : tpc_data.adc(22,193))
        {
            cout << " " << x;
        }
        cout << endl;
        cin.ignore();
    }
}
