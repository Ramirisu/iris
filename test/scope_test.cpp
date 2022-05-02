#include <thirdparty/test.hpp>

#include <iris/scope.hpp>

using namespace iris;

TEST_SUITE_BEGIN("scope");

template <template <typename...> class ScopeType>
void test_scope_exit(bool release, bool throw_exception, bool result)
{
    bool called = false;

    try {
        ScopeType guard([&]() noexcept { called = true; });
        if (release) {
            guard.release();
        }
        if (throw_exception) {
            throw std::exception();
        }
    } catch (const std::exception& ex) {
        IRIS_UNUSED(ex);
    }
    CHECK_EQ(called, result);
}

TEST_CASE("scope_[exit|failure|success]")
{
    test_scope_exit<scope_exit>(false, false, true);
    test_scope_exit<scope_exit>(false, true, true);
    test_scope_exit<scope_exit>(true, false, false);
    test_scope_exit<scope_exit>(true, true, false);
    test_scope_exit<scope_failure>(false, false, false);
    test_scope_exit<scope_failure>(false, true, true);
    test_scope_exit<scope_failure>(true, false, false);
    test_scope_exit<scope_failure>(true, true, false);
    test_scope_exit<scope_success>(false, false, true);
    test_scope_exit<scope_success>(false, true, false);
    test_scope_exit<scope_success>(true, false, false);
    test_scope_exit<scope_success>(true, true, false);
}

TEST_SUITE_END();
