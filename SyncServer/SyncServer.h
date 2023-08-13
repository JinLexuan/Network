#pragma once

#include <boost/asio.hpp>
#include <set>
#include <thread>

namespace np
{
class SyncServer
{
public:
    void start();

private:
    static const int MAX_LENGTH = 1024;

    std::set<std::shared_ptr<std::thread>> threadSet;

    static void session(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

    void server(const unsigned short     port,
                boost::asio::io_context& ioContext);
};

} // namespace np
