#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <queue>

#include "MsgNode.h"

namespace np
{
class Session
{
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void connect(const boost::asio::ip::tcp::endpoint& ep);

    void writeCallBack(const boost::system::error_code& errorCode,
                       std::size_t                      transferredBytes);
    void writeToSocket(const std::string& buf);

    void writeAllCallBack(const boost::system::error_code& errorCode,
                          std::size_t                      transferredBytes);
    void writeAllToSocket(const std::string& buf);

    void readFromSocket();
    void readCallBack(const boost::system::error_code& errorCode,
                      std::size_t                      transferredBytes);

    void readAllFromSocket();
    void readAllCallBack(const boost::system::error_code& errorCode,
                         std::size_t                      transferredBytes);

private:
    bool isSendPending;
    bool isReceivePending;

    std::shared_ptr<np::MsgNode> receiveNode;

    std::queue<std::shared_ptr<np::MsgNode>> sendQueue;

    std::shared_ptr<boost::asio::ip::tcp::socket> sock;
};
} // namespace np