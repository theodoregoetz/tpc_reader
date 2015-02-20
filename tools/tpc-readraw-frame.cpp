#include <fstream>
#include <iostream>
#include <string>

#include "tpc/data_frame.hpp"
#include "tpc/data_map.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string mapfile = argv[1];
    string infile = argv[2];

    tpc::DataMap mp;
    mp.load(mapfile);

    ifstream fin(infile);
    if (!fin.good())
    {
        cerr << "Could not open file: " << infile << endl;
        return 1;
    }
    tpc::DataFrame fr;

    cout << "aget, asad, channel\n";
    while (fr.read(fin))
    {
        const tpc::DataFrameHeader& hd = fr.header();

        cout << "meta type: " << hd.meta_type() << endl;
        cout << "    frame size: " << hd.frame_size() << endl;
        cout << "    frame size: " << hd.frame_size() / sizeof(uint32_t) << endl;
        cout << "    header size: " << hd.header_size() << endl;
        cout << "    event index: " << hd.event_index() << endl;
        cout << "    asad: " << hd.asad_index() << endl;

        cout << "        ring pad asad aget channel cell val word\n";
        for (auto i : {300,301,302,303,304,305})
        {
            const auto& elem = fr[i];
            cout << "       "
                << " " << mp.pad_id(elem.id).ring
                << " " << mp.pad_id(elem.id).id
                << " " << elem.id.asad
                << " " << elem.id.aget
                << " " << elem.id.channel
                << " " << elem.cell
                << " " << elem.val;
            cout << hex
                << " " << fr.buffer()[i];
            cout << dec
                << endl;
        }

        cin.ignore();
    }
}
