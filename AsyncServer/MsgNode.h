#pragma once
#include <cstddef>

#define MAX_LENGTH  1024 * 2
#define HEAD_LENGTH 2

namespace np
{
class MsgNode
{
public:
    MsgNode(const char*       msg,
            const std::size_t maxLen);
    MsgNode(const std::size_t maxLen);
    void clear();
    ~MsgNode();

    std::size_t totalLen;
    std::size_t curLen;
    char*       data;
};

} // namespace np
