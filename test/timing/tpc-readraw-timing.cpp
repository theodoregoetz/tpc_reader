#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include "tpc/data.hpp"

using namespace std;

typedef chrono::high_resolution_clock hrclock;
template <typename T>
float to_seconds(const T& d)
{
    int ms = chrono::duration_cast<chrono::milliseconds>(d).count();
    return float(ms) / 1000.;
}

int main(int argc, char** argv)
{
    string mapfile = argv[1];
    string infile = argv[2];

    ifstream fin(infile);
    tpc::Data tpc_data;
    tpc_data.load_map(mapfile);

    vector<int> elapsed_times;

    int count = 10;
    while (count-->0)
    {
        int asad = tpc::nasads;
        auto start = hrclock::now();
        while (asad-->0)
        {
            tpc_data.read(fin);
        }
        auto end = hrclock::now();
        auto elapsed_sec = to_seconds(end - start);
        cout << "Time to read " << tpc::nasads << " data frames: "
             << elapsed_sec << " seconds\n";
        elapsed_times.push_back(elapsed_sec);
    }
    auto sum = accumulate(
        elapsed_times.begin(),
        elapsed_times.end(),
        0 );
    auto avg = float(sum) / elapsed_times.size();
    cout << " Average: " << avg << " seconds\n";
}
