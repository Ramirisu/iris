#pragma once

#include <iris/config.hpp>

#include <iris/detail/sync_wait.hpp>

#include <coroutine>
#include <type_traits>

namespace iris {

template <typename T = void>
class lazy;

namespace __lazy_detail {
    template <typename T>
    class lazy_promise_type_base {
    public:
        void return_value(T value)
        {
            value_ = std::move(value);
        }

        T result()
        {
            return std::move(value_);
        }

    private:
        T value_;
    };

    template <>
    class lazy_promise_type_base<void> {
    public:
        void return_void() noexcept { }

        void result() noexcept { }
    };

    template <typename T>
    class lazy_promise_type : public lazy_promise_type_base<T> {
    public:
        lazy<T> get_return_object();

        auto initial_suspend() noexcept
        {
            return std::suspend_always();
        }

        auto final_suspend() noexcept
        {
            class awaitable {
            public:
                awaitable(std::coroutine_handle<> continuation)
                    : continuation_(continuation)
                {
                }

                bool await_ready() noexcept
                {
                    return false;
                }

                auto await_suspend(std::coroutine_handle<>) noexcept
                {
                    return continuation_;
                }

                void await_resume() noexcept { }

                std::coroutine_handle<> continuation_;
            };

            return awaitable(continuation_);
        }

        void unhandled_exception() noexcept
        {
            std::terminate();
        }

        void set_continuation(std::coroutine_handle<> continuation) noexcept
        {
            continuation_ = continuation;
        }

    private:
        std::coroutine_handle<> continuation_;
    };
}

template <typename T>
class [[nodiscard]] lazy {
    friend class __lazy_detail::lazy_promise_type<T>;

public:
    using promise_type = __lazy_detail::lazy_promise_type<T>;
    using value_type = T;

    lazy(lazy&& other) noexcept
        : handle_(std::exchange(other.handle_))
    {
    }

    ~lazy() noexcept
    {
        if (handle_) {
            handle_.destroy();
        }
    }

    auto operator co_await() const noexcept
    {
        class awaitable {
        public:
            awaitable(std::coroutine_handle<promise_type> handle)
                : handle_(handle)
            {
            }

            bool await_ready() noexcept
            {
                return false;
            }

            auto await_suspend(std::coroutine_handle<> handle) noexcept
            {
                handle_.promise().set_continuation(handle);
                return handle_;
            }

            auto await_resume() noexcept
            {
                return handle_.promise().result();
            }

            std::coroutine_handle<promise_type> handle_;
        };

        return awaitable(handle_);
    }

    T sync_wait()
    {
        auto task = detail::sync_wait_impl(*this);
        task.start();
        return task.result();
    }

private:
    lazy(std::coroutine_handle<promise_type> handle) noexcept
        : handle_(handle)
    {
    }

    std::coroutine_handle<promise_type> handle_;
};

namespace __lazy_detail {
    template <typename T>
    lazy<T> lazy_promise_type<T>::get_return_object()
    {
        return std::coroutine_handle<lazy_promise_type<T>>::from_promise(*this);
    }
}
}
