#include "request_commands.hpp"

#include "connection.hpp"
#include "id_generator.hpp"

namespace etex::details
{

request_command::~request_command() = default;

errorneous_request::~errorneous_request() = default;

auto errorneous_request::execute(common::message_header msg_hdr,
                                 std::shared_ptr<connection> conn) -> bool
{
    spdlog::error("Errorneous request happened");
    return false;
}

connection_to_server_request::~connection_to_server_request() = default;

auto connection_to_server_request::execute(common::message_header msg_hdr,
                                           std::shared_ptr<connection> conn) -> bool
{
    if (!conn->is_registered())
    {
        conn->set_registered(true);
        conn->set_id(id_generator::get_id());
        spdlog::info("Client {} registered", conn->id());
        return true;
    }
    return false;
};

connection_to_user_request::~connection_to_user_request() = default;

auto connection_to_user_request::execute(common::message_header msg_hdr,
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