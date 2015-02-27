#include <inttypes.h> // uint32_t
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/asio.hpp>

using namespace std;

namespace asio = boost::asio;
using boost::asio::ip::tcp;
typedef asio::streambuf::mutable_buffers_type mutable_buffer;

union Word
{
    char     c[4];
    uint32_t i;
};

uint32_t get_uint32(const vector<char>& v, size_t start, size_t size)
{
    Word w = {0,0,0,0};
    for (size_t i=0; i<size; i++)
    {
        w.c[i] = v[start+size-i-1];
    }
    return w.i;
}

size_t get_data_size(const vector<char>& v)
{
    return (get_uint32(v,1,3) - get_uint32(v,8,2)) * 64;
}

int main(int argc, char** argv)
{
    static const size_t buffer_max = 262144; // 2**18
    static const size_t header_size = 128;
    vector<char> header(header_size);
    vector<char> data;

    try
    {
        asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 46005));

        asio::streambuf sbuf;
        istream is(&sbuf);

        tcp::socket socket(io_service);
        acceptor.accept(socket);

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
                    is.read(&data.front(),data_size);
                    have_data = true;
                }
            }
            else
            {
                if (sbuf.size() >= header_size)
                {
                    is.read(&header.front(),header_size);
                    data_size = get_data_size(header);
                    data.resize(data_size);
                    have_header = true;
                }
            }

            if (have_data && have_header)
            {
                cout.write(&header.front(),header.size());
                cout.write(&data.front(),data.size());
                cout.flush();
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
