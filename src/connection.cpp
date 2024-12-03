#include "connection.hpp"

namespace etex
{
connection::connection(boost::asio::ip::tcp::socket&& socket) : m_socket{std::move(socket)} {}

auto connection::start() -> boost::asio::awaitable<void>
{
    try
    {
        for (;;)
        {
            co_await m_socket.async_read_some(ba::buffer(m_buf), ba::use_awaitable);

            auto msg_hdr = common::message_header{};
            auto result =
                details::request_factory(msg_hdr.msg_type)
                    ->execute(std::span{m_buf.begin(), m_buf.begin() + common::k_header_size},
                              shared_from_this());
            if (result)
            {
                const std::string msg{"response"};
                co_await m_socket.async_write_some(ba::buffer(msg.data(), msg.size()),
                                                   ba::use_awaitable);
            }
        }
    }
    catch (std::exception& e)
    {
        spdlog::error("Exception: {}", e.what());
    }
}
}  // namespace etex