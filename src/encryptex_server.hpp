#pragma once
#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <print>
#include <unordered_set>

#include "connection.hpp"
namespace etex
{
namespace ba = boost::asio;

class server
{
    public:
    server(ba::io_context& io, uint32_t port) :
        m_io{io}, m_acceptor{m_io, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)}, m_port{port}
    {}

    void run()
    {
        try
        {
            ba::co_spawn(m_io, start_accept(), ba::detached);
            m_io.run();
        } catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    ba::awaitable<void> start_accept()
    {
        for (;;)
        {
            ba::ip::tcp::socket socket = co_await m_acceptor.async_accept(ba::use_awaitable);
            auto connection            = connection::create_connection(std::move(socket));
            m_connections.insert(connection);
            std::print("Client {} connected\n", connection->name());
            ba::co_spawn(m_io, connection->start(), ba::detached);
        }
    }

    private:
    boost::asio::io_context& m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    uint32_t m_port;
    std::unordered_set<connection::ptr> m_connections{};
    std::optional<ba::ip::tcp::socket> m_socket;
};

}  // namespace etex