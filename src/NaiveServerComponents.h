#pragma once

#include <memory>
#include <optional>

namespace Naive {

struct port_t;
struct addr_t;
enum class error_t;
class SelfClosingSocket;
using message_t = std::string;
using socket_t = int;

class SelfClosingSocket {
    socket_t _socket;
public:
    SelfClosingSocket() = delete;
    SelfClosingSocket(const socket_t& socket);
    ~SelfClosingSocket();
    operator socket_t() const;
};

class NaiveServerComponents {
public:
    NaiveServerComponents() = delete;
    NaiveServerComponents(const addr_t&, const port_t&);
    ~NaiveServerComponents();
    [[nodiscard]] auto Success() const -> error_t;
    [[nodiscard]] auto Bind() const -> error_t;
    [[nodiscard]] auto Listen(const std::size_t max_connections) const -> error_t;
    [[nodiscard]] auto Accept() -> std::unique_ptr<SelfClosingSocket>;
    [[nodiscard]] auto Read(const socket_t& client) const -> message_t;
                  auto Write(const socket_t& client, const message_t& message) const -> void;

private:
    class impl;
    std::unique_ptr<impl> pImpl;
};

}