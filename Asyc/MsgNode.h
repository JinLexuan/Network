#pragma once
#include <cstddef>

namespace np
{
class MsgNode
{
public:
    MsgNode(const char*       msg,
            const std::size_t totalLen);
    MsgNode(const std::size_t totalLen);
    ~MsgNode();

    std::size_t totalLen;
    std::size_t curLen;
    char*       data;
};

} // namespace np
