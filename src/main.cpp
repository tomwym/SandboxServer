#include "NaiveServerComponents.h"
#include "BasicTypes.h"

#include <iostream>

auto main() -> int {
    Naive::addr_t address{"0.0.0.0"};
    Naive::port_t port{8080};
    Naive::NaiveServerComponents server(address, port);
    if (server.Bind() == Naive::error_t::FAILED)
        return 0;
    if (server.Listen(2) == Naive::error_t::FAILED)
        return 0;
    int number = 0;

    while (true) {
        auto sock = server.Accept();
        if (sock) {
            std::cout << "server side client connection: " << static_cast<Naive::socket_t>(*sock) << '\n';
            auto s = server.Read(*sock);
            char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

            server.Write(*sock, hello);
            number++;
        }
    }

    return 0;
}
