#include "SyncServer.h"

#include <array>
#include <cstddef>
#include <iostream>

void np::SyncServer::session(std::shared_ptr<boost::asio::ip::tcp::socket> sock) // NOLINT if pass by reference, then fail to compile.
{
    try
    {
        while (true)
        {
            std::array<char, MAX_LENGTH> data;
            std::memset(data.data(), '\0', MAX_LENGTH); // Clear the data
            boost::system::error_code error;

            const std::size_t length = sock->read_some(boost::asio::buffer(data.data(), MAX_LENGTH),
                                                       error);
            if (error == boost::asio::error::eof)
            {
                std::cout << "Connection closed by peer" << std::endl;
                break;
            }
            else if (error)
            {
                throw boost::system::system_error(error);
            }

            std::cout << "Receive from " << sock->remote_endpoint().address().to_string() << std::endl;
            std::cout << "Message is " << data.data() << std::endl;
            boost::asio::write(*sock, boost::asio::buffer(data.data(), length));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    }
}

void np::SyncServer::server(const unsigned short     port,
                            boost::asio::io_context& ioContext)
{
    boost::asio::ip::tcp::acceptor acc(ioContext,
                                       boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                                      port));

    while (true)
    {
        std::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(ioContext));
        acc.accept(*socket);
        std::shared_ptr<std::thread> t = std::make_shared<std::thread>(np::SyncServer::session, socket);
        this->threadSet.insert(t);
        std::cout << this->threadSet.size() << std::endl;
    }
}

void np::SyncServer::start()
{
    try
    {
        boost::asio::io_context ioContext;
        np::SyncServer::server(10086, ioContext);
        for (const auto& t : this->threadSet)
        {
            t->join();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception" << e.what() << std::endl;
    }
}
