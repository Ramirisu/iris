#include <thirdparty/test.hpp>

#include <iris/lazy.hpp>

using namespace iris;

TEST_SUITE_BEGIN("lazy");

lazy<> generate_void(int& value)
{
    value = 100;
    co_return;
}

TEST_CASE("lazy<>")
{
    int value = 0;
    auto task = generate_void(value);
    CHECK_EQ(value, 0);
    task.sync_wait();
    CHECK_EQ(value, 100);
}

lazy<int> generate_int(int& value)
{
    value = 100;
    co_return value + 1000;
}

TEST_CASE("lazy<int>")
{
    int value = 0;
    auto task = generate_int(value);
    CHECK_EQ(value, 0);
    auto result = task.sync_wait();
    CHECK_EQ(value, 100);
    CHECK_EQ(result, 1100);
}

lazy<int> nested_generate_int(int& inner, int& outer)
{
    outer = 200;
    co_return outer + co_await generate_int(inner);
}

TEST_CASE("nested lazy<int>")
{
    int inner = 0;
    int outer = 0;
    auto task = nested_generate_int(inner, outer);
    CHECK_EQ(inner, 0);
    CHECK_EQ(outer, 0);
    auto result = task.sync_wait();
    CHECK_EQ(inner, 100);
    CHECK_EQ(outer, 200);
    CHECK_EQ(result, 1300);
}

TEST_SUITE_END();
