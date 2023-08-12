#include "Buffer.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

void np::Buffer::useConstBuffer()
{
    const std::string buf = "Hello world!";

    const boost::asio::const_buffer        asioBuf(buf.c_str(), buf.length());
    std::vector<boost::asio::const_buffer> bufferSequence;
    bufferSequence.push_back(asioBuf);
}

void np::Buffer::useBufferStr()
{
    boost::asio::const_buffers_1 outputBuf = boost::asio::buffer("hello world");
    (void) outputBuf;
}

void np::Buffer::useBufferArray()
{
    const std::size_t BUF_SIZE_BYTES = 20; // NOLINT

    std::unique_ptr<std::array<char, BUF_SIZE_BYTES>> buf;

    auto inputBuf = boost::asio::buffer(static_cast<void*>(buf.get()),
                                        BUF_SIZE_BYTES);

    (void) inputBuf;
}

void np::Buffer::writeToSocket(boost::asio::ip::tcp::socket& sock)
{
    const std::string buf = "hello world";

    std::size_t totalBytesWritten = 0;

    while (totalBytesWritten != buf.length())
    {
        totalBytesWritten += sock.write_some(boost::asio::buffer(buf.substr(totalBytesWritten).c_str(),
                                                                 buf.length() - totalBytesWritten));
    }

    // buf.substr(totalBytesWritten).c_str() equals to buf.c_str() + totalBytesWritten
}

int np::Buffer::sendDataByWriteSome()
{
    const std::string    rawIPAddress = "192.168.3.11";
    const unsigned short portNum      = 3333;

    try
    {
        const boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(rawIPAddress),
                                                portNum);

        boost::asio::io_context      ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        np::Buffer::writeToSocket(sock);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

int np::Buffer::sendDataBySend()
{
    const std::string    rawIPAddress = "192.168.3.11";
    const unsigned short portNum      = 3333;

    try
    {
        const boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(rawIPAddress),
                                                portNum);

        boost::asio::io_context      ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        const std::string buf = "hello world";

        const std::size_t sendLength = sock.send(boost::asio::buffer(buf.c_str(),
                                                                     buf.length()));
        if (sendLength == 0)
        {
            return 0;
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

int np::Buffer::sendDataByWrite()
{
    const std::string    rawIPAddress = "192.168.3.11";
    const unsigned short portNum      = 3333;

    try
    {
        const boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(rawIPAddress),
                                                portNum);

        boost::asio::io_context      ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        const std::string buf = "hello world";

        const std::size_t sendLength = boost::asio::write(sock,
                                                          boost::asio::buffer(buf.c_str(), buf.length()));
        if (sendLength == 0)
        {
            return 0;
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

std::string np::Buffer::readFromSocket(boost::asio::ip::tcp::socket& sock)
{
    const unsigned char MESSAGE_SIZE = 7; // NOLINT

    std::array<char, MESSAGE_SIZE> buf;

    std::size_t totalBytesRead = 0;
    while (totalBytesRead != MESSAGE_SIZE)
    {
        totalBytesRead += sock.read_some(boost::asio::buffer(buf.data() + totalBytesRead, // NOLINT
                                                             MESSAGE_SIZE - totalBytesRead));
    }

    return { buf.data(), MESSAGE_SIZE };
}

int np::Buffer::readDataByReadSome()
{
    const std::string    rawIPAddress = "127.0.0.1";
    const unsigned short portNum      = 3333;
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(rawIPAddress),
                                          portNum);
        boost::asio::io_context        ioc;
        boost::asio::ip::tcp::socket   sock(ioc, ep.protocol());
        sock.connect(ep);
        np::Buffer::readFromSocket(sock);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

int np::Buffer::readDataByReceive()
{
    const std::string    rawIPAddress = "127.0.0.1";
    const unsigned short portNum      = 3333;
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(rawIPAddress),
                                          portNum);
        boost::asio::io_context        ioc;
        boost::asio::ip::tcp::socket   sock(ioc, ep.protocol());
        sock.connect(ep);

        const unsigned char         BUFF_SIZE = 7; // NOLINT
        std::array<char, BUFF_SIZE> bufferReceive;

        const std::size_t receiveLength = sock.receive(boost::asio::buffer(bufferReceive.data(),
                                                                           BUFF_SIZE));
        if (receiveLength == 0)
        {
            std::cout << "Receive fails" << std::endl;
            return 0;
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

int np::Buffer::readDataByRead()
{
    const std::string    rawIPAddress = "127.0.0.1";
    const unsigned short portNum      = 3333;
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(rawIPAddress),
                                          portNum);
        boost::asio::io_context        ioc;
        boost::asio::ip::tcp::socket   sock(ioc, ep.protocol());
        sock.connect(ep);

        const unsigned char         BUFF_SIZE = 7; // NOLINT
        std::array<char, BUFF_SIZE> bufferReceive;

        const std::size_t receiveLength = boost::asio::read(sock,
                                                            boost::asio::buffer(bufferReceive.data(), BUFF_SIZE));
        if (receiveLength == 0)
        {
            std::cout << "Receive fails" << std::endl;
            return 0;
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}
