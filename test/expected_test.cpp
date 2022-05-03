#include <thirdparty/test.hpp>

#include <iris/bind.hpp>
#include <iris/expected.hpp>
#include <iris/utility.hpp>

#include <functional>

using namespace iris;

TEST_SUITE_BEGIN("expected");

TEST_CASE("unexpected")
{
    SUBCASE("ctor")
    {
        CHECK_EQ(iris::unexpected<std::string>("e").error(), "e");
        CHECK_EQ(iris::unexpected<std::string>(std::in_place, "e").error(),
                 "e");
        CHECK_EQ(iris::unexpected<std::string>(std::in_place, { 'e' }).error(),
                 "e");
    }
    SUBCASE("copy ctor")
    {
        auto lvalue = iris::unexpected<std::string>("e");
        CHECK_EQ(iris::unexpected<std::string>(lvalue).error(), "e");
    }
    SUBCASE("move ctor")
    {
        CHECK_EQ(
            iris::unexpected<std::string>(iris::unexpected<std::string>("e"))
                .error(),
            "e");
    }
    SUBCASE("swap")
    {
        auto a = iris::unexpected<std::string>("a");
        auto b = iris::unexpected<std::string>("b");
        swap(a, b);
        CHECK_EQ(a.error(), "b");
        CHECK_EQ(b.error(), "a");
    }
    SUBCASE("operator==")
    {
        CHECK_EQ(iris::unexpected<std::string>("e"),
                 iris::unexpected<std::string_view>("e"));
    }
}

TEST_CASE("expected: ctor")
{
    CHECK(expected<void, int>());
    CHECK(!expected<void, int>(iris::unexpected(0)));
    CHECK(!expected<void, int>(unexpect, 0));
    CHECK_EQ(expected<int, int>().value(), 0);
    CHECK_EQ(expected<int, int>(0).value(), 0);
    CHECK_EQ(expected<int, int>(iris::unexpected(0)).error(), 0);
    CHECK_EQ(expected<int, int>(std::in_place, 0).value(), 0);
    CHECK_EQ(expected<int, int>(unexpect, 0).error(), 0);
    CHECK_EQ(expected<std::string, std::string>().value(), "");
    CHECK_EQ(expected<std::string, std::string>("v").value(), "v");
    CHECK_EQ(expected<std::string, std::string>(iris::unexpected("e")).error(),
             "e");
    CHECK_EQ(expected<std::string, std::string>(std::in_place, 1, 'v').value(),
             "v");
    CHECK_EQ(expected<std::string, std::string>(std::in_place, { 'v' }).value(),
             "v");
    CHECK_EQ(expected<std::string, std::string>(unexpect, "e").error(), "e");
    CHECK_EQ(expected<std::string, std::string>(unexpect, { 'e' }).error(),
             "e");
}

TEST_CASE("expected: assignment")
{
    SUBCASE("void")
    {
        static const auto exp = expected<void, int>();
        static const auto expu = expected<void, int>(unexpect, 1);
        static const auto unexp = iris::unexpected<int>(1);

        auto copy_assignment = expected<void, int>();
        copy_assignment = unexp;
        CHECK_EQ(copy_assignment.error(), 1);
        copy_assignment = exp;
        CHECK(copy_assignment);
        copy_assignment = expu;
        CHECK_EQ(copy_assignment.error(), 1);

        auto move_assignment = expected<void, int>();
        move_assignment = iris::unexpected<int>(unexp);
        CHECK_EQ(move_assignment.error(), 1);
        move_assignment = expected<void, int>(exp);
        CHECK(move_assignment);
        move_assignment = expected<void, int>(expu);
        CHECK_EQ(move_assignment.error(), 1);
    }
    SUBCASE("non-void")
    {
        static const auto exp = expected<int, int>(0);
        static const auto expu = expected<int, int>(unexpect, 1);
        static const auto unexp = iris::unexpected<int>(1);

        auto copy_assignment = expected<int, int>(1);
        copy_assignment = unexp;
        CHECK_EQ(copy_assignment.error(), 1);
        copy_assignment = exp;
        CHECK_EQ(copy_assignment.value(), 0);
        copy_assignment = expu;
        CHECK_EQ(copy_assignment.error(), 1);
        copy_assignment = 0;
        CHECK_EQ(copy_assignment.value(), 0);

        auto move_assignment = expected<int, int>(1);
        move_assignment = iris::unexpected<int>(unexp);
        CHECK_EQ(move_assignment.error(), 1);
        move_assignment = expected<int, int>(exp);
        CHECK_EQ(move_assignment.value(), 0);
        move_assignment = expected<int, int>(expu);
        CHECK_EQ(move_assignment.error(), 1);
        move_assignment = 0;
        CHECK_EQ(move_assignment.value(), 0);
    }
}

