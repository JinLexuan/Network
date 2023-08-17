#include "MsgNode.h"
#include <cstring>

np::MsgNode::MsgNode(const char* msg, const std::size_t totalLen) :
    totalLen(totalLen),
    curLen(0),
    msg(new char[totalLen])
{
    std::memcpy(this->msg, msg, this->totalLen);
}

np::MsgNode::MsgNode(const std::size_t totalLen) :
    totalLen(totalLen),
    curLen(0),
    msg(new char[totalLen])
{
}

np::MsgNode::~MsgNode()
{
    delete[] this->msg;
}
