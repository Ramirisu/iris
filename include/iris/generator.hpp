#pragma once

#include <iris/config.hpp>

#include <iris/ranges/elements_of.hpp>

#include <coroutine>
#include <ranges>
#include <type_traits>

namespace iris {
namespace __generator_detail {
    struct alignas(__STDCPP_DEFAULT_NEW_ALIGNMENT__) __default_new_alignment {
        std::byte d[__STDCPP_DEFAULT_NEW_ALIGNMENT__];
    };

    constexpr std::size_t __aligned_allocated_count(std::size_t size)
    {
        return (size + __STDCPP_DEFAULT_NEW_ALIGNMENT__ - 1)
            / __STDCPP_DEFAULT_NEW_ALIGNMENT__;
    }

    template <typename Allocator>
    class __frame_allocator {
    public:
        using pointer = typename std::allocator_traits<Allocator>::pointer;

        static constexpr void* allocate(Allocator allocator, std::size_t size)
        {
            auto n = __aligned_allocated_count(size);
            auto a = std::size_t(0);
            if constexpr (!std::allocator_traits<
                              Allocator>::is_always_equal::value) {
                a = __aligned_allocated_count(sizeof(Allocator));
            }

            auto p = allocator.allocate(n + a);

            if constexpr (!std::allocator_traits<
                              Allocator>::is_always_equal::value) {
                std::construct_at(get_allocator(p, n), std::move(allocator));
            }

            return p;
        }

        static constexpr void deallocate(void* p, std::size_t size)
        {
            auto n = __aligned_allocated_count(size);
            if constexpr (!std::allocator_traits<
                              Allocator>::is_always_equal::value) {
                Allocator allocator = std::move(*get_allocator(p, n));
                std::destroy_at(get_allocator(p, n));
                auto a = __aligned_allocated_count(sizeof(Allocator));
                allocator.deallocate(static_cast<pointer>(p), n + a);
            } else {
                Allocator allocator;
                allocator.deallocate(static_cast<pointer>(p), n);
            }
        }

    private:
        static constexpr Allocator* get_allocator(void* p, std::size_t n)
        {
            return reinterpret_cast<Allocator*>(static_cast<pointer>(p) + n);
        }
    };
}

template <typename R, typename V = void, typename Allocator = void>
class [[nodiscard]] generator {
public:
    using value
        = std::conditional_t<std::is_void_v<V>, std::remove_cvref_t<R>, V>;
    using reference = std::conditional_t<std::is_void_v<V>, R&&, R>;
    using yielded = std::conditional_t<std::is_reference_v<reference>,
                                       reference,
                                       const reference&>;

    // clang-format off
    static_assert(!std::is_const_v<value> && !std::is_reference_v<value>);
    static_assert(std::is_reference_v<reference> 
        || (!std::is_const_v<reference> 
            && !std::is_reference_v<reference> 
            && std::is_copy_constructible_v<reference>));
    // clang-format on

    class promise_type;
    class iterator;

    class promise_type {
        friend class iterator;

    public:
        promise_type()
            : root_(std::coroutine_handle<promise_type>::from_promise(*this))
        {
        }

        generator get_return_object() noexcept;

        auto initial_suspend() noexcept
        {
            return std::suspend_always();
        }

        auto final_suspend() noexcept
        {
            class awaitable {
            public:
                bool await_ready() noexcept
                {
                    return false;
                }

                std::coroutine_handle<> await_suspend(
                    std::coroutine_handle<promise_type> handle) noexcept
                {
                    // leaf coroutine is done, return control to parent
                    // coroutine.
                    if (handle.promise().parent_) {
                        handle.promise().root_.promise().root_
                            = handle.promise().parent_;
                        return handle.promise().parent_;
                    }

                    return std::noop_coroutine();
                }

                void await_resume() noexcept { }
            };

            return awaitable {};
        }

        void unhandled_exception()
        {
            if (root_.address()
                == std::coroutine_handle<promise_type>::from_promise(*this)
                       .address()) {
                // coroutine associated with this promise object is the sole
                // element of the coroutine stack
                throw;
            }

            // propagate to root
            root_.promise().exception_ = std::current_exception();
        }

