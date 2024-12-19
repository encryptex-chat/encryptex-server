#include "request_commands.hpp"

#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cmath>

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

            // TODO: add process for respone of user_to_user request
            co_return common::message{
                .hdr{.msg_type = common::message_type::connection_to_user_response,
                     .src_id   = msg.hdr.src_id,
                     .dst_id   = msg.hdr.dst_id},
                .data = {}};
        }
        else
        {
            co_return common::message{
                .hdr  = {.msg_type = common::message_type::connection_to_user_bad_response,
                         .src_id   = msg.hdr.src_id,
                         .dst_id   = msg.hdr.dst_id},
                .data = {}};
        }
    }
    co_return std::unexpected{common::error_type::unknown};
};

auto request_factory(etex::common::message_type msg_type) -> std::unique_ptr<request_command>
{
    using namespace etex::common;
    switch (msg_type)
    {
        case message_type::connection_to_server_request:
            return std::make_unique<connection_to_server_request>();
            break;
        case message_type::connection_to_user_request:
            return std::make_unique<connection_to_user_request>();
            break;
        default: return std::make_unique<errorneous_request>(); break;
    }
}
}  // namespace etex::details
