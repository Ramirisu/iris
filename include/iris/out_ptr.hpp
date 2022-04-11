#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <memory>
#include <tuple>

namespace iris {
namespace __out_ptr_detail {

    template <typename Smart>
    struct pointer_of_impl {
        using type = typename std::pointer_traits<Smart>::element_type*;
    };

    template <typename Smart>
        requires(detail::has_member_pointer_v<Smart>)
    struct pointer_of_impl<Smart>
        : std::type_identity<typename Smart::pointer> {
    };

    template <typename Smart>
        requires(!detail::has_member_pointer_v<
                     Smart> && detail::has_member_element_type_v<Smart>)
    struct pointer_of_impl<Smart>
        : std::type_identity<typename Smart::element_type*> {
    };

    template <typename Smart>
    using pointer_of = typename pointer_of_impl<Smart>::type;

    template <typename T, typename U>
    struct pointer_of_or_impl {
        using type = U;
    };

    template <typename T, typename U>
        requires requires
        {
            typename pointer_of<T>;
        }
    struct pointer_of_or_impl<T, U> {
        using type = pointer_of<T>;
    };

    template <typename T, typename U>
    using pointer_of_or = typename pointer_of_or_impl<T, U>::type;

    template <typename Smart, typename SP, typename Pointer, typename... Args>
    concept resettable_with
        = requires(Smart& smart, Pointer pointer, Args&&... args)
    {
        smart.reset(static_cast<SP>(pointer), std::forward<Args>(args)...);
    };

    template <typename Smart>
    auto get_pointer(Smart& smart)
    {
        if constexpr (std::is_pointer_v<Smart>) {
            return smart;
        } else {
            return smart.get();
        }
    }
}

template <typename Smart, typename Pointer, typename... Args>
class out_ptr_t {
public:
    explicit out_ptr_t(Smart& smart, Args... args)
        : smart_(smart)
        , args_(std::forward<Args>(args)...)
        , pointer_(__out_ptr_detail::get_pointer(smart))
    {
    }

    out_ptr_t(const out_ptr_t&) = delete;

    ~out_ptr_t()
    {
        if (pointer_) {
            using SP = __out_ptr_detail::pointer_of_or<Smart, Pointer>;
            if constexpr (__out_ptr_detail::resettable_with<Smart, SP, Pointer,
                                                            Args...>) {
                std::apply(
                    [&](auto&&... args) {
                        smart_.reset(static_cast<SP>(pointer_),
                                     std::forward<Args>(args)...);
                    },
                    std::move(args_));
            } else if constexpr (std::is_constructible_v<Smart, SP, Args...>) {
                std::apply(
                    [&](auto&&... args) {
                        smart_ = Smart(static_cast<SP>(pointer_),
                                       std::forward<Args>(args)...);
                    },
                    std::move(args_));
            } else {
                static_assert(always_false_v<Smart>);
            }
        }
    }

    operator Pointer*() const noexcept requires(!std::is_same_v<Pointer, void*>)
    {
        return std::addressof(const_cast<Pointer&>(pointer_));
    }

    operator void**() const noexcept
    {
        return reinterpret_cast<void**>(static_cast<Pointer*>(*this));
    }

private:
    Smart& smart_;
    std::tuple<Args...> args_;
    Pointer pointer_;
};

template <typename Pointer = void, typename Smart, typename... Args>
auto out_ptr(Smart& smart, Args&&... args)
{
    if constexpr (std::is_same_v<Pointer, void>) {
        return out_ptr_t<Smart, __out_ptr_detail::pointer_of<Smart>, Args...>(
            smart, std::forward<Args>(args)...);
    } else {
        return out_ptr_t<Smart, Pointer, Args...>(smart,
                                                  std::forward<Args>(args)...);
    }
}

template <typename Smart, typename Pointer, typename... Args>
class inout_ptr_t {
    static_assert(!is_specialized_of_v<Smart, std::shared_ptr>);

public:
    explicit inout_ptr_t(Smart& smart, Args... args)
        : smart_(smart)
        , args_(std::forward<Args>(args)...)
        , pointer_(__out_ptr_detail::get_pointer(smart))
    {
    }

    inout_ptr_t(const inout_ptr_t&) = delete;

    ~inout_ptr_t()
    {
        if (pointer_) {
            using SP = __out_ptr_detail::pointer_of_or<Smart, Pointer>;
            if constexpr (std::is_pointer_v<Smart>) {
                std::apply(
                    [&](auto&&... args) {
                        smart_ = Smart(static_cast<SP>(pointer_),
                                       std::forward<Args>(args)...);
                    },
                    std::move(args_));
            } else if constexpr (__out_ptr_detail::resettable_with<
                                     Smart, SP, Pointer, Args...>) {
                std::apply(
                    [&](auto&&... args) {
                        smart_.release();
                        smart_.reset(static_cast<SP>(pointer_),
                                     std::forward<Args>(args)...);
                    },
                    std::move(args_));
            } else if constexpr (std::is_constructible_v<Smart, SP, Args...>) {
                std::apply(
                    [&](auto&&... args) {
                        smart_.release();
                        smart_ = Smart(static_cast<SP>(pointer_),
                                       std::forward<Args>(args)...);
                    },
                    std::move(args_));
            } else {
                static_assert(always_false_v<Smart>);
            }
        }
    }

    operator Pointer*() const noexcept requires(!std::is_same_v<Pointer, void*>)
    {
        return std::addressof(const_cast<Pointer&>(pointer_));
    }

    operator void**() const noexcept
    {
        return reinterpret_cast<void**>(static_cast<Pointer*>(*this));
    }

private:
    Smart& smart_;
    std::tuple<Args...> args_;
    Pointer pointer_;
};

template <typename Pointer = void, typename Smart, typename... Args>
auto inout_ptr(Smart& smart, Args&&... args)
{
    if constexpr (std::is_same_v<Pointer, void>) {
        return inout_ptr_t<Smart, __out_ptr_detail::pointer_of<Smart>, Args...>(
            smart, std::forward<Args>(args)...);
    } else {
        return inout_ptr_t<Smart, Pointer, Args...>(
            smart, std::forward<Args>(args)...);
    }
}

}
