#include <fstream>
#include <iostream>
#include <string>

#include "tpc/data_frame.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string infile = argv[1];

    ifstream fin(infile.c_str());
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
        tpc::DataFrame::const_iterator elem;
        for (elem = fr.begin(); elem != fr.end(); ++elem)
        {
            const tpc::DataFrameElementID& id = elem->id;
            min.aget    = id.aget    < min.aget    ? id.aget    : min.aget;
            min.asad    = id.asad    < min.asad    ? id.asad    : min.asad;
            min.channel = id.channel < min.channel ? id.channel : min.channel;
            max.aget    = id.aget    > max.aget    ? id.aget    : max.aget;
            max.asad    = id.asad    > max.asad    ? id.asad    : max.asad;
            max.channel = id.channel > max.channel ? id.channel : max.channel;
        }
    }

    cout << "      aget asad chnl\n";
    cout << "min: ";
    cout.width(5); cout << min.aget;
    cout.width(5); cout << min.asad;
    cout.width(5); cout << min.channel;
    cout << endl;
    cout << "max: ";
    cout.width(5); cout << max.aget;
    cout.width(5); cout << max.asad;
    cout.width(5); cout << max.channel;
    cout << endl;
}
