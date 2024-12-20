#include "connection.hpp"

#include "common.hpp"

namespace etex
{
connection::connection(boost::asio::ip::tcp::socket&& socket) : m_socket{std::move(socket)} {}

auto connection::on_error(std::optional<std::string_view> exc_msg) -> void
{
    spdlog::error("Error occured: {} closing socket [ID: {} IP: {}]",
                  exc_msg.has_value() ? exc_msg.value() : "", m_id.id, m_id.ip);
    m_is_registered = false;
    m_is_expired    = true;
    m_socket.close();
}

auto connection::start(msg_handler_t&& income_message_handler) -> boost::asio::awaitable<void>
{
    m_on_message = std::move(income_message_handler);
    try
    {
        for (common::message msg;;)
        {
            boost::system::error_code ec;
            co_await m_socket.async_read_some(ba::buffer(&msg, common::k_message_size),
                                              ba::redirect_error(ba::use_awaitable, ec));
            if (ec)
            {
                on_error();
                break;
            }

            auto response = co_await m_on_message(msg);

            // TODO: remove
            if (response->hdr.msg_type != common::message_type::unknown_message_type)
            {
                co_await m_socket.async_write_some(ba::buffer(&response, common::k_message_size),
                                                   ba::redirect_error(ba::use_awaitable, ec));
                if (ec)
                {
                    on_error();
                    break;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        on_error(e.what());
    }
}
}  // namespace etex
