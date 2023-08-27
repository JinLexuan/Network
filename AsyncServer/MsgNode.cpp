#include "MsgNode.h"
#include <cstring>

#include "boost/asio.hpp"

np::MsgNode::MsgNode(const char*       msg,
                     const std::size_t maxLen) :
    totalLen(maxLen + HEAD_LENGTH),
    curLen(0),
    data(new char[totalLen + 1])
{
    std::size_t maxLenHost = boost::asio::detail::socket_ops::host_to_network_short(maxLen);
    std::memcpy(this->data, &maxLenHost, HEAD_LENGTH);
    std::memcpy(this->data + HEAD_LENGTH, msg, maxLen);

    this->data[this->totalLen] = '\0';
}

np::MsgNode::MsgNode(const std::size_t maxLen) :
    totalLen(maxLen),
    curLen(0),
    data(new char[this->totalLen + 1])
{
}

void np::MsgNode::clear()
{
    std::memset(this->data, 0, this->totalLen);
    this->curLen = 0;
}

np::MsgNode::~MsgNode()
{
    delete[] this->data;
}
