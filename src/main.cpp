#include <iostream>

#include "encryptex_server.hpp"

int main()
{
    boost::asio::io_context io;
    etex::server server(io, 12345);
    server.run();
}
