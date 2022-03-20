#include <thirdparty/test.hpp>

#include <iris/coroutine.hpp>
#include <iris/lazy.hpp>

using namespace iris;

TEST_SUITE_BEGIN("coroutine");

TEST_CASE("simple_awaitable")
{
    static_assert(simple_awaitable<std::suspend_always>);
    static_assert(!simple_awaitable<lazy<>>);
}

TEST_CASE("awaitable")
{
    static_assert(awaitable<std::suspend_always>);
    static_assert(awaitable<lazy<>>);
}

TEST_CASE("awaitable_result")
{
    static_assert(
        std::is_same_v<awaitable_result_t<std::suspend_always>, void>);
    static_assert(std::is_same_v<awaitable_result_t<lazy<>>, void>);
    static_assert(std::is_same_v<awaitable_result_t<lazy<int>>, int>);
}

TEST_SUITE_END();
