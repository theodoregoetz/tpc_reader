#include <fstream>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "tpc/data_frame.hpp"
#include "tpc/data_map.hpp"

using namespace std;

namespace asio = boost::asio;
using boost::asio::ip::tcp;
typedef asio::streambuf::mutable_buffers_type mutable_buffer;

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
    string mapfile = argv[1];

    tpc::DataMap mp;
    mp.load(mapfile);

    static const size_t buffer_max = 262144; // 2**18

    try
    {
        asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 46005));

        asio::streambuf sbuf;
        istream is(&sbuf);

        tcp::socket socket(io_service);
        acceptor.accept(socket);

        tpc::DataFrame fr;
        bool have_header = false;
        bool have_data = false;
        size_t data_size;
        size_t nbytes_received;
        while (true)
        {
            mutable_buffer mbuf = sbuf.prepare(buffer_max);
            nbytes_received = socket.receive(mbuf);
            sbuf.commit(nbytes_received);

            if (have_header)
            {
                if (sbuf.size() >= data_size)
                {
                    fr.read_data(is);
                    have_data = true;
                }
            }
            else
            {
                if (sbuf.size() >= tpc::frame_header_size)
                {
                    fr.read_header(is);
                    data_size = fr.header().data_size();
                    have_header = true;
                }
            }

            if (have_data && have_header)
            {
                cout << fr;
                have_header = false;
                have_data = false;
            }
        }
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }
}
