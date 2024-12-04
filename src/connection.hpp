#pragma once

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <chrono>
#include <istream>
#include <memory>
#include <string>
#include <thread>

#include "common.hpp"
#include "request_commands.hpp"

namespace etex
{
namespace ba = boost::asio;
class connection : public std::enable_shared_from_this<connection>
{
    public:
    using conn_ptr = std::shared_ptr<connection>;

    explicit connection(boost::asio::ip::tcp::socket&& socket);

    boost::asio::ip::tcp::socket& socket() { return m_socket; };

    [[nodiscard]] auto name() const { return m_id.second; };

    [[nodiscard]] auto is_registered() const { return m_is_registered; }

    [[nodiscard]] auto id() const { return m_id.first; }

    auto set_registered(bool status) { m_is_registered = status; }

    auto set_id(uint64_t id) { m_id.first = id; }

    auto start() -> boost::asio::awaitable<void>;

    private:
    ba::ip::tcp::socket m_socket;
    std::array<uint8_t, 65536> m_buf;
    std::pair<uint64_t, std::string> m_id;
    bool m_is_registered{false};
};

}  // namespace etex