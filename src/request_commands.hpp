#pragma once
#include <spdlog/spdlog.h>

#include <expected>
#include <memory>
#include <span>

// #include "connection.hpp"
#include "common.hpp"

namespace etex
{
class connection;
}
namespace etex::details
{

class request_command
{
    public:
    virtual auto execute(const common::message& msg, std::shared_ptr<connection> conn)
        -> std::expected<common::message, common::error_type> = 0;
    virtual auto set_user(uint64_t user) -> void {}
    virtual ~request_command();
};

class errorneous_request final : public request_command
{
    public:
    auto execute(const common::message& msg, std::shared_ptr<connection> conn)
        -> std::expected<common::message, common::error_type> override;
    ~errorneous_request() override;
};

class connection_to_server_request final : public request_command
{
    public:
    auto execute(const common::message& msg, std::shared_ptr<connection> conn)
        -> std::expected<common::message, common::error_type> override;
    ~connection_to_server_request() override;
};

class connection_to_user_request final : public request_command
{
    public:
    auto execute(const common::message& msg, std::shared_ptr<connection> conn)
        -> std::expected<common::message, common::error_type> override;
    auto set_user(uint64_t user) -> void override { m_to_user = user; }
    ~connection_to_user_request() override;

    private:
    uint64_t m_to_user;
};

auto request_factory(etex::common::message_type msg_type) -> std::unique_ptr<request_command>;

}  // namespace etex::details
