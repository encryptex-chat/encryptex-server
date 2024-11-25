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
            start_accept();
            m_io.run();
        } catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    void start_accept()
    {
        m_socket.emplace(m_io);
        m_acceptor.async_accept(m_socket.value(), [this](boost::system::error_code ec) {
            auto connection = connection::create_connection(std::move(m_socket.value()));
            m_connections.insert(connection);
            if (!ec)
            {
                std::print("Client {} connected\n", connection->name());
                connection->start();
            }
            start_accept();
        });
    }

    private:
    boost::asio::io_context& m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    uint32_t m_port;
    std::unordered_set<connection::ptr> m_connections{};
    std::optional<ba::ip::tcp::socket> m_socket;
};

}  // namespace etex