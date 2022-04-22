#include <thirdparty/test.hpp>

#include <iris/algorithm/contains.hpp>

#include <span>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("contains");

TEST_CASE("contains")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    for (std::size_t i = 0; i < std::size(input); ++i) {
        CHECK(ranges::contains(std::ranges::begin(input),
                               std::ranges::end(input), input[i]));
        CHECK(ranges::contains(input, input[i]));
    }

    const auto test_cases = std::vector { -1, 5 };

    for (const auto& test_case : test_cases) {
        CHECK(!ranges::contains(input, test_case));
        CHECK(!ranges::contains(std::ranges::begin(input),
                                std::ranges::end(input), test_case));
    }
}

TEST_CASE("contains_subrange")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    for (std::size_t index = 0; index < std::size(input); ++index) {
        for (std::size_t size = 1; index + size < std::size(input); ++size) {
            const auto subrange = std::span(input).subspan(index, size);
            CHECK(ranges::contains_subrange(
                std::ranges::begin(input), std::ranges::end(input),
                std::ranges::begin(subrange), std::ranges::end(subrange)));
            CHECK(ranges::contains_subrange(input, subrange));
        }
    }

    const auto empty = std::views::empty<int>;
    CHECK(ranges::contains_subrange(input, empty));
    CHECK(ranges::contains_subrange(
        std::ranges::begin(input), std::ranges::end(input),
        std::ranges::begin(empty), std::ranges::end(empty)));

    const auto test_cases = std::vector {
        std::vector { -1 },
        std::vector { 5 },
        std::vector { 4, 4 },
        std::vector { 0, 0, 1, 2, 3, 4 },
        std::vector { 0, 1, 2, 3, 4, 5 },
    };

    for (const auto& test_case : test_cases) {
        CHECK(!ranges::contains_subrange(input, test_case));
        CHECK(!ranges::contains_subrange(
            std::ranges::begin(input), std::ranges::end(input),
            std::ranges::begin(test_case), std::ranges::end(test_case)));
    }
}

TEST_SUITE_END();
