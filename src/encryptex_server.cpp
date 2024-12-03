#include "encryptex_server.hpp"

namespace etex
{
server::server(ba::io_context& io) : m_io{io}, m_acceptor{m_io}
{
    ba::ip::tcp::resolver resolver(io);
    ba::ip::tcp::endpoint ep = *(resolver.resolve(ba::ip::tcp::resolver::query{
        common::k_server_addr, std::to_string(common::k_server_port)}));

    m_acceptor = ba::ip::tcp::acceptor{m_io, ep};
}

void server::run()
{
    try
    {
        ba::co_spawn(m_io, start_accept(), ba::detached);
        spdlog::info("Encryptex server started on address {}:{}",
                     m_acceptor.local_endpoint().address().to_string(),
                     m_acceptor.local_endpoint().port());
        m_io.run();
    }
    catch (std::exception& ex)
    {
        spdlog::info("Excetion: {}", ex.what());
    }
}

auto server::start_accept() -> ba::awaitable<void>
{
    for (;;)
    {
        ba::ip::tcp::socket socket = co_await m_acceptor.async_accept(ba::use_awaitable);
        auto conn                  = std::make_shared<connection>(std::move(socket));
        m_connections.insert(conn);
        ba::co_spawn(m_io, conn->start(), ba::detached);
    }
}

}  // namespace etex