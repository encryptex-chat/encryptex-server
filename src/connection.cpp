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
            // co_await m_socket.async_receive(ba::buffer(m_buf), ba::use_awaitable);
            co_await m_socket.async_read_some(ba::buffer(&msg, common::k_message_size),
                                              ba::use_awaitable);

            auto msg_hdr = msg.hdr;
            auto result =
                details::request_factory(msg.hdr.msg_type)->execute(msg.hdr, shared_from_this());
            if (result)
            {
                common::message resp = {
                    .hdr = {
                        .msg_type = common::message_type::connection_to_server_response,
                        .src_id   = id(),
                        .dst_id   = 0,

                    }};
                // co_await m_socket.async_send(ba::buffer(msg), ba::use_awaitable);
                co_await m_socket.async_write_some(ba::buffer(&resp, common::k_message_size),
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