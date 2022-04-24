#include <thirdparty/test.hpp>

#include <iris/ranges/algorithm/shift.hpp>

#include <span>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("shift");

struct test_case_t {
    std::vector<int> input;
    std::size_t shift;
    std::vector<int> expected;
    std::size_t offset;
    std::size_t size;
};

TEST_CASE("shift_left")
{
    static const auto test_cases = std::vector<test_case_t> {
        { { 0, 1, 2, 3, 4 }, 0, { 0, 1, 2, 3, 4 }, 0, 5 },
        { { 0, 1, 2, 3, 4 }, 1, { 1, 2, 3, 4, 4 }, 1, 4 },
        { { 0, 1, 2, 3, 4 }, 2, { 2, 3, 4, 3, 4 }, 2, 3 },
        { { 0, 1, 2, 3, 4 }, 3, { 3, 4, 2, 3, 4 }, 3, 2 },
        { { 0, 1, 2, 3, 4 }, 4, { 4, 1, 2, 3, 4 }, 4, 1 },
        { { 0, 1, 2, 3, 4 }, 5, { 0, 1, 2, 3, 4 }, 5, 0 },
    };

    for (const auto& test_case : test_cases) {
        auto input = test_case.input;
        CHECK(
            std::ranges::equal(ranges::shift_left(input, test_case.shift),
                               std::span(test_case.input)
                                   .subspan(test_case.offset, test_case.size)));
        CHECK_EQ(input, test_case.expected);
    }
}

TEST_CASE("shift_right")
{
    static const auto test_cases = std::vector<test_case_t> {
        { { 0, 1, 2, 3, 4 }, 0, { 0, 1, 2, 3, 4 }, 0, 5 },
        { { 0, 1, 2, 3, 4 }, 1, { 0, 0, 1, 2, 3 }, 0, 4 },
        { { 0, 1, 2, 3, 4 }, 2, { 0, 1, 0, 1, 2 }, 0, 3 },
        { { 0, 1, 2, 3, 4 }, 3, { 0, 1, 2, 0, 1 }, 0, 2 },
        { { 0, 1, 2, 3, 4 }, 4, { 0, 1, 2, 3, 0 }, 0, 1 },
        { { 0, 1, 2, 3, 4 }, 5, { 0, 1, 2, 3, 4 }, 0, 0 },
    };

    for (const auto& test_case : test_cases) {
        auto input = test_case.input;
        CHECK(
            std::ranges::equal(ranges::shift_right(input, test_case.shift),
                               std::span(test_case.input)
                                   .subspan(test_case.offset, test_case.size)));
        CHECK_EQ(input, test_case.expected);
    }
}

TEST_SUITE_END();
