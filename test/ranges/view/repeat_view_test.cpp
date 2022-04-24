#include <thirdparty/test.hpp>

#include <iris/ranges/view/repeat_view.hpp>

#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("repeat_view");

TEST_CASE("repeat_view")
{
    CHECK(std::ranges::equal(views::repeat(0, 5), std::vector<int>(5, 0)));
    CHECK(std::ranges::equal(views::repeat(0) | std::views::take(5),
                             std::vector<int>(5, 0)));
    CHECK(std::ranges::equal(views::repeat(std::views::iota(0, 5), 5)
                                 | std::views::join,
                             std::vector { 0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4 }));
}

TEST_SUITE_END();