        auto yield_value(yielded value) noexcept
        {
            root_.promise().set_value(&value);
            return std::suspend_always();
        }

        class yield_lvalue_awaitable {
            friend class promise_type;

        public:
            bool await_ready() noexcept
            {
                return false;
            }

            auto
            await_suspend(std::coroutine_handle<promise_type> handle) noexcept
            {
                handle.promise().root_.promise().set_value(&value_);
                return std::noop_coroutine();
            }

            void await_resume() noexcept { }

        private:
            yield_lvalue_awaitable(
                std::remove_cv_t<std::remove_reference_t<yielded>> value)
                : value_(value)
            {
            }

            std::remove_cv_t<std::remove_reference_t<yielded>> value_;
        };

        // clang-format off
        template <typename Yielded = std::remove_reference_t<yielded>>
        auto yield_value(const std::type_identity_t<Yielded>& lvalue) noexcept
            requires std::is_rvalue_reference_v<yielded> 
                && std::constructible_from<std::remove_cvref_t<yielded>, const Yielded&>
        // clang-format on
        {
            return yield_lvalue_awaitable { lvalue };
        }

        class yield_generator_awaitable {
            friend class promise_type;

        public:
            bool await_ready() noexcept
            {
                return !child_.handle();
            }

            auto
            await_suspend(std::coroutine_handle<promise_type> handle) noexcept
            {
                // child coroutine should yield value to root coroutine.
                child_.handle().promise().root_ = handle.promise().root_;

                // should resume child coroutine when try to resume the
                // root coroutine.
                handle.promise().root_.promise().root_ = child_.handle();

                // store this coroutien as parent of child coroutine.
                child_.handle().promise().parent_ = handle;

                return child_.handle();
            }

            void await_resume()
            {
                auto& root_promise = child_.handle().promise().root_.promise();
                if (root_promise.exception_) {
                    std::rethrow_exception(root_promise.exception_);
                }
            }

        private:
            yield_generator_awaitable(generator child)
                : child_(std::move(child))
            {
            }

            generator child_;
        };

        template <typename T2,
                  typename V2,
                  typename Allocator2,
                  typename Unused>
        auto yield_value(ranges::elements_of<generator<T2, V2, Allocator2>&&,
                                             Unused> range) noexcept requires
            std::same_as<typename generator<T2, V2, Allocator2>::yielded,
                         yielded>
        {
            return yield_generator_awaitable(std::move(range.range));
        }

        template <std::ranges::input_range Range, typename Allocator2>
        auto yield_value(
            ranges::elements_of<Range, Allocator2> range) noexcept requires
            std::convertible_to<std::ranges::range_reference_t<Range>, yielded>
        {
            auto nested = [](std::allocator_arg_t, Allocator2,
                             auto* range) -> generator<yielded, V, Allocator>
            // TODO:
            // -> generator<yielded, std::ranges::range_value_t<Range>,
            // Allocator2>
            {
                for (auto&& element : *range)
                    co_yield static_cast<yielded>(
                        std::forward<decltype(element)>(element));
            };

            return yield_value(ranges::elements_of(
                nested(std::allocator_arg, range.allocator, &range.range)));
        }

        void await_transform() = delete;

        void return_void() noexcept { }

        void set_value(std::add_pointer_t<yielded> value) noexcept
        {
            value_ = value;
        }

        void resume()
        {
            root_.resume();
        }

        static void* operator new(std::size_t size) requires
            std::same_as<Allocator,
                         void> || std::default_initializable<Allocator>
        {
            using U = __generator_detail::__default_new_alignment;
            using BAlloc = std::allocator_traits<std::conditional_t<
                std::same_as<Allocator, void>, std::allocator<void>,
                Allocator>>::template rebind_alloc<U>;

            return __generator_detail::__frame_allocator<BAlloc>::allocate(
                BAlloc(), size);
        }

