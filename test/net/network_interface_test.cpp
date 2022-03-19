#include <thirdparty/test.hpp>

#include <iris/net/network_interface.hpp>

using namespace iris::net;

TEST_SUITE_BEGIN("net/network_interface");

TEST_CASE("get_network_interface()")
{
    auto nis = get_network_interface();
    CHECK(!nis.empty());
    for (auto& ni : nis) {
        CHECK(!ni.name.empty());
        CHECK_GT(ni.unicast_addrs.size() + ni.multicast_addrs.size(), 0);
        CHECK_GT(ni.max_transmission_unit, 0);
    }
}

TEST_SUITE_END();
