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

    std::array<char, static_cast<std::size_t>(MAX_LENGTH)> data;

    void handleWrite(const boost::system::error_code&  error,
                     std::shared_ptr<np::AsyncSession> selfShared);
    void handleRead(const boost::system::error_code&  error,
                    std::size_t                       bytes_transferred,
                    std::shared_ptr<np::AsyncSession> selfShared);

    std::queue<std::shared_ptr<np::MsgNode>> sendQ;

    std::mutex sendLock;

    bool isHeadParsed;

    std::shared_ptr<np::MsgNode> headNode;
    std::shared_ptr<np::MsgNode> msgNode;

    bool onHeadParsed(int&                               copyLen,
                      std::size_t&                       bytes_transferred,
                      std::shared_ptr<np::AsyncSession>& selfShared);
    bool onHeadNotParsed(int&                               copyLen,
                         std::size_t&                       bytes_transferred,
                         std::shared_ptr<np::AsyncSession>& selfShared);

    void onLessThanHead(int&                               copyLen,
                        std::size_t&                       bytes_transferred,
                        std::shared_ptr<np::AsyncSession>& selfShared);
    void onGreaterThanHead(int&         copyLen,
                           std::size_t& bytes_transferred);
    void onCurMsgComplete(const short  dataLen,
                          int&         copyLen,
                          std::size_t& bytes_transferred);
    void onCurMsgNotComplete(int&                               copyLen,
                             std::size_t&                       bytes_transferred,
                             std::shared_ptr<np::AsyncSession>& selfShared);

    void clearData();
    void hangOnHandleRead(std::shared_ptr<np::AsyncSession>& selfShared);
};
} // namespace np
