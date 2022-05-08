#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <cstddef>

namespace iris::__detail {

template <typename T, std::size_t N>
class __static_storage {
public:
    __static_storage() = default;

    template <typename... Ts>
    constexpr __static_storage(Ts&&... ts) requires(pack_size_v<Ts...> <= N)
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

    friend constexpr bool operator==(const __static_storage& lhs,
                                     const __static_storage& rhs)
        = default;

private:
    T data_[N];
    std::size_t size_;
};

}
