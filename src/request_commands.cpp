#include "request_commands.hpp"

#include "connection.hpp"
#include "id_generator.hpp"

namespace etex::details
{

request_command::~request_command() = default;

errorneous_request::~errorneous_request() = default;

auto errorneous_request::execute(const common::message& msg, std::shared_ptr<connection> conn)
    -> std::expected<common::message, common::error_type>
{
    spdlog::error("Errorneous request happened");
    return std::unexpected{common::error_type::unknown};
}

connection_to_server_request::~connection_to_server_request() = default;

auto connection_to_server_request::execute(const common::message& msg,
                                           std::shared_ptr<connection> conn)
    -> std::expected<common::message, common::error_type>
{
    if (!conn->is_registered())
    {
        conn->set_registered(true);
        conn->set_id(id_generator::get_id());
        spdlog::info("Client {} registered", conn->id());
        return common::message{
            .hdr  = {.msg_type = common::message_type::connection_to_server_response,
                     .src_id   = conn->id(),
                     .dst_id   = 0},
            .data = {}};
    }
    return std::unexpected{common::error_type::unknown};
};

connection_to_user_request::~connection_to_user_request() = default;

auto connection_to_user_request::execute(const common::message& msg,
                                         std::shared_ptr<connection> conn)
    -> std::expected<common::message, common::error_type>
{
    return std::unexpected{common::error_type::unknown};
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