#pragma once

#include <boost/asio.hpp>

namespace np
{
namespace Buffer
{
    void useConstBuffer();
    void useBufferStr();
    void useBufferArray();
    void writeToSocket(boost::asio::ip::tcp::socket& sock);
    int sendDataByWriteSome();
    int sendDataBySend();
    int sendDataByWrite();
    std::string readFromSocket(boost::asio::ip::tcp::socket& sock);
    int readDataByReadSome();
    int readDataByReceive();
    int readDataByRead();

} // namespace Buffer
} // namespace np
