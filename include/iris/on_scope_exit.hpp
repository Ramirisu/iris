#pragma once

#include <iris/config.hpp>

#include <exception>
#include <type_traits>

namespace iris {

template <typename Fn>
    requires std::is_nothrow_invocable_v<Fn>
class [[nodiscard]] on_scope_exit {
public:
    template <typename Fn2>
    on_scope_exit(Fn2&& fn)
        : fn_(std::forward<Fn2>(fn))
        , commit_(false)
    {
    }

    on_scope_exit(const on_scope_exit&) = delete;
    on_scope_exit& operator=(const on_scope_exit&) = delete;

    ~on_scope_exit() noexcept
    {
        if (!commit_) {
            fn_();
        }
    }

    void commit() noexcept
    {
        commit_ = true;
    }

private:
    Fn fn_;
    bool commit_;
};

template <typename Fn>
on_scope_exit(Fn&&) -> on_scope_exit<Fn>;

template <typename Fn>
    requires std::is_nothrow_invocable_v<Fn>
class [[nodiscard]] on_scope_failure {
public:
    template <typename Fn2>
    on_scope_failure(Fn2&& fn)
        : fn_(std::forward<Fn2>(fn))
        , uncaught_exceptions_(std::uncaught_exceptions())
        , commit_(false)
    {
    }

    on_scope_failure(const on_scope_failure&) = delete;
    on_scope_failure& operator=(const on_scope_failure&) = delete;

    ~on_scope_failure() noexcept
    {
        if (!commit_ && uncaught_exceptions_ < std::uncaught_exceptions()) {
            fn_();
        }
    }

    void commit() noexcept
    {
        commit_ = true;
    }

private:
    Fn fn_;
    int uncaught_exceptions_;
    bool commit_;
};

template <typename Fn>
on_scope_failure(Fn&&) -> on_scope_failure<Fn>;

template <typename Fn>
    requires std::is_nothrow_invocable_v<Fn>
class [[nodiscard]] on_scope_success {
public:
    template <typename Fn2>
    on_scope_success(Fn2&& fn)
        : fn_(std::forward<Fn2>(fn))
        , uncaught_exceptions_(std::uncaught_exceptions())
        , commit_(false)
    {
    }

    on_scope_success(const on_scope_success&) = delete;
    on_scope_success& operator=(const on_scope_success&) = delete;

    ~on_scope_success() noexcept
    {
        if (!commit_ && uncaught_exceptions_ >= std::uncaught_exceptions()) {
            fn_();
        }
    }

    void commit() noexcept
    {
        commit_ = true;
    }

private:
    Fn fn_;
    int uncaught_exceptions_;
    bool commit_;
};

template <typename Fn>
on_scope_success(Fn&&) -> on_scope_success<Fn>;

}
