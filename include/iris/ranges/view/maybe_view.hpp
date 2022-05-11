#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/copyable_box.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {
namespace __maybe_view_detail {
    template <typename T>
    struct __is_reference_wrapper : std::false_type {
    };

    template <typename T>
    struct __is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {
    };

    template <typename T>
    inline constexpr bool __is_reference_wrapper_v
        = __is_reference_wrapper<T>::value;

    // clang-format off
    template <typename Ref, typename ConstRef>
    concept __readable_references = 
        std::is_lvalue_reference_v<Ref> 
        && std::is_object_v<std::remove_reference_t<Ref>> 
        && std::is_lvalue_reference_v<ConstRef> 
        && std::is_object_v<std::remove_reference_t<ConstRef>> 
        && std::convertible_to<
            std::add_pointer_t<ConstRef>, 
            const std::remove_reference_t<Ref>*>;
    // clang-format on

    // clang-format off
    template <typename T>
    concept __nullable = 
        std::is_object_v<T>
        && requires(T& t, const T& ct) {
            bool(ct);
            *t;
            *ct; }
        && __readable_references<
            std::iter_reference_t<T>, 
            std::iter_reference_t<const T>>;
    // clang-format on

    template <typename T>
    concept __wrapped_nullable
        = __is_reference_wrapper_v<T> && __nullable<typename T::type>;
}

template <std::copy_constructible Maybe>
    requires(__maybe_view_detail::__nullable<
                 Maybe> || __maybe_view_detail::__wrapped_nullable<Maybe>)
class maybe_view : public std::ranges::view_interface<maybe_view<Maybe>> {
    using T = std::remove_reference_t<
        std::iter_reference_t<typename std::unwrap_reference_t<Maybe>>>;

public:
    maybe_view() = default;

    constexpr explicit maybe_view(const Maybe& maybe)
        : value_(maybe)
    {
    }

    constexpr explicit maybe_view(Maybe&& maybe)
        : value_(std::move(maybe))
    {
    }

    template <typename... Args>
        requires std::constructible_from<Maybe, Args...>
    constexpr maybe_view(std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...)
    {
    }

    constexpr T* begin() noexcept
    {
        return data();
    }

    constexpr const T* begin() const noexcept
    {
        return data();
    }

    constexpr T* end() noexcept
    {
        return data() + size();
    }

    constexpr const T* end() const noexcept
    {
        return data() + size();
    }

    constexpr std::size_t size() noexcept
    {
        Maybe& m = *value_;
        if constexpr (__maybe_view_detail::__is_reference_wrapper_v<Maybe>) {
            return bool(m.get());
        } else {
            return bool(m);
        }
    }

    constexpr std::size_t size() const noexcept
    {
        const Maybe& m = *value_;
        if constexpr (__maybe_view_detail::__is_reference_wrapper_v<Maybe>) {
            return bool(m.get());
        } else {
            return bool(m);
        }
    }

    constexpr T* data() noexcept
    {
        Maybe& m = *value_;
        if constexpr (__maybe_view_detail::__is_reference_wrapper_v<Maybe>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

    constexpr const T* data() const noexcept
    {
        const Maybe& m = *value_;
        if constexpr (__maybe_view_detail::__is_reference_wrapper_v<Maybe>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

private:
    __detail::__copyable_box<Maybe> value_;
};

namespace views {
    class __maybe_fn : public range_adaptor_closure<__maybe_fn> {
    public:
        template <typename Maybe>
        constexpr auto operator()(Maybe&& maybe) const
            noexcept(noexcept(maybe_view(std::forward<Maybe>(maybe))))
                -> decltype(maybe_view(std::forward<Maybe>(maybe)))
        {
            return maybe_view(std::forward<Maybe>(maybe));
        }
    };

    inline constexpr __maybe_fn maybe {};
}
}

namespace iris {
namespace views = ranges::views;
}
