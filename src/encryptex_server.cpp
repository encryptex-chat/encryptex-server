#include "encryptex_server.hpp"

#include <algorithm>
#include <cmath>
#include <magic_enum/magic_enum.hpp>

#include "common.hpp"
#include "connection.hpp"
#include "request_commands.hpp"

namespace etex
{
server::server(ba::io_context& io) : m_io{io}, m_acceptor{m_io}
{
    ba::ip::tcp::resolver resolver(io);
    ba::ip::tcp::endpoint ep = *(resolver.resolve(ba::ip::tcp::resolver::query{
        common::k_server_addr, std::to_string(common::k_server_port)}));

    m_acceptor = ba::ip::tcp::acceptor{m_io, ep};
}

void server::run()
{
    try
    {
        ba::co_spawn(m_io, start_accept(), ba::detached);
        spdlog::info("Encryptex server started on address {}:{}",
                     m_acceptor.local_endpoint().address().to_string(),
                     m_acceptor.local_endpoint().port());
        m_io.run();
    }
    catch (std::exception& ex)
    {
        spdlog::info("Excetion: {}", ex.what());
    }
}

auto server::find_client(uint64_t client_id)
    -> std::expected<details::client_identificator, common::error_type>
{
    if (auto found_id =
            std::ranges::find_if(m_connections, [client_id](const std::shared_ptr<connection>& conn)
                                 { return client_id == conn->id(); });
        found_id != m_connections.end())
    {
        return (*found_id)->full_id();
    }
    return std::unexpected{common::error_type::client_not_found};
}

auto server::message_handler(const common::message& msg, std::shared_ptr<connection> conn)
    -> std::expected<common::message, common::error_type>
{
    spdlog::info("Request {}", magic_enum::enum_name(msg.hdr.msg_type));
    auto request_handler = details::request_factory(msg.hdr.msg_type);
    if (msg.hdr.msg_type == common::message_type::user_to_user_request)
    {
        if (auto client = find_client(msg.hdr.dst_id); client.has_value())
        {
            request_handler->set_user(client->id);
        }
        else
        {
            spdlog::error("Error: {}", magic_enum::enum_name(client.error()));
        }
    }
    return request_handler->execute(msg, conn);
}

auto server::start_accept() -> ba::awaitable<void>
{
    for (;;)
    {
        ba::ip::tcp::socket socket = co_await m_acceptor.async_accept(ba::use_awaitable);
        auto conn                  = std::make_shared<connection>(std::move(socket));
        std::weak_ptr<connection> weak_conn{conn};
        auto msg_h = [this, weak_conn](const common::message& msg)
            -> std::expected<common::message, common::error_type>
        {
            return message_handler(msg, weak_conn.lock());
        };

        m_connections.insert(conn);
        ba::co_spawn(m_io, conn->start(std::move(msg_h)), ba::detached);
    }
}

}  // namespace etex