TEST_CASE("expected: emplace")
{
    class nothrow_constructible {
    public:
        nothrow_constructible(int value) noexcept
            : value_(value)
        {
        }

        nothrow_constructible(std::initializer_list<int> list) noexcept
            : value_(std::max(list))
        {
        }

        operator int() const noexcept
        {
            return value_;
        }

    private:
        int value_ {};
    };

    SUBCASE("void")
    {
        auto exp = expected<void, nothrow_constructible>(unexpect, 1);
        CHECK(!exp);
        exp.emplace();
        CHECK(exp);
    }
    SUBCASE("non-void")
    {
        auto exp = expected<nothrow_constructible, nothrow_constructible>(
            unexpect, 1);
        CHECK(!exp);
        exp.emplace(1);
        CHECK(exp);
        CHECK_EQ(exp.value(), 1);
        exp.emplace({ 2 });
        CHECK(exp);
        CHECK_EQ(exp.value(), 2);
    }
}

TEST_CASE("expected: swap")
{
    struct void_test_case_t {
        expected<void, int> lhs;
        expected<void, int> rhs;
    };

    static const void_test_case_t void_test_cases[] = {
        { iris::unexpected(0), iris::unexpected(1) },
    };

    for (auto& test_case : void_test_cases) {
        auto lhs = test_case.lhs;
        auto rhs = test_case.rhs;
        swap(lhs, rhs);
        CHECK_EQ(lhs, test_case.rhs);
        CHECK_EQ(rhs, test_case.lhs);
    }

    struct test_case_t {
        expected<int, int> lhs;
        expected<int, int> rhs;
    };

    static const test_case_t test_cases[] = {
        { 0, 1 },
        { 0, iris::unexpected(0) },
        { iris::unexpected(0), 0 },
        { iris::unexpected(0), iris::unexpected(1) },
    };

    for (auto& test_case : test_cases) {
        auto lhs = test_case.lhs;
        auto rhs = test_case.rhs;
        swap(lhs, rhs);
        CHECK_EQ(lhs, test_case.rhs);
        CHECK_EQ(rhs, test_case.lhs);
    }
}

TEST_CASE("expected: observers")
{
    auto get_nothing = [](bool success) -> expected<void, int> {
        if (success) {
            return {};
        }
        return iris::unexpected(0);
    };
    auto get_string = [](bool success) -> expected<std::string, std::string> {
        if (success) {
            return "v";
        }
        return iris::unexpected("e");
    };
    SUBCASE("void w/ value")
    {
        auto exp = get_nothing(true);
        CHECK(exp);
        CHECK(exp.has_value());
        CHECK_NOTHROW(exp.value());
        CHECK_NOTHROW(*exp);
    }
    SUBCASE("void w/ error")
    {
        auto exp = get_nothing(false);
        CHECK(!exp);
        CHECK(!exp.has_value());
        CHECK_THROWS_AS(exp.value(), bad_expected_access<int>);
        CHECK_EQ(exp.error(), 0);
    }
    SUBCASE("non-void w/ value")
    {
        auto exp = get_string(true);
        CHECK(exp);
        CHECK(exp.has_value());
        CHECK_EQ(exp.value_or("u"), "v");
        CHECK_EQ(exp.value(), "v");
        CHECK_EQ(*exp, "v");
        CHECK_EQ(exp->size(), std::string_view("v").size());
    }
    SUBCASE("non-voidw/ error")
    {
        auto exp = get_string(false);
        CHECK(!exp);
        CHECK(!exp.has_value());
        CHECK_EQ(exp.value_or("u"), "u");
        CHECK_THROWS_AS(exp.value(), bad_expected_access<std::string>);
        CHECK_EQ(exp.error(), "e");
    }
}

