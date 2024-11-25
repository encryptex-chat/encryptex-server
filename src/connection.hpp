#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <istream>
#include <memory>
#include <print>
#include <thread>

namespace etex
{
namespace ba = boost::asio;
class connection : public std::enable_shared_from_this<connection>
{
    public:
    using ptr = std::shared_ptr<connection>;
    static std::shared_ptr<connection> create_connection(boost::asio::ip::tcp::socket&& socket)
    {
        return ptr(new connection(std::move(socket)));
    }

    boost::asio::ip::tcp::socket& socket() { return m_socket; };

    [[nodiscard]] std::string name() const { return m_name; };

    void start() { read(); }

    void read()
    {
        ba::async_read_until(
            m_socket, m_buffer, "\n",
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes) {
                std::stringstream msg;
                msg << std::istream(&(self->m_buffer)).rdbuf();
                std::print("{}\n", msg.str());
                self->read();
            });
    }

    private:
    explicit connection(boost::asio::ip::tcp::socket&& socket) :
        m_socket{std::move(socket)}, m_name{}
    {
        std::stringstream name;
        name << m_socket.remote_endpoint();
        m_name = name.str();
    }
    ba::ip::tcp::socket m_socket;
    std::string m_name;
    ba::streambuf m_buffer{1000};
    // will be defined structure for user
};

}  // namespace etex