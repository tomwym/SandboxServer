#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>

/**
 * 1. int socket(domain, type, protocol);
 * 2. int bind(int socket, const struct sockaddr *address, socklen_t address_len);
 * 3. int listen(int socket, int backlog);
 *
*/
int main() {
    const int server_fd{socket(AF_INET, SOCK_STREAM, 0)};
    if (server_fd <= 0) {
        std::cerr << "Cannot create socket.\n";
        return -1;
    }

    const int PORT = 8080;
    struct sockaddr_in address{
        AF_INET, htons(PORT), inet_addr("127.0.0.1"), {'\0'}
    };

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Cannot bind to server.\n";
        close(server_fd);
        return -1;
    } else {
        std::cout << "Bound to: " << inet_ntoa(address.sin_addr) << '\n';
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Error in listen.\n";
        return -1;
    }

    int new_socket = 0;
    long val_read = 0;
    int addrlen{sizeof(address)};
    std::string hello_message{"Hello from server.\n"};
    while(1) {
        std::cout << "\n++++++++ Waiting for new connection ++++++++\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Error in accept...\n";
            close(new_socket);
            close(server_fd);
            return -1;
        }
        std::cout << "Accepted by server.\n";

        const int BUFFER_SIZE{3000};
        std::string buffer(BUFFER_SIZE, 0);

        val_read = read(new_socket, (void*)buffer.c_str(), BUFFER_SIZE);
        std::cout << buffer << '\n';
        write(new_socket, hello_message.c_str(), hello_message.length() );
        std::cout << "------------------Hello message sent-------------------\n";
        close(new_socket);
    }
    close(server_fd);
    return 0;
}