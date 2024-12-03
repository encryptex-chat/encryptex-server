#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <unordered_set>

#include "common.hpp"
#include "connection.hpp"
namespace etex
{
namespace ba = boost::asio;

class server
{
    public:
    server(ba::io_context& io);

    auto run() -> void;

    auto start_accept() -> ba::awaitable<void>;

    private:
    boost::asio::io_context& m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_set<connection::conn_ptr> m_connections{};
    std::vector<common::message_header> m_msges;
    std::optional<ba::ip::tcp::socket> m_socket;
};

}  // namespace etex