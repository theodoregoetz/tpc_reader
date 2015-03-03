#ifndef TPC_DATA_TCP_STREAMBUF_HPP
#define TPC_DATA_TCP_STREAMBUF_HPP

#include <boost/asio.hpp>

namespace tpc
{

namespace asio = boost::asio;
using asio::ip::tcp;
typedef asio::streambuf::mutable_buffers_type mutable_buffer;

class DataTcpStreamBuf : public asio::streambuf
{

  private:
    asio::io_service io_service;
    tcp::endpoint endpoint;
    tcp::acceptor acceptor;
    tcp::socket socket;

    static const size_t buffer_max = 262144; // = 2**18 bytes

    size_t receive()
    {
        // get some data from source over socket
        mutable_buffer mbuf = this->prepare(buffer_max);
        size_t nbytes_received = socket.receive(mbuf);
        this->commit(nbytes_received);
        return nbytes_received;
    }

  public:
    DataTcpStreamBuf(int port = 46005)
    : io_service()
    , endpoint(tcp::v4(),port)
    , acceptor(io_service,endpoint)
    , socket(io_service)
    {
        acceptor.accept(socket);
    }

    void accept()
    {
        acceptor.accept(socket);
    }

    int_type underflow()
    {
        size_t nbytes_received = this->receive();
        if (!nbytes_received)
        {
            return traits_type::eof();
        }
        return asio::streambuf::underflow();
    }
};

} // namespace tpc

#endif // TPC_DATA_TCP_STREAMBUF_HPP
