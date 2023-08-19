#pragma once

#include "Server.h"
#include <boost/asio.hpp>

namespace np
{
class Server;
class AsyncSession final : public std::enable_shared_from_this<np::AsyncSession>
{
public:
    AsyncSession(boost::asio::io_context& ioc,
                 np::Server*              server);

    boost::asio::ip::tcp::socket& socket();

    void start();
    std::string getUuid() const;
    ~AsyncSession();

private:
    np::Server* server;
    std::string uuid;

    boost::asio::ip::tcp::socket sock;
    enum
    {
        max_length = 1024
    };
    std::array<char, max_length> data;

    void handleWrite(const boost::system::error_code&  error,
                     std::shared_ptr<np::AsyncSession> selfShared);
    void handleRead(const boost::system::error_code&  error,
                    std::size_t                       bytes_transferred,
                    std::shared_ptr<np::AsyncSession> selfShared);
};
} // namespace np
