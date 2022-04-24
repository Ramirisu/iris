#include <thirdparty/test.hpp>

#include <iris/ranges/algorithm/iota.hpp>

using namespace iris;

TEST_SUITE_BEGIN("iota");

TEST_CASE("iota")
{
    std::vector<int> v0(10);
    const auto result = ranges::iota(v0, 10);
    CHECK_EQ(result.out, std::ranges::end(v0));
    CHECK_EQ(result.value, 20);
    CHECK(std::ranges::equal(v0, std::views::iota(10, 20)));
}

TEST_SUITE_END();
