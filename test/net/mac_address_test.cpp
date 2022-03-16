#include <thirdparty/test.hpp>

#include <iris/net/mac_address.hpp>>

using namespace iris::net;

TEST_SUITE_BEGIN("net/mac_address");

TEST_CASE("mac_address")
{
    CHECK_EQ(mac_address().to_string(), "00:00:00:00:00:00");
    CHECK_EQ(mac_address(0x00, 0x0a, 0x01, 0x0b, 0x02, 0x0c).to_string(),
             "00:0a:01:0b:02:0c");
}

TEST_SUITE_END();
