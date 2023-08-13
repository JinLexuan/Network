#include <array>
#include <boost/asio.hpp>
#include <cstddef>
#include <iostream>

const int MAX_LENGTH = 1024;

int main()
{
    try
    {
        boost::asio::io_context        ioc;
        boost::asio::ip::tcp::endpoint remoteEndpoint(boost::asio::ip::address::from_string("127.0.0.1"),
                                                      10086);
        boost::asio::ip::tcp::socket   sock(ioc);
        boost::system::error_code      errorCode = boost::asio::error::host_not_found;

        if (sock.connect(remoteEndpoint, errorCode);
            errorCode.failed() == true)
        {
            std::cout << "Connect failed, code: " << errorCode.value()
                      << " error msg is " << errorCode.message() << std::endl;
            return 0;
        }

        std::cout << "Enter message: ";
        std::array<char, MAX_LENGTH> request;
        std::cin.getline(request.data(), MAX_LENGTH);
        const std::size_t requestLength = strlen(request.data());
        boost::asio::write(sock, boost::asio::buffer(request, requestLength));

        std::array<char, MAX_LENGTH> reply;

        const std::size_t replyLength = boost::asio::read(sock,
                                                          boost::asio::buffer(reply, requestLength));
        std::cout << "Reply is: ";
        std::cout.write(reply.data(), static_cast<std::streamsize>(replyLength));
        std::cout << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
