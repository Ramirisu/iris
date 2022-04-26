#include <thirdparty/test.hpp>

#include <iris/expected.hpp>
#include <iris/ranges/view/unwrap_view.hpp>

using namespace iris;

TEST_SUITE_BEGIN("unwrap_view");

TEST_CASE("expected")
{
    static const expected<int, int> input[] { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("expected with error")
{
    static const expected<int, int> input[] { 0, 1, 2, 3, iris::unexpected(4) };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_THROWS_AS(IRIS_UNUSED(*curr++), bad_expected_access<int>);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("optional")
{
    static const std::optional<int> input[] { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("optional with error")
{
    static const std::optional<int> input[] { 0, 1, 2, 3, std::nullopt };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_THROWS_AS(IRIS_UNUSED(*curr++), std::bad_optional_access);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
