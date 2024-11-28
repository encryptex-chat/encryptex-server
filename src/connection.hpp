#pragma once

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <chrono>
#include <istream>
#include <memory>
#include <string>
#include <thread>

#include "message.hpp"
#include "request_commands.hpp"

namespace etex
{
namespace ba = boost::asio;
class connection : public std::enable_shared_from_this<connection>
{
    public:
    using conn_ptr = std::shared_ptr<connection>;

    explicit connection(boost::asio::ip::tcp::socket&& socket) : m_socket{std::move(socket)} {}

    boost::asio::ip::tcp::socket& socket() { return m_socket; };

    [[nodiscard]] std::string name() const { return m_id.second; };

    [[nodiscard]] bool is_registered() const { return m_is_registered; }

    auto set_registered(bool status) { m_is_registered = status; }

    auto set_id(uint64_t id) { m_id.first = id; }

    [[nodiscard]] common::message_header parse() const
    {
        common::message_header hdr;
        hdr.msg_type = static_cast<common::message_type>(m_buf[0] - '0');
        std::array<char, 8> bytes;
        std::copy(m_buf.begin() + 1, m_buf.begin() + 9, bytes.begin());
        std::ranges::reverse(bytes);
        hdr.src_id = std::bit_cast<uint64_t>(bytes);
        if (hdr.msg_type != common::message_type::connection_to_server_request)
        {
            std::copy(m_buf.begin() + 9, m_buf.begin() + 17, bytes.begin());
            std::ranges::reverse(bytes);
            hdr.dst_id = std::bit_cast<uint64_t>(bytes);
        }

        return hdr;
    }

    void process_connection_request(uint64_t id)
    {
        m_id.first = id;
        std::stringstream name;
        name << m_socket.remote_endpoint();
        m_id.second = name.str().substr(0, name.str().find_last_of(":"));
    }

    auto start() -> boost::asio::awaitable<void>
    {
        try
        {
            for (;;)
            {
                co_await m_socket.async_read_some(ba::buffer(m_buf), ba::use_awaitable);

                auto msg_hdr = parse();
                auto result =
                    details::request_factory(msg_hdr.msg_type)
                        ->execute(std::span{m_buf.begin(), m_buf.begin() + common::k_header_size},
                                  shared_from_this());
                // if (result)
                // {
                /*generate response*/
                // }
            }
        } catch (std::exception& e)
        {
            std::print("exception {}", e.what());
        }
    }

    private:
    ba::ip::tcp::socket m_socket;
    std::array<uint8_t, 65536> m_buf;
    std::pair<uint64_t, std::string> m_id;
    bool m_is_registered{false};
    // will be defined structure for user
};

}  // namespace etex