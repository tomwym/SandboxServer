#include "NaiveServerComponents.h"
#include "BasicTypes.h"

#include <unistd.h> // close

#include <iostream>
#include <format>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string.h>

// resource: https://stackoverflow.com/questions/27014955/socket-connect-vs-bind

namespace Naive {

/* 
 - This class is only responsible for reading the contents into some stream.
 - This class is not responsible for opening or closing any sockets.  
 */
class NaiveServerComponentsReader {
    socket_t _connection;
public:
    NaiveServerComponentsReader() = delete;
    NaiveServerComponentsReader(const socket_t& connection)
    : _connection{connection}
    {
    }

    [[nodiscard]] auto ReadToMessageT(const std::size_t chunk_size) const -> message_t {
        std::ostringstream ss;
        ReadAsChunks(chunk_size, ss);
        message_t message{ss.str()};
        return message;
    }

    auto ReadToFile(const std::size_t chunk_size, std::filesystem::path& path) -> void {
        if (!std::filesystem::exists(path.parent_path()))
            std::filesystem::create_directory(path.parent_path());
        std::ofstream file{path, std::ios::trunc};
        ReadAsChunks(chunk_size, file);
    }

private:
    auto ReadAsChunks(const std::size_t chunk_size, std::ostream& ostream) const -> void {
        std::string buffer(chunk_size, 0);
        // std::string contents;
        ssize_t read_size {0};
        do {
            read_size = recv(_connection, (void*)(buffer.c_str()), chunk_size, MSG_DONTWAIT);
            if (read_size > 0) {
                // contents.append(buffer.begin(), buffer.begin()+read_size);
                ostream << std::string{buffer.begin(), buffer.begin() + read_size};
            }
        } while(read_size > 0);
    }
};

SelfClosingSocket::SelfClosingSocket(const socket_t& socket)
: _socket{socket}
{}
SelfClosingSocket::operator socket_t() const { return _socket; }
SelfClosingSocket::~SelfClosingSocket() {
    close(_socket);
}

/* Intent
 - This class should serve as a wrapper over the basic socket functions 
 */
class NaiveServerComponents::impl {
    addr_t _address;
    port_t _port;
    sockaddr_in _address_in;
    SelfClosingSocket _server_socket;

public:
    impl() = delete;
    impl(const addr_t& address, const port_t& port)
    : _address{address}
    , _port{port}
    , _address_in{AF_INET, _port.Get(), _address.Get(), {'\0'}}
    , _server_socket{socket(AF_INET, SOCK_STREAM, 0)}
    {
        const int enable = 1;
        int rest {setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int))};
        if (rest < 0)
            std:: cout << "error/.\n";
    }

    [[nodiscard]] auto Bind() -> error_t {
        const int bind_result {bind(_server_socket,
                               (struct sockaddr*)(&_address_in),
                               sizeof(_address_in))};
        if (bind_result < 0)
        {
            fprintf(stderr, "connect: %s\n", strerror(errno));
        }
        return SuccessOrFail(bind_result,
                             std::format("Call to bind at address {} SUCCESSFUL.\n", _address.address),
                             std::format("Call to bind at address {} has FAILED.\n", _address.address));
    }
    
    [[nodiscard]] auto Listen(const std::size_t& max_connections) -> error_t {
        const int listen_result {listen(_server_socket, max_connections)};
        return SuccessOrFail(listen_result,
                             std::format("Call to listen SUCCESSFUL.\n"),
                             std::format("Call to listen has FAILED.\n"));
    }

    [[nodiscard]] auto Accept() -> std::unique_ptr<SelfClosingSocket> {
        int address_in_length{sizeof(sockaddr_in)};
        const socket_t new_socket = accept(_server_socket,
                                           reinterpret_cast<sockaddr*>(&_address_in),
                                           reinterpret_cast<socklen_t*>(&address_in_length));
        if (new_socket < 0) {
            std::cerr << std::format("Call to accept has FAILED.\n");
            return nullptr;
        }
        else {
            std::cout << std::format("Call to accept SUCCSESFUL.\n");
            return std::make_unique<SelfClosingSocket>(new_socket);
        }
    }

    [[nodiscard]] auto Read(const socket_t& client) const -> message_t {
        NaiveServerComponentsReader reader{client};
        message_t message {reader.ReadToMessageT(10)};
        std::cout << message << '\n';
        return message;
    }

    auto Write(const socket_t& client, const message_t& message) const -> void {
        write(client, message.c_str(), message.length() );
    }


private:
    [[nodiscard]] auto SuccessOrFail(const int result, const std::string& success, const std::string& failed) -> error_t {
        if (result < 0) {
            std::cerr << failed;
            return error_t::FAILED;
        }
        else {
            std::cout << success;
            return error_t::SUCCESS;
        }
    }
};

NaiveServerComponents::NaiveServerComponents(const addr_t& address, const port_t& port)
: pImpl{std::make_unique<impl>(address, port)}
{
}

NaiveServerComponents::~NaiveServerComponents() = default;

auto NaiveServerComponents::Success() const -> error_t {
    return {};
}

auto NaiveServerComponents::Bind() const -> error_t {
    return pImpl->Bind();
}

auto NaiveServerComponents::Listen(const std::size_t max_connections) const -> error_t {
    return pImpl->Listen(max_connections);
}

auto NaiveServerComponents::Accept() -> std::unique_ptr<SelfClosingSocket> {
    return pImpl->Accept();
}

auto NaiveServerComponents::Read(const socket_t& client) const -> message_t {
    return pImpl->Read(client);
}

auto NaiveServerComponents::Write(const socket_t& client, const message_t& message) const -> void {
    return pImpl->Write(client, message);
}

}
