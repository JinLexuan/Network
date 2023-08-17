#pragma once

#include "AsyncSession.h"
#include <boost/asio.hpp>

namespace np
{
class AsyncSession;
class Server
{
public:
    Server(boost::asio::io_context& ioc,
           const short              port);

private:
    boost::asio::io_context&       ioc;
    boost::asio::ip::tcp::acceptor acc;

    void startAccept();
    void handleAccept(np::AsyncSession*                newSession,
                      const boost::system::error_code& error);
};
} // namespace np
