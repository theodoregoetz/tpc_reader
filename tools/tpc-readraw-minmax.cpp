#include <fstream>
#include <iostream>
#include <string>

#include "tpc/data.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string infile = argv[1];

    ifstream fin(infile);
    if (!fin.good())
    {
        cerr << "Could not open file: " << infile << endl;
        return 1;
    }
    tpc::DataFrame fr;

    tpc::DataFrameElementID min;
    tpc::DataFrameElementID max;

    min.aget    = INT_MAX;
    min.asad    = INT_MAX;
    min.channel = INT_MAX;

    max.aget    = 0;
    max.asad    = 0;
    max.channel = 0;

    while (fr.read(fin))
    {
        for (const auto& elem : fr)
        {
            const tpc::DataFrameElementID& id = elem.id;
            min.aget    = id.aget    < min.aget    ? id.aget    : min.aget;
            min.asad    = id.asad    < min.asad    ? id.asad    : min.asad;
            min.channel = id.channel < min.channel ? id.channel : min.channel;
            max.aget    = id.aget    > max.aget    ? id.aget    : max.aget;
            max.asad    = id.asad    > max.asad    ? id.asad    : max.asad;
            max.channel = id.channel > max.channel ? id.channel : max.channel;
        }
    }

    cout << "min.aget: " << min.aget << endl
         << "min.asad: " << min.asad << endl
         << "min.chnl: " << min.channel << endl;
    cout << "max.aget: " << max.aget << endl
         << "max.asad: " << max.asad << endl
         << "max.chnl: " << max.channel << endl;
}
