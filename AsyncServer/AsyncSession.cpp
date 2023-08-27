#include "AsyncSession.h"
#include "MsgNode.h"

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
    std::memset(this->data.data(), 0, static_cast<std::size_t>(MAX_LENGTH));
    this->sock.async_read_some(boost::asio::buffer(this->data,
                                                   static_cast<std::size_t>(MAX_LENGTH)),
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

        return;
    }

    int copyLen = 0;
    while (bytes_transferred > 0)
    {
        if (this->isHeadParsed == false)
        {
            if (np::AsyncSession::onHeadNotParsed(copyLen, bytes_transferred, selfShared) == false)
            {
                return;
            }

            continue;
        }

        if (np::AsyncSession::onHeadParsed(copyLen, bytes_transferred, selfShared) == false)
        {
            return;
        }
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
                                     boost::asio::buffer(msgNode->data,
                                                         msgNode->totalLen),
                                     std::bind(&np::AsyncSession::handleWrite,
                                               this,
                                               std::placeholders::_1,
                                               selfShared));
        }
    }
}

bool np::AsyncSession::onHeadParsed(int&                               copyLen,
                                    std::size_t&                       bytes_transferred,
                                    std::shared_ptr<np::AsyncSession>& selfShared)
{
    std::cout << "total len: " << this->msgNode->totalLen << " cur len: " << this->msgNode->curLen << std::endl;
    const std::size_t remainedMsgSize = this->msgNode->totalLen - this->msgNode->curLen;
    // The length of received data is smaller than the expected size, which means data is not completely received.
    // Then put all the data into message node.
    if (bytes_transferred < remainedMsgSize)
    {
        np::AsyncSession::onCurMsgNotComplete(copyLen, bytes_transferred, selfShared);
        return false;
    }

    // Current data completely recieved.
    np::AsyncSession::onCurMsgComplete(static_cast<short>(remainedMsgSize), copyLen, bytes_transferred);

    // Continue to handle remained data.
    if (bytes_transferred <= 0)
    {
        np::AsyncSession::clearData();
        np::AsyncSession::hangOnHandleRead(selfShared);
        return false;
    }

    return true;
}

bool np::AsyncSession::onHeadNotParsed(int&                               copyLen,
                                       std::size_t&                       bytes_transferred,
                                       std::shared_ptr<np::AsyncSession>& selfShared)
{
    // Received data is less than HEAD_LENGTH
    if (bytes_transferred + this->headNode->curLen < HEAD_LENGTH)
    {
        np::AsyncSession::onLessThanHead(copyLen, bytes_transferred, selfShared);

        return false;
    }

    np::AsyncSession::onGreaterThanHead(copyLen, bytes_transferred);

    // Get the head data.
    short dataLen = 0;
    std::memcpy(&dataLen, this->headNode->data, HEAD_LENGTH);
    std::cout << "The length of data is " << dataLen << std::endl;

    // Judge whether the length is invalid.
    if (dataLen > MAX_LENGTH)
    {
        std::cout << "The length of data is invalid; " << dataLen << std::endl;
        this->server->removeSession(this->uuid);

        return false;
    }

    this->msgNode = std::make_shared<np::MsgNode>(dataLen);

    return np::AsyncSession::onHeadParsed(copyLen, bytes_transferred, selfShared);
}

void np::AsyncSession::onLessThanHead(int&                               copyLen,
                                      std::size_t&                       bytes_transferred,
                                      std::shared_ptr<np::AsyncSession>& selfShared)
{
    std::memcpy(this->headNode->data + this->headNode->curLen,
                this->data.data() + copyLen,
                bytes_transferred);
    this->headNode->curLen += bytes_transferred;
    np::AsyncSession::clearData();
    np::AsyncSession::hangOnHandleRead(selfShared);
}

void np::AsyncSession::onGreaterThanHead(int&         copyLen,
                                         std::size_t& bytes_transferred)
{
    // Received data is greater than HEAD_LENGTH.
    // headRemained: Uncopied head part.
    const int headRemain = HEAD_LENGTH - this->headNode->curLen;
    std::memcpy(this->headNode->data + this->headNode->curLen,
                this->data.data() + copyLen,
                headRemain);
    copyLen += headRemain;
    bytes_transferred -= headRemain;

    // Get the head data.
    short dataLen = 0;
    std::memcpy(&dataLen, this->headNode->data, HEAD_LENGTH);
    std::cout << "The length of data is " << dataLen << std::endl;
}

void np::AsyncSession::onCurMsgComplete(const short  dataLen,
                                        int&         copyLen,
                                        std::size_t& bytes_transferred)
{
    std::memcpy(this->msgNode->data + this->msgNode->curLen,
                this->data.data() + copyLen,
                dataLen);

    this->msgNode->curLen += dataLen;
    copyLen += dataLen;
    bytes_transferred -= dataLen;
    this->msgNode->data[this->msgNode->totalLen] = '\0';
    std::cout << "Server received data: " << this->msgNode << std::endl;
    np::AsyncSession::send(this->msgNode->data,
                           static_cast<int>(this->msgNode->totalLen));

    this->isHeadParsed = false;
    this->msgNode->clear();
}

void np::AsyncSession::onCurMsgNotComplete(int&                               copyLen,
                                           std::size_t&                       bytes_transferred,
                                           std::shared_ptr<np::AsyncSession>& selfShared)
{
    std::memcpy(this->msgNode->data + this->msgNode->curLen,
                this->data.data() + copyLen,
                bytes_transferred);
    this->msgNode->curLen += bytes_transferred;

    np::AsyncSession::clearData();
    np::AsyncSession::hangOnHandleRead(selfShared);
    this->isHeadParsed = true; // Already parsed the head.
}

void np::AsyncSession::clearData()
{
    std::memset(this->data.data(), 0, static_cast<std::size_t>(MAX_LENGTH));
}

void np::AsyncSession::hangOnHandleRead(std::shared_ptr<np::AsyncSession>& selfShared)
{
    this->sock.async_read_some(boost::asio::buffer(this->data.data(),
                                                   static_cast<std::size_t>(MAX_LENGTH)),
                               std::bind(&np::AsyncSession::handleRead,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2,
                                         selfShared));
}