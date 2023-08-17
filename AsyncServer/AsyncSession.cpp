#include "AsyncSession.h"

#include <functional>
#include <iostream>

np::AsyncSession::AsyncSession(boost::asio::io_context& ioc) :
    sock(ioc)
{
}

boost::asio::ip::tcp::socket& np::AsyncSession::socket()
{
    return this->sock;
}

void np::AsyncSession::start()
{
    std::memset(this->data.data(), 0, this->max_length);
    this->sock.async_read_some(boost::asio::buffer(this->data,
                                                   this->max_length),
                               std::bind(&np::AsyncSession::handleRead,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
}

void np::AsyncSession::handleRead(const boost::system::error_code& error,
                                  std::size_t                      bytes_transferred)
{
    if (error.failed() == true)
    {
        std::cout << "Read error" << std::endl;
        delete this;
    }
    else
    {
        std::cout << "Server received data: " << this->data.data() << std::endl;
        boost::asio::async_write(this->sock,
                                 boost::asio::buffer(this->data.data(),
                                                     bytes_transferred),
                                 std::bind(&np::AsyncSession::handleWrite,
                                           this,
                                           std::placeholders::_1));
    }
}

void np::AsyncSession::handleWrite(const boost::system::error_code& error)
{
    if (error.failed() == true)
    {
        std::cout << "Write error:" << error.value() << std::endl;
        delete this;
    }
    else
    {
        std::memset(this->data.data(), 0, this->max_length);
        this->sock.async_read_some(boost::asio::buffer(this->data,
                                                       this->max_length),
                                   std::bind(&np::AsyncSession::handleRead,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
    }
}
