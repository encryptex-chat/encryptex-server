#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <unordered_set>

#include "connection.hpp"
#include "message.hpp"
namespace etex
{
namespace ba = boost::asio;

class server
{
    public:
    server(ba::io_context& io) : m_io{io}, m_acceptor{m_io}
    {
        ba::ip::tcp::resolver resolver(io);
        ba::ip::tcp::endpoint ep = *(resolver.resolve(ba::ip::tcp::resolver::query{
            common::k_server_addr, std::to_string(common::k_server_port)}));

        m_acceptor = ba::ip::tcp::acceptor{m_io, ep};
    }

    void run()
    {
        try
        {
            ba::co_spawn(m_io, start_accept(), ba::detached);
            spdlog::info("Encryptex server started on address {}:{}",
                         m_acceptor.local_endpoint().address().to_string(),
                         m_acceptor.local_endpoint().port());
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
            auto conn                  = std::make_shared<connection>(std::move(socket));
            m_connections.insert(conn);
            ba::co_spawn(m_io, conn->start(), ba::detached);
        }
    }

    private:
    boost::asio::io_context& m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_set<connection::conn_ptr> m_connections{};
    std::vector<common::message_header> m_msges;
    std::optional<ba::ip::tcp::socket> m_socket;
};

}  // namespace etex