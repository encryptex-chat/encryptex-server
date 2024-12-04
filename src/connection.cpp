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
        for (;;)
        {
            co_await m_socket.async_read_some(ba::buffer(m_buf), ba::use_awaitable);

            std::replace(m_buf.begin(), m_buf.end(), '\n', (char)0);
            std::replace(m_buf.begin(), m_buf.end(), '\r', (char)0);
            auto msg_hdr =
                common::deserialize_msg_hdr(m_buf | std::views::take(common::k_header_size));
            auto result =
                details::request_factory(msg_hdr.msg_type)->execute(msg_hdr, shared_from_this());
            if (result)
            {
                common::message_header resp_hdr = {
                    .msg_type = common::message_type::connection_to_server_response,
                    .src_id   = id(),
                    .dst_id   = 0};
                auto msg = common::serialize_msg_hdr(resp_hdr);
                co_await m_socket.async_send(ba::buffer(msg), ba::use_awaitable);
                // co_await m_socket.async_write_some(ba::buffer(msg), ba::use_awaitable);
            }
        }
    }
    catch (std::exception& e)
    {
        spdlog::error("Exception: {}", e.what());
    }
}
}  // namespace etex