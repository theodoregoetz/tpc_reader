#include <fstream>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

using namespace std;

namespace asio = boost::asio;
using boost::asio::ip::tcp;
typedef asio::streambuf::mutable_buffers_type mutable_buffer;

int main(int argc, char** argv)
{
    try
    {
        // setup socket on port 46005
        asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 46005);
        tcp::acceptor acceptor(io_service, endpoint);
        tcp::socket socket(io_service);

        // set maximum mutable buffer size for incoming data
        static const size_t buffer_max = 262144; // = 2**18 bytes

        // setup input stream and it's streambuf
        asio::streambuf sbuf;
        istream is(&sbuf);

        // start listening on socket
        acceptor.accept(socket);

        // setup objects to hold incoming data
        static const size_t header_size = sizeof(uint32_t);
        uint32_t header;
        vector<char> data;

        bool have_header = false;
        bool have_data = false;
        size_t data_size;
        size_t nbytes_received;
        while (true)
        {
            // get some data from source over socket
            mutable_buffer mbuf = sbuf.prepare(buffer_max);
            nbytes_received = socket.receive(mbuf);
            sbuf.commit(nbytes_received);

            while (true)
            {
                if (!have_header)
                {
                    if (sbuf.size() >= header_size)
                    {
                        is.read(
                            reinterpret_cast<char*>(&header),
                            header_size);
                        data_size = header;
                        have_header = true;
                    }
                    else
                    {
                        // need to receive more data
                        break;
                    }
                }
                else
                {
                    if (sbuf.size() >= data_size)
                    {
                        data.resize(data_size);
                        is.read(&data.front(),data_size);
                        have_data = true;
                    }
                    else
                    {
                        // need to receive more data
                        break;
                    }
                }

                if (have_data && have_header)
                {
                    header = 0;
                    data.clear();
                    have_header = false;
                    have_data = false;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }
}
