#pragma once
#include <spdlog/spdlog.h>

#include <expected>
#include <memory>
#include <span>

// #include "connection.hpp"
#include "message.hpp"

namespace etex
{
class connection;
}
namespace etex::details
{

class request_command
{
    public:
    virtual auto execute(std::span<uint8_t> hdr, std::shared_ptr<connection> conn) -> bool = 0;
    virtual ~request_command();

    protected:
    virtual auto parse_header(std::span<uint8_t> hdr)
        -> std::expected<common::message_header, common::error_type>;
};

class errorneous_request : public request_command
{
    public:
    auto execute(std::span<uint8_t> hdr, std::shared_ptr<connection> conn) -> bool override;
    ~errorneous_request() override;
};

class connection_to_server_request final : public request_command
{
    public:
    auto execute(std::span<uint8_t> hdr, std::shared_ptr<connection> conn) -> bool override;
    ~connection_to_server_request() override;
    auto parse_header(std::span<uint8_t> hdr)
        -> std::expected<common::message_header, common::error_type> override;
};

class connection_to_user_request : public request_command
{
    public:
    auto execute(std::span<uint8_t> hdr, std::shared_ptr<connection> conn) -> bool override;
    ~connection_to_user_request() override;
};

auto request_factory(etex::common::message_type msg_type) -> std::unique_ptr<request_command>;

}  // namespace etex::details
