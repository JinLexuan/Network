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

void np::Server::removeSession(const std::string& uuid)
{
    this->sessionsMap.erase(uuid);
}

void np::Server::startAccept()
{
    std::shared_ptr<np::AsyncSession> newSession = std::make_shared<np::AsyncSession>(this->ioc, this);
    this->acc.async_accept(newSession->socket(),
                           std::bind(&np::Server::handleAccept,
                                     this,
                                     newSession,
                                     std::placeholders::_1));
}

void np::Server::handleAccept(std::shared_ptr<np::AsyncSession>& newSession,
                              const boost::system::error_code&   error)
{
    if (error.failed() == false)
    {
        newSession->start();
        this->sessionsMap.insert(std::make_pair(newSession->getUuid(), newSession));
    }

    np::Server::startAccept();
}