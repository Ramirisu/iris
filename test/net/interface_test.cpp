#include <thirdparty/test.hpp>

#include <iris/net/interface.hpp>

using namespace iris::net;

TEST_SUITE_BEGIN("net/interface");

TEST_CASE("get_interface()")
{
    auto interfaces = get_interface();
    CHECK(!interfaces.empty());
    for (auto& ifa : interfaces) {
        CHECK(!ifa.name.empty());
        CHECK_GT(ifa.max_transmission_unit, 0);
    }
}

TEST_SUITE_END();
