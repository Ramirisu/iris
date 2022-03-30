#pragma once

#include <iris/config.hpp>

#include <cstddef>

namespace iris::detail {

template <typename T, std::size_t N>
class static_storage {
public:
    static_storage() = default;

    template <typename... Ts>
    constexpr static_storage(Ts&&... ts) requires(sizeof...(Ts) <= N)
        : data_ { static_cast<T>(std::forward<Ts>(ts))... }
        , size_(sizeof...(Ts))
    {
    }

    constexpr const T& operator[](std::size_t index) const noexcept
    {
        IRIS_ASSERT(index <= size_);
        return data_[index];
    }

    constexpr auto size() const noexcept
    {
        return size_;
    }

private:
    T data_[N];
    std::size_t size_;
};

}
