#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>

int main() {

    const char *message = "Hello from client";

    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "Socket creation error.'\n";
        return -1;
    }

    const int PORT = 8080;
    struct sockaddr_in serv_addr{
        AF_INET, htons(PORT), 0, {'\0'}
    };

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/ address not supported.\n";
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection failed\n";
        close(sock);
        return -1;
    }

    send(sock, message, std::strlen(message), 0);
    std::cout << "\n"; // Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.

    const int BUFFER_SIZE{1024};
    std::string buffer(BUFFER_SIZE, 0);
    long valread = read(sock, (void*)buffer.c_str(), BUFFER_SIZE);
    std::cout << buffer << '\n';
    std::cout << "server side client connection: " << sock << '\n';

    close(sock);
    return 0;
}