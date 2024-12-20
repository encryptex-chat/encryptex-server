#pragma once
#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <expected>
#include <memory>

#include "common.hpp"

namespace etex
{
class server;
}
namespace etex::details
{

namespace ba = boost::asio;

class request_command
{
    public:
    virtual auto process(const common::message& msg, server& serv)
        -> ba::awaitable<std::expected<common::message, common::error_type>> = 0;
    virtual ~request_command();
};

class errorneous_request final : public request_command
{
    public:
    auto process(const common::message& msg, server& serv)
        -> ba::awaitable<std::expected<common::message, common::error_type>> override;
    ~errorneous_request() override;
};

class connection_to_server_request final : public request_command
{
    public:
    auto process(const common::message& msg, server& serv)
        -> ba::awaitable<std::expected<common::message, common::error_type>> override;
    ~connection_to_server_request() override;
};

class connection_to_user_request final : public request_command
{
    public:
    auto process(const common::message& msg, server& serv)
        -> ba::awaitable<std::expected<common::message, common::error_type>> override;
    ~connection_to_user_request() override;
};

class user_to_user_response final : public request_command
{
    public:
    auto process(const common::message& msg, server& serv)
        -> ba::awaitable<std::expected<common::message, common::error_type>> override;
    ~user_to_user_response() override;
};

class data_transfer_command final : public request_command
{
    public:
    auto process(const common::message& msg, server& serv)
        -> ba::awaitable<std::expected<common::message, common::error_type>> override;
    ~data_transfer_command() override;
};

auto request_factory(etex::common::message_type msg_type) -> std::unique_ptr<request_command>;

}  // namespace etex::details
