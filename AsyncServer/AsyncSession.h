#pragma once

#include "Server.h"
#include <boost/asio.hpp>
#include <queue>

#include "MsgNode.h"

namespace np
{
class Server;
class AsyncSession final : public std::enable_shared_from_this<np::AsyncSession>
{
public:
    AsyncSession(boost::asio::io_context& ioc,
                 np::Server*              server);
    ~AsyncSession();

    boost::asio::ip::tcp::socket& socket();

    void start();
    std::string getUuid() const;

    void send(char* msg, int maxLength);

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

    std::queue<std::shared_ptr<np::MsgNode>> sendQ;

    std::mutex sendLock;
};
} // namespace np
