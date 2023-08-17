#include "Session.h"
#include <functional>
#include <memory>

np::Session::Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket) :
    isSendPending(false),
    isReceivePending(false),
    sock(socket) // NOLINT
{
}

void np::Session::connect(const boost::asio::ip::tcp::endpoint& ep)
{
    this->sock->connect(ep);
}

void np::Session::writeCallBack(const boost::system::error_code& errorCode,
                                std::size_t                      transferredBytes)
{
    if (errorCode.failed() == true)
    {
        return;
    }

    if (auto& firstBuf = this->sendQueue.front();
        firstBuf->curLen + transferredBytes < firstBuf->totalLen)
    {
        firstBuf->curLen += transferredBytes;
        this->sock->async_write_some(boost::asio::buffer(firstBuf->msg + firstBuf->curLen,
                                                         firstBuf->totalLen - firstBuf->curLen),
                                     std::bind(&np::Session::writeCallBack,
                                               this,
                                               std::placeholders::_1,
                                               std::placeholders::_2));
        return;
    }

    this->sendQueue.pop();

    if (this->sendQueue.empty() == true)
    {
        this->isSendPending = false;
    }
    else
    {
        auto& firstBuf = this->sendQueue.front();
        this->sock->async_write_some(boost::asio::buffer(firstBuf->msg + firstBuf->curLen,
                                                         firstBuf->totalLen - firstBuf->curLen),
                                     std::bind(&np::Session::writeCallBack,
                                               this,
                                               std::placeholders::_1,
                                               std::placeholders::_2));
    }
}

void np::Session::writeToSocket(const std::string& buf)
{
    this->sendQueue.emplace(std::make_shared<np::MsgNode>(buf.c_str(), buf.length()));

    if (this->isSendPending == false)
    {
        this->sock->async_write_some(boost::asio::buffer(buf),
                                     std::bind(&np::Session::writeCallBack,
                                               this,
                                               std::placeholders::_1,
                                               std::placeholders::_2));

        this->isSendPending = true;
    }
}

void np::Session::writeAllCallBack(const boost::system::error_code& errorCode,
                                   std::size_t                      transferredBytes)
{
    (void) transferredBytes;

    if (errorCode.failed() == true)
    {
        return;
    }

    this->sendQueue.pop();

    if (this->sendQueue.empty() == true)
    {
        this->isSendPending = false;
    }
    else
    {
        auto& firstBuf = this->sendQueue.front();
        this->sock->async_send(boost::asio::buffer(firstBuf->msg + firstBuf->curLen,
                                                   firstBuf->totalLen - firstBuf->curLen),
                               std::bind(&np::Session::writeCallBack,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
    }
}

void np::Session::writeAllToSocket(const std::string& buf)
{
    this->sendQueue.emplace(std::make_shared<np::MsgNode>(buf.c_str(), buf.length()));

    if (this->isSendPending == true)
    {
        return;
    }

    this->sock->async_send(boost::asio::buffer(buf),
                           std::bind(&np::Session::writeAllCallBack,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2));

    this->isSendPending = true;
}

void np::Session::readFromSocket()
{
    if (this->isReceivePending == true)
    {
        return;
    }
    const int receiveSize = 1024;
    this->receiveNode     = std::make_shared<np::MsgNode>(receiveSize);

    this->sock->async_read_some(boost::asio::buffer(this->receiveNode->msg,
                                                    this->receiveNode->totalLen),
                                std::bind(&np::Session::readCallBack,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2));

    this->isReceivePending = true;
}

void np::Session::readCallBack(const boost::system::error_code& errorCode,
                               std::size_t                      transferredBytes)
{
    if (errorCode.failed() == true)
    {
        return;
    }

    this->receiveNode->curLen += transferredBytes;
    if (this->receiveNode->curLen < this->receiveNode->totalLen)
    {
        this->sock->async_read_some(boost::asio::buffer(this->receiveNode->msg + this->receiveNode->curLen,
                                                        this->receiveNode->totalLen - this->receiveNode->curLen),
                                    std::bind(&np::Session::readCallBack,
                                              this,
                                              std::placeholders::_1,
                                              std::placeholders::_2));

        return;
    }

    this->isReceivePending = false;
    this->receiveNode      = nullptr;
}

void np::Session::readAllFromSocket()
{
    if (this->isReceivePending == true)
    {
        return;
    }
    const int receiveSize = 1024;
    this->receiveNode     = std::make_shared<np::MsgNode>(receiveSize);

    this->sock->async_receive(boost::asio::buffer(this->receiveNode->msg,
                                                  this->receiveNode->totalLen),
                              std::bind(&np::Session::readCallBack,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));

    this->isReceivePending = true;
}

void np::Session::readAllCallBack(const boost::system::error_code& errorCode,
                                  std::size_t                      transferredBytes)
{
    if (errorCode.failed() == true)
    {
        return;
    }

    this->receiveNode->curLen += transferredBytes;
    this->isReceivePending = false;
    this->receiveNode      = nullptr;
}