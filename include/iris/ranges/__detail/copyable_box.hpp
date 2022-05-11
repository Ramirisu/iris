#pragma once

#include <iris/config.hpp>

#include <optional>

namespace iris::ranges::__detail {

template <typename T>
    requires std::copy_constructible<T> && std::is_object_v<T>
class __copyable_box : public std::optional<T> {
public:
    constexpr __copyable_box() //
        noexcept(std::is_nothrow_default_constructible_v<T>) //
        requires std::default_initializable<T>
        : __copyable_box { std::in_place }
    {
    }
    template <typename T2>
        requires(!std::same_as<std::remove_cvref_t<T2>, __copyable_box>)
    constexpr __copyable_box(T2&& value)
        : std::optional<T>(std::forward<T2>(value))
    {
    }

    template <typename... Args>
    constexpr __copyable_box(std::in_place_t, Args&&... args)
        : std::optional<T>(std::in_place, std::forward<Args>(args)...)
    {
    }

    __copyable_box(const __copyable_box&) = default;
    __copyable_box(__copyable_box&&) = default;

    constexpr __copyable_box& operator=(const __copyable_box& other) //
        noexcept(std::is_nothrow_copy_constructible_v<T>) //
        requires(std::copyable<T>)
        = default;

    constexpr __copyable_box& operator=(const __copyable_box& other) //
        noexcept(std::is_nothrow_copy_constructible_v<T>) //
        requires(!std::copyable<T>)
    {
        if (this != std::addressof(other)) {
            if (other)
                this->emplace(*other);
            else
                this->reset();
        }
        return *this;
    }

    constexpr __copyable_box& operator=(__copyable_box&& other) //
        noexcept(std::is_nothrow_move_constructible_v<T>) //
        requires(std::movable<T>)
        = default;

    constexpr __copyable_box& operator=(__copyable_box&& other) //
        noexcept(std::is_nothrow_move_constructible_v<T>) //
        requires(!std::movable<T>)
    {
        if (this != std::addressof(other)) {
            if (other)
                this->emplace(std::move(*other));
            else
                this->reset();
        }
        return *this;
    }
};

}
