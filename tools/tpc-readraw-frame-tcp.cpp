#include <fstream>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "tpc/data_frame.hpp"
#include "tpc/data_map.hpp"

using namespace std;

namespace asio = boost::asio;
using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    string mapfile = argv[1];

    tpc::DataMap mp;
    mp.load(mapfile);

    try
    {
        asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 46005));

        asio::streambuf sbuf;
        asio::streambuf::mutable_buffers_type mbuf = sbuf.prepare(pow(2,21));
        istream is(&sbuf);

        tcp::socket socket(io_service);
        cout << "socket is setup.\n";
        acceptor.accept(socket);
        cout << "accepting data.\n";

        tpc::DataFrame fr;
        bool have_header = false;
        bool have_data = false;
        size_t data_size;
        while (true)
        {
            size_t n = socket.receive(mbuf);
            sbuf.commit(n);

            if (! have_header)
            {
                if (sbuf.size() >= tpc::frame_header_size)
                {
                    cout << "reading header (buf size: " << sbuf.size() << ").\n";
                    fr.read_header(is);
                    cout << "    sbuf size after read: " << sbuf.size() << endl;
                    data_size = fr.header().data_size();
                    have_header = true;
                }
            }
            else
            {
                if (sbuf.size() >= data_size)
                {
                    cout << "reading data (buf size: " << sbuf.size() << ").\n";
                    fr.read_data(is);
                    cout << "    sbuf size after read: " << sbuf.size() << endl;
                    have_data = true;
                }
            }

            if (have_data && have_header)
            {
                have_data = false;
                have_header = false;

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
            }
        }
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }
}
