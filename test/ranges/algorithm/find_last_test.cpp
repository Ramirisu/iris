#include <thirdparty/test.hpp>

#include <iris/ranges/algorithm/find_last.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("find_last");

TEST_CASE("find_last")
{
    struct test_case_t {
        int value;
        std::vector<int> expected;
    };

    static const auto test_cases
        = std::vector<test_case_t> { { -1, {} },
                                     { 0, { 0 } },
                                     { 3, { 3, 0 } },
                                     { 2, { 2, 3, 0 } },
                                     { 1, { 1, 2, 3, 0 } } };

    static const auto foward_range = std::forward_list { 0, 1, 2, 3, 0 };
    for (auto&& test_case : test_cases) {
        CHECK(
            std::ranges::equal(ranges::find_last(foward_range, test_case.value),
                               test_case.expected));
    }

    static const auto bidirectional_range = std::list { 0, 1, 2, 3, 0 };
    for (auto&& test_case : test_cases) {
        CHECK(std::ranges::equal(
            ranges::find_last(bidirectional_range, test_case.value),
            test_case.expected));
    }
}

TEST_CASE("find_last_if")
{
    struct test_case_t {
        int value;
        std::vector<int> expected;
    };

    static const auto test_cases
        = std::vector<test_case_t> { { -1, {} },
                                     { 0, { 0 } },
                                     { 3, { 3, 0 } },
                                     { 2, { 2, 3, 0 } },
                                     { 1, { 1, 2, 3, 0 } } };

    static const auto foward_range = std::forward_list { 0, 1, 2, 3, 0 };
    for (auto&& test_case : test_cases) {
        CHECK(std::ranges::equal(
            ranges::find_last_if(
                foward_range,
                [&](auto& value) { return value == test_case.value; }),
            test_case.expected));
    }

    static const auto bidirectional_range = std::list { 0, 1, 2, 3, 0 };
    for (auto&& test_case : test_cases) {
        CHECK(std::ranges::equal(
            ranges::find_last_if(
                bidirectional_range,
                [&](auto& value) { return value == test_case.value; }),
            test_case.expected));
    }
}

TEST_CASE("find_last_if_not")
{
    struct test_case_t {
        int value;
        std::vector<int> expected;
    };

    static const auto test_cases = std::vector<test_case_t> {
        { -1, { 0 } }, { 0, { 3, 0 } }, { 3, { 0 } }, { 2, { 0 } }, { 1, { 0 } }
    };

    static const auto foward_range = std::forward_list { 0, 1, 2, 3, 0 };
    for (auto&& test_case : test_cases) {
        CHECK(std::ranges::equal(
            ranges::find_last_if_not(
                foward_range,
                [&](auto& value) { return value == test_case.value; }),
            test_case.expected));
    }

    static const auto bidirectional_range = std::list { 0, 1, 2, 3, 0 };
    for (auto&& test_case : test_cases) {
        CHECK(std::ranges::equal(
            ranges::find_last_if_not(
                bidirectional_range,
                [&](auto& value) { return value == test_case.value; }),
            test_case.expected));
    }
}

TEST_SUITE_END();
