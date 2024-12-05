#include "connection.hpp"

#include <ranges>

#include "id_generator.hpp"

namespace etex
{
connection::connection(boost::asio::ip::tcp::socket&& socket) : m_socket{std::move(socket)} {}

auto connection::start() -> boost::asio::awaitable<void>
{
    try
    {
        for (common::message msg;;)
        {
            co_await m_socket.async_read_some(ba::buffer(&msg, common::k_message_size),
                                              ba::use_awaitable);

            auto response =
                details::request_factory(msg.hdr.msg_type)->execute(msg, shared_from_this());

            co_await m_socket.async_write_some(ba::buffer(&response, common::k_message_size),
                                               ba::use_awaitable);
        }
    }
    catch (std::exception& e)
    {
        spdlog::error("Exception: {}", e.what());
    }
}
}  // namespace etex