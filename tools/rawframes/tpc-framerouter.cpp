#include <iostream>
#include <fstream>
#include <vector>

#include <boost/asio.hpp>

#include "tpc/data_frame.hpp"

using namespace std;

namespace asio = boost::asio;
using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    vector<tpc::DataFrame> fr;

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

        // start listening on socket for a connection
        acceptor.accept(socket);

        // setup objects to hold incoming data
        static const size_t header_size = sizeof(uint32_t);
        uint32_t header;
        vector<char> data;

        bool have_header = false;
        bool have_data = false;
        size_t data_size;
        size_t nbytes_received;
        while (socket.is_open())
        {
            // get some data from source over socket
            mutable_buffer mbuf = sbuf.prepare(buffer_max);
            nbytes_received = socket.receive(mbuf);
            sbuf.commit(nbytes_received);

            while (nbytes_received)
            {
                if (have_header)
                {
                    if (nbytes_received >= data_size)
                    {
                        if (!fr.read_data(is))
                        {
                            break;
                        }
                        nbytes_received -= data_size;
                        have_data = true;
                    }
                    else
                    {
                        // need to receive more data
                        break;
                    }
                }
                else
                {
                    if (nbytes_received >= tpc::frame_header_size)
                    {
                        if (!fr.read_header(is))
                        {
                            break;
                        }
                        nbytes_received -= tpc::frame_header_size;
                        data_size = fr.header().data_size();
                        have_header = true;
                    }
                    else
                    {
                        // need to receive more data
                        break;
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
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }
}
