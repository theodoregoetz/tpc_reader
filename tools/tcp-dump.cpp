#include <fstream>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

using namespace std;

using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    tcp::iostream is("127.0.0.1", "46005");

    cout << ios::hex;
    unsigned int x;
    while (is.read(reinterpret_cast<char*>(&x),sizeof(unsigned int)))
    {
        cout << x;
    }
}
