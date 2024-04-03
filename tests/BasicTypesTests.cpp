#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "BasicTypes.h"
#include "NaiveServerComponents.h"

#include <iostream>

using namespace ::testing;

const in_addr_t INVALID_ADDR {in_addr_t(-1)};

TEST(BasicTypesTests, AddrTTests)
{
    {
        Naive::addr_t address{""};
        EXPECT_TRUE(address.Get() == INVALID_ADDR);
    }
    {
        Naive::addr_t address{"www.example.com"};
        EXPECT_TRUE(address.Get() == INVALID_ADDR);
    }
    {
        Naive::addr_t address{"127.0.0.1"};
        /* Note:
         - The conversion simply takes each 'number' in the string and turns it into a byte.
         - The bytes are ordered in network order (most significant byte left to right).
         - 127.0.0.1 == 7F 00 00 01
         - reorder
         - 7F000001
         */
        EXPECT_TRUE(address.Get() == 0x0100007F);
    }
}

TEST(BasicTypesTests, PortTTests)
{
    {
        Naive::port_t port{0};
        EXPECT_TRUE(port() == 0);
        std::cout << port.Get() << '\n';
    }
    {
        Naive::port_t port{80};
        EXPECT_TRUE(port() == 80);
        /* Note:
         - This conversion takes some int and converts to unsigned int (16 bits) in network order.
         - For int 80 == 0x0050
         - In network order this would be equivalent to 0x5000
         */
        EXPECT_TRUE(port.Get() == 0x5000);
    }
}