        template <typename Alloc, typename... Args>
            requires std::same_as<Allocator,
                                  void> || std::convertible_to<Alloc, Allocator>
        static void* operator new(std::size_t size,
                                  std::allocator_arg_t,
                                  Alloc&& alloc,
                                  Args&...)
        {
            using U = __generator_detail::__default_new_alignment;
            using BAlloc = std::allocator_traits<std::conditional_t<
                std::same_as<Allocator, void>, std::allocator<void>,
                Allocator>>::template rebind_alloc<U>;

            return __generator_detail::__frame_allocator<BAlloc>::allocate(
                std::forward<Alloc>(alloc), size);
        }

        template <typename This, typename Alloc, typename... Args>
            requires std::same_as<Allocator,
                                  void> || std::convertible_to<Alloc, Allocator>
        static void* operator new(std::size_t size,
                                  This&,
                                  std::allocator_arg_t,
                                  Alloc&& alloc,
                                  Args&...)
        {
            using U = __generator_detail::__default_new_alignment;
            using BAlloc = std::allocator_traits<std::conditional_t<
                std::same_as<Allocator, void>, std::allocator<void>,
                Allocator>>::template rebind_alloc<U>;

            return __generator_detail::__frame_allocator<BAlloc>::allocate(
                std::forward<Alloc>(alloc), size);
        }

        static void operator delete(void* pointer, std::size_t size)
        {
            using U = __generator_detail::__default_new_alignment;
            using BAlloc = std::allocator_traits<std::conditional_t<
                std::same_as<Allocator, void>, std::allocator<void>,
                Allocator>>::template rebind_alloc<U>;

            return __generator_detail::__frame_allocator<BAlloc>::deallocate(
                pointer, size);
        }

    private:
        std::add_pointer_t<yielded> value_ = nullptr;
        std::exception_ptr exception_;
        std::coroutine_handle<promise_type> root_;
        std::coroutine_handle<promise_type> parent_;
    };

    class iterator {
        friend class generator;

    public:
        using value_type = value;
        using difference_type = std::ptrdiff_t;

        iterator() = default;

        iterator(iterator&& other)
            : handle_(std::exchange(other.handle_, {}))
        {
        }

        iterator& operator=(iterator&& other)
        {
            handle_ = std::exchange(other.handle_, {});
        }

        [[nodiscard]] reference operator*() const
            noexcept(std::is_nothrow_copy_constructible_v<reference>)
        {
            IRIS_ASSERT(handle_ && !handle_.done());
            return static_cast<reference>(*handle_.promise().value_);
        }

        iterator& operator++()
        {
            IRIS_ASSERT(handle_ && !handle_.done());
            handle_.promise().resume();
            return *this;
        }

        void operator++(int)
        {
            IRIS_ASSERT(handle_ && !handle_.done());
            ++*this;
        }

        [[nodiscard]] bool operator==(std::default_sentinel_t) const
        {
            return !handle_ || handle_.done();
        }

    private:
        explicit iterator(std::coroutine_handle<promise_type> handle) noexcept
            : handle_(handle)
        {
        }

        std::coroutine_handle<promise_type> handle_;
    };

    generator(const generator&) = delete;

    generator(generator&& other) noexcept
        : handle_(std::exchange(other.handle_, {}))
    {
    }

    ~generator()
    {
        if (handle_) {
            handle_.destroy();
        }
    }

    generator& operator=(const generator&) = delete;

    generator& operator=(generator&& other)
    {
        if (auto old
            = std::exchange(handle_, std::exchange(other.handle_, {}))) {
            old.destroy();
        }
    }

    [[nodiscard]] iterator begin()
    {
        handle_.resume();
        return iterator { handle_ };
    }

    [[nodiscard]] std::default_sentinel_t end()
    {
        return {};
    }

    [[nodiscard]] auto handle() const noexcept
    {
        return handle_;
    }

private:
    explicit generator(std::coroutine_handle<promise_type> handle)
        : handle_(handle)
    {
    }

    std::coroutine_handle<promise_type> handle_;
};

template <typename R, typename V, typename Allocator>
generator<R, V, Allocator>
generator<R, V, Allocator>::promise_type::get_return_object() noexcept
{
    return generator { std::coroutine_handle<promise_type>::from_promise(
        *this) };
}

}

namespace std::ranges {

template <typename R, typename V, typename Allocator>
inline constexpr bool enable_view<iris::generator<R, V, Allocator>> = true;

}
