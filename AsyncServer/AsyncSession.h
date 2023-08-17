#pragma once

#include <boost/asio.hpp>

namespace np
{
class AsyncSession
{
public:
    AsyncSession(boost::asio::io_context& ioc);

    boost::asio::ip::tcp::socket& socket();

    void start();

private:
    boost::asio::ip::tcp::socket sock;
    enum
    {
        max_length = 1024
    };
    std::array<char, max_length> data;

    void handleWrite(const boost::system::error_code& error);
    void handleRead(const boost::system::error_code& error,
                    std::size_t                      bytes_transferred);
};
} // namespace np
