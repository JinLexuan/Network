#include "AsyncSession.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <functional>
#include <iostream>

np::AsyncSession::AsyncSession(boost::asio::io_context& ioc,
                               np::Server*              server) :
    server(server),
    sock(ioc)
{
    boost::uuids::uuid myUuid = boost::uuids::random_generator()();
    this->uuid                = boost::uuids::to_string(myUuid);
}

np::AsyncSession::~AsyncSession()
{
    std::cout << "Call session destructor." << std::endl;
}

boost::asio::ip::tcp::socket& np::AsyncSession::socket()
{
    return this->sock;
}

std::string np::AsyncSession::getUuid() const
{
    return this->uuid;
}

void np::AsyncSession::start()
{
    std::memset(this->data.data(), 0, this->max_length);
    this->sock.async_read_some(boost::asio::buffer(this->data,
                                                   this->max_length),
                               std::bind(&np::AsyncSession::handleRead,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2,
                                         shared_from_this()));
}

void np::AsyncSession::send(char* msg,
                            int   maxLength)
{
    bool isPending = false;

    std::lock_guard<std::mutex> lock(sendLock);

    if (this->sendQ.empty() == false)
    {
        isPending = false;
    }

    this->sendQ.push(std::make_shared<np::MsgNode>(msg, maxLength));
    if (isPending == true)
    {
        return;
    }

    boost::asio::async_write(this->sock,
                             boost::asio::buffer(msg,
                                                 maxLength),
                             std::bind(&np::AsyncSession::handleWrite,
                                       this,
                                       std::placeholders::_1,
                                       shared_from_this()));
}

void np::AsyncSession::handleRead(const boost::system::error_code&  error,
                                  std::size_t                       bytes_transferred,
                                  std::shared_ptr<np::AsyncSession> selfShared)
{
    if (error.failed() == true)
    {
        std::cout << "Read error" << std::endl;
        this->server->removeSession(this->uuid);
    }
    else
    {
        std::cout << "Server received data: " << this->data.data() << std::endl;
        np::AsyncSession::send(this->data.data(), static_cast<int>(bytes_transferred));
        std::memset(this->data.data(), 0, this->max_length);
        this->sock.async_read_some(boost::asio::buffer(this->data,
                                                       this->max_length),
                                   std::bind(&np::AsyncSession::handleRead,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             selfShared));
    }
}

void np::AsyncSession::handleWrite(const boost::system::error_code&  error,
                                   std::shared_ptr<np::AsyncSession> selfShared)
{
    if (error.failed() == true)
    {
        std::cout << "Write error:" << error.value() << std::endl;
        this->server->removeSession(this->getUuid());
    }
    else
    {
        std::lock_guard<std::mutex> lock(this->sendLock);
        this->sendQ.pop();

        if (this->sendQ.empty() == false)
        {
            auto& msgNode = sendQ.front();
            boost::asio::async_write(this->sock,
                                     boost::asio::buffer(msgNode->msg,
                                                         msgNode->totalLen),
                                     std::bind(&np::AsyncSession::handleWrite,
                                               this,
                                               std::placeholders::_1,
                                               selfShared));
        }
    }
}
