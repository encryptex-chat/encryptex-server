#pragma once

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <string>

#include "common.hpp"
#include "request_commands.hpp"

namespace etex
{
namespace details
{
    struct client_identificator
    {
        // NOLINTBEGIN
        uint64_t id;
        std::string ip;
        // NOLINTEND
    };
}  // namespace details
using msg_handler_t =
    std::function<std::expected<common::message, common::error_type>(const common::message&)>;
namespace ba = boost::asio;

class connection : public std::enable_shared_from_this<connection>
{
    public:
    explicit connection(boost::asio::ip::tcp::socket&& socket);

    boost::asio::ip::tcp::socket& socket() { return m_socket; };

    [[nodiscard]] auto ip() const { return m_id.ip; };

    auto set_ip(const std::string& ip) { m_id.ip = ip; }

    [[nodiscard]] auto is_registered() const { return m_is_registered; }

    [[nodiscard]] auto id() const { return m_id.id; }

    auto set_full_id(details::client_identificator c_id) { m_id = c_id; }
    [[nodiscard]] auto full_id() const { return m_id; }

    auto set_registered(bool status) { m_is_registered = status; }

    auto set_id(uint64_t id) { m_id.id = id; }

    auto start(msg_handler_t&& income_message_handler) -> boost::asio::awaitable<void>;

    private:
    ba::ip::tcp::socket m_socket;
    details::client_identificator m_id;
    bool m_is_registered{false};
    msg_handler_t m_on_message;
};

}  // namespace etex
