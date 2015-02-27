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

    ifstream fin(infile.c_str());
    if (!fin.good())
    {
        cerr << "Could not open file: " << infile << endl;
        return 1;
    }
    tpc::DataFrame fr;

    while (fr.read(fin))
    {
        const tpc::DataFrameHeader& hd = fr.header();

        cout << "meta type: " << hd.meta_type() << endl;
        cout << "    frame size (bytes): " << hd.frame_size() << endl;
        cout << "    frame size (words): " << hd.frame_size() / sizeof(uint32_t) << endl;
        cout << "    header size: " << hd.header_size() << endl;
        cout << "    event index: " << hd.event_index() << endl;
        cout << "    read offset: " << hd.read_offset() << endl;
        cout << "    asad: " << hd.asad_index() << endl;
        cout << "    last cell(aget=0,1,2,3):"
             << " " << hd.last_cell(0)
             << " " << hd.last_cell(1)
             << " " << hd.last_cell(2)
             << " " << hd.last_cell(3)
             << endl;
        cout << "    actual frame size: " << fr.size() << endl;
        cout << "    actual buffer size: " << fr.buffer().size() << endl;

        cout << "        ring pad aget channel cell val word\n";
        int fsize = fr.size();
        static const size_t idxs[] = {
            300,301,302,303,304,305,
            fsize-4,fsize-3,fsize-2,fsize-1 };
        for (size_t i=0; i<10; i++)
        {
            size_t idx = idxs[i];
            const tpc::DataFrameElement& elem = fr[idx];
            cout << "       ";
            cout.width(5); cout << mp.pad_id(elem.id).ring;
            cout.width(5); cout << mp.pad_id(elem.id).id;
            cout.width(5); cout << elem.id.aget;
            cout.width(5); cout << elem.id.channel;
            cout.width(5); cout << elem.cell;
            cout.width(5); cout << elem.val;
            cout << hex
                << " " << fr.buffer()[i];
            cout << dec
                << endl;
        }

        cin.ignore();
    }
}
