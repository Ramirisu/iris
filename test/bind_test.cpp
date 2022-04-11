#include <thirdparty/test.hpp>

#include <iris/bind.hpp>

using namespace iris;

TEST_SUITE_BEGIN("bind");

int muladd(int a, int b, int c)
{
    return a * b + c;
}

TEST_CASE("bind_front")
{
    const auto exp = muladd(2, 3, 5);
    CHECK_EQ(bind_front(muladd)(2, 3, 5), exp);
    CHECK_EQ(bind_front(muladd, 2)(3, 5), exp);
    CHECK_EQ(bind_front(muladd, 2, 3)(5), exp);
    CHECK_EQ(bind_front(muladd, 2, 3, 5)(), exp);
    CHECK_EQ(bind_front(bind_front(muladd, 2), 3)(5), exp);
    CHECK_EQ(bind_front(bind_front(muladd, 2), 3, 5)(), exp);
    CHECK_EQ(bind_front(bind_front(muladd, 2, 3), 5)(), exp);
    CHECK_EQ(bind_front(bind_front(bind_front(muladd, 2), 3), 5)(), exp);
}

TEST_CASE("bind_back")
{
    const auto exp = muladd(2, 3, 5);
    CHECK_EQ(bind_back(muladd)(2, 3, 5), exp);
    CHECK_EQ(bind_back(muladd, 5)(2, 3), exp);
    CHECK_EQ(bind_back(muladd, 3, 5)(2), exp);
    CHECK_EQ(bind_back(muladd, 2, 3, 5)(), exp);
    CHECK_EQ(bind_back(bind_back(muladd, 5), 3)(2), exp);
    CHECK_EQ(bind_back(bind_back(muladd, 5), 2, 3)(), exp);
    CHECK_EQ(bind_back(bind_back(muladd, 3, 5), 2)(), exp);
    CHECK_EQ(bind_back(bind_back(bind_back(muladd, 5), 3), 2)(), exp);
}

TEST_SUITE_END();
