#include <string>
#include <arpa/inet.h>
/* has
 - inet_addr
 - inet_ntoa
 - htons
 */
#include <netinet/in.h>
/* has
 - in_addr_t
 */

namespace Naive {

struct addr_t {
    using store_t = std::string;
    store_t address;

    addr_t() = delete;
    addr_t(const store_t& in)
    : address{in}
    {}
    in_addr_t Get() { return inet_addr(address.c_str()); }
};

struct port_t {
    using value_t = int;
    // store the port number as host value
    value_t port_number;

    port_t() = delete;
    port_t(const value_t in)
    : port_number{in}
    {}
    value_t operator()() { return port_number; }
    uint16_t Get() { return htons(port_number); }
};

enum class error_t {
    SUCCESS,
    FAILED,
    ERROR
};

}