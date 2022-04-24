#include <thirdparty/test.hpp>

#include <iris/expected.hpp>

using namespace iris;

TEST_SUITE_BEGIN("expected");

enum class error_code {
    timeout = 1,
    invalid_argument,
};

TEST_CASE("unexpected")
{
    iris::unexpected<error_code> unexp(error_code::timeout);
    CHECK_EQ(unexp.error(), error_code::timeout);
}

TEST_CASE("ctor")
{
    CHECK(expected<void, error_code>());
    CHECK(!expected<void, error_code>(iris::unexpected(error_code::timeout)));
    CHECK(!expected<void, error_code>(unexpect, error_code::timeout));
    CHECK(expected<int, error_code>());
    CHECK(expected<int, error_code>(100));
    CHECK(!expected<int, error_code>(iris::unexpected(error_code::timeout)));
    CHECK(expected<int, error_code>(std::in_place, 0));
    CHECK(!expected<int, error_code>(unexpect, error_code::timeout));
    CHECK(expected<std::string, error_code>());
    CHECK(expected<std::string, error_code>("success"));
    CHECK(!expected<std::string, error_code>(
        iris::unexpected(error_code::timeout)));
    CHECK(expected<std::string, error_code>(std::in_place, 0, 1));
    CHECK(!expected<std::string, error_code>(unexpect, error_code::timeout));
}

TEST_CASE("ctor nested conversion")
{
    struct base {
        int b = 0;
    };

    struct derived : base {
        int d = 0;
    };

    auto inner_func = [](bool inner_success) -> expected<derived, error_code> {
        if (inner_success) {
            return derived { base { 1 }, 1 };
        }
        return iris::unexpected(error_code::invalid_argument);
    };

    auto outer_func = [&](bool outer_success,
                          bool inner_success) -> expected<base, error_code> {
        auto inner = inner_func(inner_success);
        if (!inner) {
            return inner;
        }
        if (outer_success) {
            return 2;
        }
        return iris::unexpected(error_code::timeout);
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
        CHECK_EQ(exp.error(), error_code::timeout);
    }
    SUBCASE("(true, false)")
    {
        auto exp = outer_func(true, false);
        CHECK(!exp);
        CHECK_EQ(exp.error(), error_code::invalid_argument);
    }
    SUBCASE("(false, false)")
    {
        auto exp = outer_func(false, false);
        CHECK(!exp);
        CHECK_EQ(exp.error(), error_code::invalid_argument);
    }
}

TEST_CASE("emplace")
{
    SUBCASE("non-void")
    {
        struct nothrow_constructible {
            nothrow_constructible(int a, int b) noexcept
                : a(a)
                , b(b)
            {
            }

            int a = 0;
            int b = 0;
        };

        expected<nothrow_constructible, error_code> exp
            = iris::unexpected(error_code::timeout);
        CHECK(!exp);
        exp.emplace(10, 20);
        CHECK(exp);
        CHECK_EQ(exp.value().a, 10);
        CHECK_EQ(exp.value().b, 20);
    }
    SUBCASE("void")
    {
        expected<void, error_code> exp = iris::unexpected(error_code::timeout);
        CHECK(!exp);
        exp.emplace();
        CHECK(exp);
    }
}

