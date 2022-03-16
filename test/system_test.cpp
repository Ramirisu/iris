#include "test.hpp"

#include <iris/system.hpp>

using namespace iris;

TEST_SUITE_BEGIN("system");

TEST_CASE("get_host_name()")
{
    auto name = get_host_name();
    CHECK(name);
    CHECK(name.value().size() > 0);
}

TEST_CASE("get_user_name()")
{
    auto name = get_user_name();
    CHECK(name);
    CHECK(name.value().size() > 0);
}

TEST_SUITE_END();
