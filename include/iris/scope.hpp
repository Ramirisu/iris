#pragma once

#include <iris/config.hpp>

#include <exception>
#include <type_traits>

namespace iris {

template <typename Fn>
    requires std::is_nothrow_invocable_v<Fn>
class [[nodiscard]] scope_exit {
public:
    template <typename Fn2>
    scope_exit(Fn2&& fn)
        : fn_(std::forward<Fn2>(fn))
        , release_(false)
    {
    }

    scope_exit(const scope_exit&) = delete;
    scope_exit& operator=(const scope_exit&) = delete;

    ~scope_exit() noexcept
    {
        if (!release_) {
            fn_();
        }
    }

    void release() noexcept
    {
        release_ = true;
    }

private:
    Fn fn_;
    bool release_;
};

template <typename Fn>
scope_exit(Fn&&) -> scope_exit<Fn>;

template <typename Fn>
    requires std::is_nothrow_invocable_v<Fn>
class [[nodiscard]] scope_failure {
public:
    template <typename Fn2>
    scope_failure(Fn2&& fn)
        : fn_(std::forward<Fn2>(fn))
        , uncaught_exceptions_(std::uncaught_exceptions())
        , release_(false)
    {
    }

    scope_failure(const scope_failure&) = delete;
    scope_failure& operator=(const scope_failure&) = delete;

    ~scope_failure() noexcept
    {
        if (!release_ && uncaught_exceptions_ < std::uncaught_exceptions()) {
            fn_();
        }
    }

    void release() noexcept
    {
        release_ = true;
    }

private:
    Fn fn_;
    int uncaught_exceptions_;
    bool release_;
};

template <typename Fn>
scope_failure(Fn&&) -> scope_failure<Fn>;

template <typename Fn>
    requires std::is_nothrow_invocable_v<Fn>
class [[nodiscard]] scope_success {
public:
    template <typename Fn2>
    scope_success(Fn2&& fn)
        : fn_(std::forward<Fn2>(fn))
        , uncaught_exceptions_(std::uncaught_exceptions())
        , release_(false)
    {
    }

    scope_success(const scope_success&) = delete;
    scope_success& operator=(const scope_success&) = delete;

    ~scope_success() noexcept
    {
        if (!release_ && uncaught_exceptions_ >= std::uncaught_exceptions()) {
            fn_();
        }
    }

    void release() noexcept
    {
        release_ = true;
    }

private:
    Fn fn_;
    int uncaught_exceptions_;
    bool release_;
};

template <typename Fn>
scope_success(Fn&&) -> scope_success<Fn>;

}