TEST_CASE("expected: ctor nested conversion")
{
    struct base {
        int b = 0;
    };

    struct derived : base {
        int d = 0;
    };

    auto inner_func = [](bool inner_success) -> expected<derived, int> {
        if (inner_success) {
            return derived { base { 1 }, 1 };
        }
        return iris::unexpected(0);
    };

    auto outer_func
        = [&](bool outer_success, bool inner_success) -> expected<base, int> {
        auto inner = inner_func(inner_success);
        if (!inner) {
            return inner;
        }
        if (outer_success) {
            return 2;
        }
        return iris::unexpected(1);
    };

    SUBCASE("(true, true)")
    {
        auto exp = outer_func(true, true);
        CHECK(exp);
    }
    SUBCASE("(false, true)")
    {
        auto exp = outer_func(false, true);
        CHECK(!exp);
        CHECK_EQ(exp.error(), 1);
    }
    SUBCASE("(true, false)")
    {
        auto exp = outer_func(true, false);
        CHECK(!exp);
        CHECK_EQ(exp.error(), 0);
    }
    SUBCASE("(false, false)")
    {
        auto exp = outer_func(false, false);
        CHECK(!exp);
        CHECK_EQ(exp.error(), 0);
    }
}

TEST_CASE("expected<T, E>: and_then")
{
    auto then_succeed = [](int x) { return expected<int, int>(x + 1); };
    auto then_fail = [](int x) { return expected<int, int>(unexpect, x - 1); };

    {
        expected<int, int> exp(0);
        auto result = exp.and_then(then_succeed);
        CHECK(result);
        CHECK_EQ(*result, 1);
    }
    {
        const expected<int, int> exp(0);
        auto result = exp.and_then(then_succeed);
        CHECK(result);
        CHECK_EQ(*result, 1);
    }
    {
        expected<int, int> exp(0);
        auto result = std::move(exp).and_then(then_succeed);
        CHECK(result);
        CHECK_EQ(*result, 1);
    }
    {
        const expected<int, int> exp(0);
        auto result = std::move(exp).and_then(then_succeed);
        CHECK(result);
        CHECK_EQ(*result, 1);
    }
    {
        expected<int, int> exp(0);
        auto result = exp.and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
    {
        const expected<int, int> exp(0);
        auto result = exp.and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
    {
        expected<int, int> exp(0);
        auto result = std::move(exp).and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
    {
        const expected<int, int> exp(0);
        auto result = std::move(exp).and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
}

TEST_CASE("expected<T, E>: or_else")
{
    auto handle_error = [](int x) { return expected<int, int>(x + 1); };

    {
        expected<int, int> exp(0);
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        const expected<int, int> exp(0);
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        expected<int, int> exp(0);
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        const expected<int, int> exp(0);
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        expected<int, int> exp(unexpect, 2);
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 3);
    }
    {
        const expected<int, int> exp(unexpect, 2);
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 3);
    }
    {
        expected<int, int> exp(unexpect, 2);
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 3);
    }
    {
        const expected<int, int> exp(unexpect, 2);
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(*result, 3);
    }
}

TEST_CASE("expected<T, E>: transform")
{
    static const auto succeed = std::string_view("succeed");
    auto f = [](int) { return expected<std::string_view, int>(succeed); };

    {
        expected<int, int> exp(0);
        auto result = exp.transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        const expected<int, int> exp(0);
        auto result = exp.transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        expected<int, int> exp(0);
        auto result = std::move(exp).transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        const expected<int, int> exp(0);
        auto result = std::move(exp).transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        expected<int, int> exp(unexpect, 1);
        auto result = exp.transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
    {
        const expected<int, int> exp(unexpect, 1);
        auto result = exp.transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
    {
        expected<int, int> exp(unexpect, 1);
        auto result = std::move(exp).transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
    {
        const expected<int, int> exp(unexpect, 1);
        auto result = std::move(exp).transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
}

TEST_CASE("expected<T, E>: transform_error")
{
    static const auto error = std::string_view("error");
    auto f = [](int) { return error; };

    {
        expected<int, int> exp(0);
        auto result = exp.transform_error(f);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        const expected<int, int> exp(0);
        auto result = std::move(exp).transform_error(f);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        expected<int, int> exp(0);
        auto result = exp.transform_error(f);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        const expected<int, int> exp(0);
        auto result = std::move(exp).transform_error(f);
        CHECK(result);
        CHECK_EQ(*result, 0);
    }
    {
        expected<int, int> exp(unexpect, 1);
        auto result = exp.transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
    {
        const expected<int, int> exp(unexpect, 1);
        auto result = std::move(exp).transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
    {
        expected<int, int> exp(unexpect, 1);
        auto result = exp.transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
    {
        const expected<int, int> exp(unexpect, 1);
        auto result = std::move(exp).transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
}

TEST_CASE("expected<void, E>: and_then")
{
    auto then_succeed = []() { return expected<void, int>(); };
    auto then_fail = []() { return expected<void, int>(unexpect, -1); };

    {
        expected<void, int> exp;
        auto result = exp.and_then(then_succeed);
        CHECK(result);
    }
    {
        const expected<void, int> exp;
        auto result = exp.and_then(then_succeed);
        CHECK(result);
    }
    {
        expected<void, int> exp;
        auto result = std::move(exp).and_then(then_succeed);
        CHECK(result);
    }
    {
        const expected<void, int> exp;
        auto result = std::move(exp).and_then(then_succeed);
        CHECK(result);
    }
    {
        expected<void, int> exp;
        auto result = exp.and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
    {
        const expected<void, int> exp;
        auto result = exp.and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
    {
        expected<void, int> exp;
        auto result = std::move(exp).and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
    {
        const expected<void, int> exp;
        auto result = std::move(exp).and_then(then_fail);
        CHECK(!result);
        CHECK_EQ(result.error(), -1);
    }
}

TEST_CASE("expected<void, E>: or_else")
{
    int check = 0;
    auto handle_error = [&]() { check += 1; };

    SUBCASE("")
    {
        expected<void, int> exp;
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 0);
    }
    SUBCASE("")
    {
        const expected<void, int> exp;
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 0);
    }
    SUBCASE("")
    {
        expected<void, int> exp;
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 0);
    }
    SUBCASE("")
    {
        const expected<void, int> exp;
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 0);
    }
    SUBCASE("")
    {
        expected<void, int> exp(unexpect, 2);
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 1);
    }
    SUBCASE("")
    {
        const expected<void, int> exp(unexpect, 2);
        auto result = exp.or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 1);
    }
    SUBCASE("")
    {
        expected<void, int> exp(unexpect, 2);
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 1);
    }
    SUBCASE("")
    {
        const expected<void, int> exp(unexpect, 2);
        auto result = std::move(exp).or_else(handle_error);
        CHECK(result);
        CHECK_EQ(check, 1);
    }
}

TEST_CASE("expected<void, E>: transform")
{
    static const auto succeed = std::string_view("succeed");
    auto f = []() { return expected<std::string_view, int>(succeed); };

    {
        expected<void, int> exp;
        auto result = exp.transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        const expected<void, int> exp;
        auto result = exp.transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        expected<void, int> exp;
        auto result = std::move(exp).transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        const expected<void, int> exp;
        auto result = std::move(exp).transform(f);
        CHECK(result);
        CHECK_EQ(*result, succeed);
    }
    {
        expected<void, int> exp(unexpect, 1);
        auto result = exp.transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
    {
        const expected<void, int> exp(unexpect, 1);
        auto result = exp.transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
    {
        expected<void, int> exp(unexpect, 1);
        auto result = std::move(exp).transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
    {
        const expected<void, int> exp(unexpect, 1);
        auto result = std::move(exp).transform(f);
        CHECK(!result);
        CHECK_EQ(result.error(), 1);
    }
}

TEST_CASE("expected<void, E>: transform_error")
{
    static const auto error = std::string_view("error");
    auto f = [](int) { return error; };

    {
        expected<void, int> exp;
        auto result = exp.transform_error(f);
        CHECK(result);
    }
    {
        const expected<void, int> exp;
        auto result = std::move(exp).transform_error(f);
        CHECK(result);
    }
    {
        expected<void, int> exp;
        auto result = exp.transform_error(f);
        CHECK(result);
    }
    {
        const expected<void, int> exp;
        auto result = std::move(exp).transform_error(f);
        CHECK(result);
    }
    {
        expected<void, int> exp(unexpect, 1);
        auto result = exp.transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
    {
        const expected<void, int> exp(unexpect, 1);
        auto result = std::move(exp).transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
    {
        expected<void, int> exp(unexpect, 1);
        auto result = exp.transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
    {
        const expected<void, int> exp(unexpect, 1);
        auto result = std::move(exp).transform_error(f);
        CHECK(!result);
        CHECK_EQ(result.error(), error);
    }
}

TEST_SUITE_END();
