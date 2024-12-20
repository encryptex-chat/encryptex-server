#include "request_commands.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <cmath>
#include <memory>

#include "common.hpp"
#include "encryptex_server.hpp"
#include "id_generator.hpp"

namespace etex::details
{

request_command::~request_command() = default;

errorneous_request::~errorneous_request() = default;

auto errorneous_request::process(const common::message& msg, server& serv)
    -> ba::awaitable<std::expected<common::message, common::error_type>>
{
    spdlog::error("Errorneous request happened");
    co_return std::unexpected{common::error_type::unknown};
}

connection_to_server_request::~connection_to_server_request() = default;

auto connection_to_server_request::process(const common::message& msg, server& serv)
    -> ba::awaitable<std::expected<common::message, common::error_type>>
{
    if (auto found = serv.find_client(msg.hdr.src_id); found.has_value())
    {
        auto conn = found.value();
        if (!conn->is_registered())
        {
            conn->set_registered(true);
            conn->set_full_id({.id = id_generator::get_id(),
                               .ip = conn->socket().remote_endpoint().address().to_string()});
            spdlog::info("Client {} registered", conn->id());
            co_return common::message{
                .hdr  = {.msg_type = common::message_type::connection_to_server_response,
                         .src_id   = conn->id(),
                         .dst_id   = 0},
                .data = {}};
        }
    }
    else
    {
        co_return std::unexpected{found.error()};
    }
    co_return std::unexpected{common::error_type::unknown};
};

connection_to_user_request::~connection_to_user_request() = default;

auto connection_to_user_request::process(const common::message& msg, server& serv)
    -> ba::awaitable<std::expected<common::message, common::error_type>>
{
    if (auto found = serv.find_client(msg.hdr.dst_id); found.has_value())
    {
        auto conn = found.value();
        if (conn->is_registered())
        {
            spdlog::info("Client {} request to connect to client {}", msg.hdr.src_id,
                         msg.hdr.dst_id);
            common::message msg_to_client{msg};
            msg_to_client.hdr.msg_type = common::message_type::user_to_user_request;
            co_await boost::asio::async_write(conn->socket(),
                                              ba::buffer(&msg_to_client, common::k_message_size),
                                              ba::use_awaitable);
        }
    }
    co_return common::message{};
    // co_return std::unexpected{common::error_type::unknown};
};

user_to_user_response::~user_to_user_response() = default;

auto user_to_user_response::process(const common::message& msg, server& serv)
    -> ba::awaitable<std::expected<common::message, common::error_type>>
{
    if (auto found = serv.find_client(msg.hdr.dst_id); found.has_value())
    {
        auto conn = found.value();
        if (conn->is_registered())
        {
            spdlog::info("Client {} response to connect to client", msg.hdr.src_id, msg.hdr.dst_id);
            common::message msg_response{
                .hdr = {
                    .msg_type = common::message_type::connection_to_user_response,
                    .src_id   = msg.hdr.src_id,
                    .dst_id   = msg.hdr.dst_id,
                }};
            co_await boost::asio::async_write(conn->socket(),
                                              ba::buffer(&msg_response, common::k_message_size),
                                              ba::use_awaitable);
        }
    }
    co_return common::message{};
}

data_transfer_command::~data_transfer_command() = default;

auto data_transfer_command::process(const common::message& msg, server& serv)
    -> ba::awaitable<std::expected<common::message, common::error_type>>
{
    if (auto found = serv.find_client(msg.hdr.dst_id); found.has_value())
    {
        auto conn = found.value();
        co_await boost::asio::async_write(conn->socket(), ba::buffer(&msg, common::k_message_size),
                                          ba::use_awaitable);
    }
    co_return common::message{};
}

auto request_factory(etex::common::message_type msg_type) -> std::unique_ptr<request_command>
{
    using namespace etex::common;
    switch (msg_type)
    {
            // clang-format off
        case message_type::connection_to_server_request:
            return std::make_unique<connection_to_server_request>();
        case message_type::connection_to_user_request:
            return std::make_unique<connection_to_user_request>();
        case message_type::data_transfer:
            return std::make_unique<data_transfer_command>();
        default: return std::make_unique<errorneous_request>(); break;
            // clang-format on
    }
}
}  // namespace etex::details
