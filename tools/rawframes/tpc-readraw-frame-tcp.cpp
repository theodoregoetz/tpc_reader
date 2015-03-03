#include <fstream>
#include <iostream>
#include <string>

#include "tpc/data_frame.hpp"
#include "tpc/data_map.hpp"
#include "tpc/data_streambuf.hpp"

using namespace std;

void operator<<(ostream& os, const tpc::DataFrame& fr)
{
    const tpc::DataFrameHeader& hd = fr.header();

    os << "meta type: " << hd.meta_type() << endl;
    os << "    frame size (bytes): " << hd.frame_size() << endl;
    os << "    frame size (words): " << hd.frame_size() / sizeof(uint32_t) << endl;
    os << "    header size: " << hd.header_size() << endl;
    os << "    event index: " << hd.event_index() << endl;
    os << "    read offset: " << hd.read_offset() << endl;
    os << "    asad: " << hd.asad_index() << endl;
    os << "    last cell(aget=0,1,2,3):"
       << " " << hd.last_cell(0)
       << " " << hd.last_cell(1)
       << " " << hd.last_cell(2)
       << " " << hd.last_cell(3)
       << endl;
    os << "    actual frame size: " << fr.size() << endl;
    os << "    actual buffer size: " << fr.buffer().size() << endl;

    os << "        aget channel cell val word\n";
    int fsize = fr.size();
    static const size_t idxs[] = {
        300,301,302,303,304,305,
        fsize-4,fsize-3,fsize-2,fsize-1 };
    for (size_t i=0; i<10; i++)
    {
        size_t idx = idxs[i];
        const tpc::DataFrameElement& elem = fr[idx];
        os << "       ";
        os.width(5); os << elem.id.aget;
        os.width(5); os << elem.id.channel;
        os.width(5); os << elem.cell;
        os.width(5); os << elem.val;
        os << hex << " " << fr.buffer()[i] << dec << endl;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Error: Missing TPC map file\n\n";
        cout << "usage: " << argv[0] << " path/to/tpc_map.txt\n";
        cout << endl;
        return 1;
    }
    string mapfile = argv[1];

    tpc::DataMap mp;
    mp.load(mapfile);

    istream* is;

    if (argc < 3)
    {
        is = new istream(new tpc::DataTcpStreamBuf());
    }
    else if (argc < 4)
    {
        if (string(argv[2]) == string("stdin"))
        {
            is = new istream(cin.rdbuf());
        }
        else
        {
            is = new ifstream(argv[2],ios::binary);
        }
    }

    try
    {
        tpc::DataFrame fr;
        bool have_header = false;
        bool have_data = false;
        while (is->good() && !is->eof())
        {
            if (!have_header)
            {
                if (!fr.read_header(*is))
                {
                    // error reading header
                    break;
                }
                have_header = true;
            }
            else
            {
                if (!fr.read_data(*is))
                {
                    // error reading data
                    break;
                }
                have_data = true;
            }

            if (have_data && have_header)
            {
                cout << fr;
                have_header = false;
                have_data = false;
                cin.ignore();
            }
        }
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }
}
