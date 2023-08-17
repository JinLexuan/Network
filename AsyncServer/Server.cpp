#include "Server.h"
#include <iostream>

np::Server::Server(boost::asio::io_context& ioc,
                   const short              port) :
    ioc(ioc),
    acc(ioc,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                       port))
{
    std::cout << "Server start successfully, on port:" << port << std::endl;
    np::Server::startAccept();
}

void np::Server::startAccept()
{
    np::AsyncSession* newSession = new np::AsyncSession(ioc);
    this->acc.async_accept(newSession->socket(),
                           std::bind(&np::Server::handleAccept,
                                     this,
                                     newSession,
                                     std::placeholders::_1));
}

void np::Server::handleAccept(np::AsyncSession*                newSession,
                              const boost::system::error_code& error)
{
    if (error.failed() == true)
    {
        delete newSession;
    }
    else
    {
        newSession->start();
    }

    np::Server::startAccept();
}