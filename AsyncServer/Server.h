#pragma once

#include "AsyncSession.h"
#include <boost/asio.hpp>
#include <map>
#include <memory>

namespace np
{
class AsyncSession;
class Server
{
public:
    Server(boost::asio::io_context& ioc,
           const short              port);
    void removeSession(const std::string& uuid);

private:
    boost::asio::io_context&       ioc;
    boost::asio::ip::tcp::acceptor acc;
    std::map<std::string, std::shared_ptr<np::AsyncSession>> sessionsMap;

    void startAccept();
    void handleAccept(std::shared_ptr<np::AsyncSession>& newSession,
                      const boost::system::error_code&   error);
};
} // namespace np
