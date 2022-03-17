#include <thirdparty/test.hpp>

#include <iris/net/interface.hpp>

using namespace iris::net;

TEST_SUITE_BEGIN("net/interface");

TEST_CASE("get_interface()")
{
    auto interfaces = get_interface();
    CHECK(!interfaces.empty());
}

TEST_SUITE_END();
