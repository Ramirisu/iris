#include <thirdparty/test.hpp>

#include <iris/net/ipv4_address.hpp>
#include <iris/net/ipv6_address.hpp>

using namespace iris::net;

TEST_SUITE_BEGIN("net/ip_address");

TEST_CASE("ipv4_address")
{
    CHECK_EQ(ipv4_address().to_string(), "0.0.0.0");
    CHECK_EQ(ipv4_address(127, 0, 0, 1).to_string(), "127.0.0.1");
    CHECK_EQ(ipv4_address(0x7f000001).to_string(), "127.0.0.1");
}

TEST_CASE("ipv6_address")
{
    CHECK_EQ(ipv6_address().to_string(), "::");
    CHECK_EQ(ipv6_address(0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                          0x0000, 0x0001)
                 .to_string(),
             "::1");
    CHECK_EQ(ipv6_address(0x2001, 0x0db8, 0x02de, 0x0000, 0x0000, 0x0000,
                          0x0000, 0x0e13)
                 .to_string(),
             "2001:db8:2de::e13");
    CHECK_EQ(ipv6_address(0x2001, 0x0000, 0x0000, 0x25de, 0x25de, 0x0000,
                          0x0000, 0xcade)
                 .to_string(),
             "2001::25de:25de:0:0:cade");
}

TEST_SUITE_END();