TEST_CASE("swap")
{
    SUBCASE("swap(value, value)")
    {
        expected<int, error_code> lhs = 0;
        expected<int, error_code> rhs = 1;
        swap(lhs, rhs);
        CHECK(lhs);
        CHECK_EQ(lhs.value(), 1);
        CHECK(rhs);
        CHECK_EQ(rhs.value(), 0);
    }
    SUBCASE("swap(value, error)")
    {
        expected<int, error_code> lhs = 0;
        expected<int, error_code> rhs = iris::unexpected(error_code::timeout);
        swap(lhs, rhs);
        CHECK(!lhs);
        CHECK_EQ(lhs.error(), error_code::timeout);
        CHECK(rhs);
        CHECK_EQ(rhs.value(), 0);
    }
    SUBCASE("swap(error, value)")
    {
        expected<int, error_code> lhs = iris::unexpected(error_code::timeout);
        expected<int, error_code> rhs = 1;
        swap(lhs, rhs);
        CHECK(lhs);
        CHECK_EQ(lhs.value(), 1);
        CHECK(!rhs);
        CHECK_EQ(rhs.error(), error_code::timeout);
    }
    SUBCASE("swap(error, error)")
    {
        expected<int, error_code> lhs = iris::unexpected(error_code::timeout);
        expected<int, error_code> rhs = iris::unexpected(error_code::timeout);
        swap(lhs, rhs);
        CHECK(!lhs);
        CHECK_EQ(lhs.error(), error_code::timeout);
        CHECK(!rhs);
        CHECK_EQ(rhs.error(), error_code::timeout);
    }
    SUBCASE("swap(void, void)")
    {
        expected<void, error_code> lhs;
        expected<void, error_code> rhs;
        swap(lhs, rhs);
        CHECK(lhs);
        CHECK(rhs);
    }
    SUBCASE("swap(void, error)")
    {
        expected<void, error_code> lhs;
        expected<void, error_code> rhs = iris::unexpected(error_code::timeout);
        swap(lhs, rhs);
        CHECK(!lhs);
        CHECK_EQ(lhs.error(), error_code::timeout);
        CHECK(rhs);
    }
    SUBCASE("swap(error, void)")
    {
        expected<void, error_code> lhs = iris::unexpected(error_code::timeout);
        expected<void, error_code> rhs;
        swap(lhs, rhs);
        CHECK(lhs);
        CHECK(!rhs);
        CHECK_EQ(rhs.error(), error_code::timeout);
    }
    SUBCASE("swap(error, error)")
    {
        expected<void, error_code> lhs = iris::unexpected(error_code::timeout);
        expected<void, error_code> rhs = iris::unexpected(error_code::timeout);
        swap(lhs, rhs);
        CHECK(!lhs);
        CHECK_EQ(lhs.error(), error_code::timeout);
        CHECK(!rhs);
        CHECK_EQ(rhs.error(), error_code::timeout);
    }
}

expected<int, error_code> run(bool success)
{
    if (success) {
        return 0;
    }
    return iris::unexpected(error_code::timeout);
}

TEST_CASE("expected w/ value")
{
    auto exp = run(true);
    CHECK(exp);
    CHECK(exp.has_value());
    CHECK_EQ(exp.value_or(1), 0);
    CHECK_EQ(exp.value(), 0);
}

TEST_CASE("expected w/ error")
{
    auto exp = run(false);
    CHECK(!exp);
    CHECK(!exp.has_value());
    CHECK_EQ(exp.value_or(1), 1);
    CHECK_EQ(exp.error(), error_code::timeout);
}

TEST_CASE("equality")
{
    CHECK_EQ(expected<int, error_code>(1), expected<int, error_code>(1));
    CHECK_NE(expected<int, error_code>(1), expected<int, error_code>(0));
    CHECK_NE(expected<int, error_code>(1),
             expected<int, error_code>(iris::unexpected(error_code::timeout)));
    CHECK_NE(expected<int, error_code>(iris::unexpected(error_code::timeout)),
             expected<int, error_code>(1));
    CHECK_EQ(expected<int, error_code>(iris::unexpected(error_code::timeout)),
             expected<int, error_code>(iris::unexpected(error_code::timeout)));
    CHECK_EQ(expected<int, error_code>(1), 1);
    CHECK_NE(expected<int, error_code>(1),
             iris::unexpected(error_code::timeout));
    CHECK_EQ(expected<int, error_code>(iris::unexpected(error_code::timeout)),
             iris::unexpected(error_code::timeout));

    CHECK_EQ(expected<void, error_code>(), expected<void, error_code>());
    CHECK_NE(expected<void, error_code>(),
             expected<void, error_code>(iris::unexpected(error_code::timeout)));
    CHECK_NE(expected<void, error_code>(iris::unexpected(error_code::timeout)),
             expected<void, error_code>());
    CHECK_EQ(expected<void, error_code>(iris::unexpected(error_code::timeout)),
             expected<void, error_code>(iris::unexpected(error_code::timeout)));
    CHECK_NE(expected<void, error_code>(),
             iris::unexpected(error_code::timeout));
    CHECK_EQ(expected<void, error_code>(iris::unexpected(error_code::timeout)),
             iris::unexpected(error_code::timeout));
}

TEST_SUITE_END();
