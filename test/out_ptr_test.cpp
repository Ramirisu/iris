#include <thirdparty/test.hpp>

#include <iris/out_ptr.hpp>

#include <memory>

using namespace iris;

TEST_SUITE_BEGIN("out_ptr");

TEST_CASE("raw pointer")
{
    int value = 0;
    int* p = &value;
    {
        auto f = [](int** handle) {
            CHECK_EQ(**handle, 0);
            static int value = 1;
            *handle = &value;
        };
        f(out_ptr(p));
        CHECK_EQ(*p, 1);
    }
    {
        auto f = [](int** handle) {
            CHECK_EQ(**handle, 1);
            static int value = 2;
            *handle = &value;
        };
        f(inout_ptr(p));
        CHECK_EQ(*p, 2);
    }
}

TEST_CASE("shared_ptr")
{
    int count = 0;
    auto deleter = [&](int* handle) {
        delete handle;
        ++count;
    };
    auto p = std::make_shared<int>(0);
    {
        auto f = [](int** handle) {
            CHECK_EQ(**handle, 0);
            *handle = new int(1);
        };
        f(iris::out_ptr(p, deleter));
        CHECK_EQ(*p, 1);
        CHECK_EQ(count, 0);
    }
    {
        auto f = [](int** handle) {
            CHECK_EQ(**handle, 1);
            *handle = new int(2);
        };
        f(iris::out_ptr(p, deleter));
        CHECK_EQ(*p, 2);
        CHECK_EQ(count, 1);
    }
    p.reset();
    CHECK_EQ(count, 2);
}

TEST_SUITE_END();
