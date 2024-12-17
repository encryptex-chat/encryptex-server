#pragma once

#include <boost/asio.hpp>
#include <expected>
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

    auto message_handler(const common::message& msg)
        -> ba::awaitable<std::expected<common::message, common::error_type>>;

    auto find_client(uint64_t client_id)
        -> std::expected<std::shared_ptr<connection>, common::error_type> const;

    private:
    boost::asio::io_context& m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_set<std::shared_ptr<connection>> m_connections{};
    std::optional<ba::ip::tcp::socket> m_socket;
};

}  // namespace etex
