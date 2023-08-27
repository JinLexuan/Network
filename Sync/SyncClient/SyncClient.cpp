#include "SyncClient.h"

#include <array>
#include <boost/asio.hpp>
#include <cstddef>
#include <iostream>

void np::SyncClient::startClient()
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
            return;
        }

        std::cout << "Enter message: ";
        std::array<char, np::SyncClient::MAX_LENGTH> request;
        std::cin.getline(request.data(), np::SyncClient::MAX_LENGTH);
        const std::size_t requestLength = strlen(request.data());

        std::array<char, np::SyncClient::MAX_LENGTH> sendData = { 0 };

        std::memcpy(sendData.data(), &requestLength, 2);
        std::memcpy(sendData.data() + 2, request.data(), requestLength);
        boost::asio::write(sock,
                           boost::asio::buffer(sendData,
                                               requestLength + 2));

        std::array<char, np::SyncClient::HEAD_LENGTH> replyHead;

        boost::asio::read(sock,
                          boost::asio::buffer(replyHead,
                                              np::SyncClient::HEAD_LENGTH));

        short msgLen = 0;

        std::memcpy(&msgLen, replyHead.data(), np::SyncClient::HEAD_LENGTH);
        std::array<char, np::SyncClient::MAX_LENGTH> msg;

        boost::asio::read(sock,
                          boost::asio::buffer(msg.data(), msgLen));
        std::cout << "Reply is: ";
        std::cout.write(msg.data(), static_cast<std::streamsize>(msgLen));
        std::cout << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}