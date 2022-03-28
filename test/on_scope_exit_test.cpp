#include <thirdparty/test.hpp>

#include <iris/on_scope_exit.hpp>

using namespace iris;

TEST_SUITE_BEGIN("on_scope_exit");

template <template <typename...> class OnScopeType>
void test_on_scope_exit(bool commit, bool throw_exception, bool result)
{
    bool called = false;

    try {
        OnScopeType guard([&]() noexcept { called = true; });
        if (commit) {
            guard.commit();
        }
        if (throw_exception) {
            throw std::exception();
        }
    } catch (const std::exception& ex) {
    }
    CHECK_EQ(called, result);
}

TEST_CASE("on_scope_[exit|failure|success]")
{
    test_on_scope_exit<on_scope_exit>(false, false, true);
    test_on_scope_exit<on_scope_exit>(false, true, true);
    test_on_scope_exit<on_scope_exit>(true, false, false);
    test_on_scope_exit<on_scope_exit>(true, true, false);
    test_on_scope_exit<on_scope_failure>(false, false, false);
    test_on_scope_exit<on_scope_failure>(false, true, true);
    test_on_scope_exit<on_scope_failure>(true, false, false);
    test_on_scope_exit<on_scope_failure>(true, true, false);
    test_on_scope_exit<on_scope_success>(false, false, true);
    test_on_scope_exit<on_scope_success>(false, true, false);
    test_on_scope_exit<on_scope_success>(true, false, false);
    test_on_scope_exit<on_scope_success>(true, true, false);
}

TEST_SUITE_END();
