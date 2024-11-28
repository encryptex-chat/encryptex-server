#include "request_commands.hpp"

#include "connection.hpp"

namespace etex::details
{

request_command::~request_command() = default;

auto request_command::parse_header(std::span<uint8_t> hdr)
    -> std::expected<common::message_header, common::error_type>
{
    return std::unexpected{common::error_type::unknown};
};

errorneous_request::~errorneous_request() = default;

auto errorneous_request::execute(std::span<uint8_t> hdr, std::shared_ptr<connection> conn) -> bool
{
    spdlog::error("Errorneous request happened");
    return false;
}

connection_to_server_request::~connection_to_server_request() = default;

auto connection_to_server_request::execute(std::span<uint8_t> hdr,
                                           std::shared_ptr<connection> conn) -> bool
{
    if (!conn->is_registered())
    {
        auto msg_h = parse_header(hdr);
        if (auto msg_h = parse_header(hdr); msg_h.has_value())
        {
            conn->set_registered(true);
            conn->set_id(msg_h.value().src_id);
            spdlog::info("Client {} registered", msg_h.value().src_id);
            return true;
        }
    }
    return false;
};

auto connection_to_server_request::parse_header(std::span<uint8_t> hdr)
    -> std::expected<common::message_header, common::error_type>
{
    if (hdr.size() != common::k_header_size)
    {
        return std::unexpected{common::error_type::invalid_size};
    }
    common::message_header msg_h;
    msg_h.msg_type = static_cast<common::message_type>(hdr.front());
    std::array<uint8_t, 8> bytes;
    std::copy(hdr.begin() + 1, hdr.begin() + 9, bytes.begin());
    std::ranges::reverse(bytes);
    msg_h.src_id = std::bit_cast<uint64_t>(bytes);

    return msg_h;
}

connection_to_user_request::~connection_to_user_request() = default;
auto connection_to_user_request::execute(std::span<uint8_t> hdr,
                                         std::shared_ptr<connection> conn) -> bool
{
    return false;
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