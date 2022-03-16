#include "test.hpp"

#include <iris/system.hpp>

using namespace iris;

TEST_SUITE_BEGIN("system");

TEST_CASE("get_host_name()")
{
    auto hostname = get_host_name();
    CHECK(hostname);
    CHECK(hostname.value().size() > 0);
}

TEST_SUITE_END();
