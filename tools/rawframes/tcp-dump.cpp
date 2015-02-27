#include <fstream>
#include <iostream>
#include <string>

#include <unistd.h>

#include <boost/asio.hpp>

using namespace std;

using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    tcp::iostream is(tcp::endpoint(tcp::v4(), 46005));

    cout << ios::hex;
    while (true)
    {
        while (!is.rdbuf()->available())
        {
            sleep(1);
        }
        cout << is.rdbuf();
    }
}
