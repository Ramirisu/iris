#include <thirdparty/test.hpp>

#include <iris/ranges/view/enumerate_view.hpp>

using namespace iris;

TEST_SUITE_BEGIN("enumerate_view");

TEST_CASE("index & value")
{
    using type = std::pair<std::size_t, const int&>;

    static const int input[] = { 1, 2, 4, 8 };
    auto view = views::enumerate(input);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, type { 0, input[0] });
    CHECK_EQ(*curr++, type { 1, input[1] });
    CHECK_EQ(*curr++, type { 2, input[2] });
    CHECK_EQ(*curr++, type { 3, input[3] });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("for loop")
{
    static const int input[] = { 1, 2, 4, 8 };
    std::size_t i = 0;

    for (auto&& [index, value] : views::enumerate(input)) {
        static_assert(std::is_lvalue_reference_v<decltype(value)>);
        CHECK_EQ(index, i);
        CHECK_EQ(value, input[i]);
        ++i;
    }
}

TEST_SUITE_END();
