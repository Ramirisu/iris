#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <memory>

namespace iris::ranges::__detail {

template <typename T>
    requires std::is_object_v<T>
class __non_propagating_cache {
public:
    constexpr __non_propagating_cache() noexcept {};

    constexpr ~__non_propagating_cache()
    {
        if (has_) {
            value_.~T();
        }
    }

    constexpr __non_propagating_cache(
        const __non_propagating_cache&) noexcept = default;

    constexpr __non_propagating_cache(__non_propagating_cache&& other)
    {
        if (other.has_) {
            other.value_.~T();
            other.has_ = false;
        }
    }

    constexpr __non_propagating_cache&
    operator=(const __non_propagating_cache& other)
    {
        if (std::addressof(other) != this) {
            if (other.has_) {
                other.value_.~T();
                other.has_ = false;
            }
        }

        return *this;
    };

    constexpr __non_propagating_cache&
    operator=(__non_propagating_cache&& other)
    {
        if (has_) {
            value_.~T();
            has_ = false;
        }

        if (other.has_) {
            other.value_.~T();
            other.has_ = false;
        }

        return *this;
    };

    constexpr T& operator*() noexcept
    {
        IRIS_ASSERT(has_);
        return value_;
    }

    constexpr const T& operator*() const noexcept
    {
        IRIS_ASSERT(has_);
        return value_;
    }

    template <typename... Args>
    constexpr T& emplace(Args&&... args)
    {
        if (has_) {
            value_.~T();
            has_ = false;
        }

        std::construct_at(&value_, std::forward<Args>(args)...);
        has_ = true;

        return value_;
    }

private:
    union {
        T value_;
    };

    bool has_ = false;
};

}
