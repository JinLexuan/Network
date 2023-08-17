#include "Server.h"
#include <boost/asio.hpp>
#include <iostream>

int main()
{
    try
    {
        boost::asio::io_context ioc;

        np::Server s(ioc, 10086);
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}