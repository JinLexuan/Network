#include "Endpoint.h"

#include <boost/asio.hpp>
#include <iostream>
#include <ostream>
#include <string>

int np::Endpoint::createClientEndpoint()
{
    const std::string    rawIPAddress = "127.4.8.1";
    const unsigned short portNum      = 3333;

    boost::system::error_code      errorCode;
    const boost::asio::ip::address ipAddress
        = boost::asio::ip::address::from_string(rawIPAddress, errorCode);

    if (errorCode.failed() == true)
    {
        std::cout << "Failed to parse the ip address! Error code =  "
                  << errorCode.value() << ". Message: "
                  << errorCode.message() << std::endl;

        return errorCode.value();
    }

    boost::asio::ip::tcp::endpoint ep(ipAddress, portNum);

    return 0;
}

int np::Endpoint::createServerEndpoint()
{
    const unsigned short                 portNum   = 3333;
    const boost::asio::ip::address       ipAddress = boost::asio::ip::address_v6::any();
    const boost::asio::ip::tcp::endpoint ep(ipAddress, portNum);

    return 0;
}

int np::Endpoint::createTCPSocket()
{
    boost::asio::io_context      ioContext;
    const boost::asio::ip::tcp   protocol = boost::asio::ip::tcp::v4();
    boost::asio::ip::tcp::socket sock(ioContext);
    boost::system::error_code    errorCode;
    sock.open(protocol, errorCode);

    if (errorCode.failed() == true)
    {
        std::cout << "Failed to open the socket! Error code =  "
                  << errorCode.value() << ". Message: "
                  << errorCode.message() << std::endl;

        return errorCode.value();
    }

    return 0;
}

int np::Endpoint::createAcceptorSocket()
{
    boost::asio::io_context ioContext;
    // boost::asio::ip::tcp::acceptor acceptor(ioContext);
    // const boost::asio::ip::tcp     protocol = boost::asio::ip::tcp::v4();
    // boost::system::error_code      errorCode;
    // acceptor.open(protocol, errorCode);

    // if (errorCode.failed() == true)
    // {
    //     std::cout << "Failed to open the acceptor socket! Error code =  "
    //               << errorCode.value() << ". Message: "
    //               << errorCode.message() << std::endl;

    //     return errorCode.value();
    // }

    const unsigned short portNum = 3333;

    boost::asio::ip::tcp::acceptor acc(ioContext,
                                       boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                                      portNum));

    return 0;
}

int np::Endpoint::bindAcceptorSocket()
{
    const unsigned short           portNum = 3333;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(), portNum);

    boost::asio::io_context        ioContext;
    boost::asio::ip::tcp::acceptor acceptor(ioContext, endpoint.protocol());
    boost::system::error_code      errorCode;
    acceptor.bind(endpoint, errorCode);

    if (errorCode.failed() == true)
    {
        std::cout << "Failed to bind the acceptor socket! Error code =  "
                  << errorCode.value() << ". Message: "
                  << errorCode.message() << std::endl;

        return errorCode.value();
    }

    return 0;
}

int np::Endpoint::connectToEnd()
{
    const std::string    rawIPAddress = "192.168.1.124";
    const unsigned short portNum      = 3333;
    try
    {
        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(rawIPAddress),
                                                      portNum);

        boost::asio::io_context      ioContext;
        boost::asio::ip::tcp::socket sock(ioContext, endpoint.protocol());
        sock.connect(endpoint);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

int np::Endpoint::connectDNStoEnd()
{
    const std::string       host    = "llfc.club";
    const std::string       portNum = "3333";
    boost::asio::io_context ioContext;

    const boost::asio::ip::tcp::resolver::query resolverQuery(host,
                                                              portNum,
                                                              boost::asio::ip::tcp::resolver::query::numeric_service);

    boost::asio::ip::tcp::resolver resolver(ioContext);

    try
    {
        const boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(resolverQuery);
        boost::asio::ip::tcp::socket                   sock(ioContext);
        boost::asio::connect(sock, it);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}

int np::Endpoint::acceptNewConnection()
{
    const int               BLACKLOG_SIZE = 30; // NOLINT
    const unsigned short    portNum       = 3333;
    boost::asio::io_context ioContext;

    const boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), portNum);

    try
    {
        boost::asio::ip::tcp::acceptor acceptor(ioContext,
                                                ep.protocol());
        acceptor.bind(ep);
        acceptor.listen(BLACKLOG_SIZE);
        boost::asio::ip::tcp::socket sock(ioContext);
        acceptor.accept(sock);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code =  " << e.code()
                  << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}
