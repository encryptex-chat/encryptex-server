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
    virtual auto execute(common::message_header msg_hdr,
                         std::shared_ptr<connection> conn) -> bool = 0;
    virtual ~request_command();
};

class errorneous_request final : public request_command
{
    public:
    auto execute(common::message_header msg_hdr, std::shared_ptr<connection> conn) -> bool override;
    ~errorneous_request() override;
};

class connection_to_server_request final : public request_command
{
    public:
    auto execute(common::message_header msg_hdr, std::shared_ptr<connection> conn) -> bool override;
    ~connection_to_server_request() override;
};

class connection_to_user_request final : public request_command
{
    public:
    auto execute(common::message_header msg_hdr, std::shared_ptr<connection> conn) -> bool override;
    ~connection_to_user_request() override;
};

auto request_factory(etex::common::message_type msg_type) -> std::unique_ptr<request_command>;

}  // namespace etex::details
