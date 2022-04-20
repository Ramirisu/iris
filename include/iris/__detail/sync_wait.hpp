#pragma once

#include <iris/config.hpp>

#include <iris/__detail/manual_reset_event.hpp>
#include <iris/coroutine.hpp>

#include <coroutine>
#include <type_traits>

namespace iris::__detail {

template <typename T>
class __sync_wait;

template <typename T>
class __sync_wait_promise_type_base {
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
class __sync_wait_promise_type_base<void> {
public:
    void return_void() noexcept { }

    void result() noexcept { }
};

template <typename T>
class __sync_wait_promise_type : public __sync_wait_promise_type_base<T> {
public:
    __sync_wait<T> get_return_object();

    auto initial_suspend() noexcept
    {
        return std::suspend_always();
    }

    auto final_suspend() noexcept
    {
        IRIS_ASSERT(event_ != nullptr);
        event_->set();
        return std::suspend_always();
    }

    void unhandled_exception() noexcept
    {
        std::terminate();
    }

    void set_event(__manual_reset_event& event)
    {
        event_ = &event;
    }

private:
    __manual_reset_event* event_ = nullptr;
};

template <typename T>
class __sync_wait {
    friend class __sync_wait_promise_type<T>;

public:
    using promise_type = __sync_wait_promise_type<T>;
    using value_type = T;

    __sync_wait(__sync_wait&& other) noexcept
        : handle_(std::exchange(other.handle_))
    {
    }

    ~__sync_wait() noexcept
    {
        if (handle_) {
            handle_.destroy();
        }
    }

    void start()
    {
        __manual_reset_event event;
        handle_.promise().set_event(event);
        IRIS_ASSERT(handle_);
        handle_.resume();
        event.wait();
    }

    T result()
    {
        return handle_.promise().result();
    }

private:
    __sync_wait(std::coroutine_handle<promise_type> handle)
        : handle_(handle)
    {
    }

    std::coroutine_handle<promise_type> handle_;
};

template <typename Awaitable>
__sync_wait<awaitable_result_t<Awaitable>>
__sync_wait_impl(Awaitable&& obj) requires awaitable<Awaitable>
{
    co_return co_await std::forward<Awaitable>(obj);
}

template <typename T>
__sync_wait<T> __sync_wait_promise_type<T>::get_return_object()
{
    return std::coroutine_handle<__sync_wait_promise_type<T>>::from_promise(
        *this);
}

}
