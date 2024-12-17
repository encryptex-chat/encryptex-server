#include "connection.hpp"

namespace etex
{
connection::connection(boost::asio::ip::tcp::socket&& socket) : m_socket{std::move(socket)} {}

auto connection::start(msg_handler_t&& income_message_handler) -> boost::asio::awaitable<void>
{
    m_on_message = std::move(income_message_handler);
    try
    {
        for (common::message msg;;)
        {
            co_await m_socket.async_read_some(ba::buffer(&msg, common::k_message_size),
                                              ba::use_awaitable);
            auto response = co_await m_on_message(msg);

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
