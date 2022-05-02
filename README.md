
# IRIS

[![Linux CI](https://github.com/Ramirisu/iris/actions/workflows/linux.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/linux.yml)
[![Windows CI](https://github.com/Ramirisu/iris/actions/workflows/windows.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/windows.yml)

## Integration

CMake

```
# clone into your project's subdirectory
add_subdirectory(path/to/iris)
add_executable(your-exe ...)
target_link_libraries(your-exe PRIVATE iris)
```

## Supported Compilers

* Visual Studio 2022 19.1 (`/std:latest`)
* Visual Studio 2022 19.2 (`/std:c++20`)
* GCC 11.2 (`-std=c++20`)

## Feature

* Range Adaptors
  * `ranges::join_with_view<Range, Pattern>` ([P2441R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2441r1.html))
  * `ranges::zip_view<Ranges...>` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `ranges::zip_transform_view<Fn, Ranges...>` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `ranges::adjacent_view<Range, N>` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `ranges::adjacent_transform_view<Range, Fn, N>` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `ranges::chunk_by_view<Range, Pred>` ([P2443R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2443r1.html))
  * `ranges::chunk_view<Range>` ([P2442R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2442r1.html))
  * `ranges::slide_view<Range>` ([P2442R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2442r1.html))
  * `ranges::repeat_view<Value, Bound>` ([P2474R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2474r1.html))
  * `ranges::stride_view<Range>` ([P1899R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1899r2.html))
  * `ranges::as_rvalue_view<Range>` ([P2446R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2446r2.html))
  * `ranges::cartesian_product_view<Ranges...>` ([P2374R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2374r3.html))
  * `ranges::enumerate_view<Range>` ([P2164R5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2164r5.pdf))
  * `ranges::concat_view<Ranges...>` ([P2542R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2542r1.html))
  * `ranges::unwrap_view<Range>`
  * `ranges::to_base64_view<Range, Binary, Text>`
  * `ranges::from_base64_view<Range, Binary, Text>`
  * `ranges::to_utf_view<Range, Unicode, UTF>`
  * `ranges::from_utf_view<Range, Unicode, UTF>`
* Range Adaptor Objects
  * `views::join_with` ([P2441R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2441r1.html))
  * `views::zip` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `views::zip_transform` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `views::adjacent<N>` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `views::pairwise` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `views::adjacent_transform<N>` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `views::pairwise_transform` ([P2321R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2321r2.html))
  * `views::chunk_by` ([P2443R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2443r1.html))
  * `views::chunk` ([P2442R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2442r1.html))
  * `views::slide` ([P2442R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2442r1.html))
  * `views::repeat` ([P2474R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2474r1.html))
  * `views::stride` ([P1899R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1899r2.html))
  * `views::as_rvalue` ([P2446R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2446r2.html))
  * `views::cartesian_product` ([P2374R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2374r3.html))
  * `views::enumerate` ([P2164R5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2164r5.pdf))
  * `views::concat` ([P2542R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2542r1.html))
  * `views::unwrap`
  * `views::to_base64`
  * `views::from_base64`
  * `views::to_utf<UTF>`
  * `views::from_utf`
* Range Utilities
  * `ranges::to` ([P1206R7](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1206r7.pdf))
  * `ranges::elements_of`
* Range Algorithms
  * `ranges::iota` ([P2440R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2440r1.html))
  * `ranges::shift_left` ([P2440R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2440r1.html))
  * `ranges::shift_right` ([P2440R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2440r1.html))
  * `ranges::starts_with` ([P1659R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1659r3.html))
  * `ranges::ends_with` ([P1659R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1659r3.html))
  * `ranges::contains` ([P2302R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2302r3.html))
  * `ranges::contains_subrange` ([P2302R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2302r3.html))
  * `ranges::fold_left` ([P2322R5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2322r5.html))
  * `ranges::fold_left_first` ([P2322R5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2322r5.html))
  * `ranges::fold_right` ([P2322R5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2322r5.html))
  * `ranges::fold_right_last` ([P2322R5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2322r5.html))
* Coroutine Types
  * `generator<R, V, Allocator>`
  * `lazy<T>` ([P2506R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2506r0.pdf))
* Encoding
  * `base64<Binary, Text>`
  * `utf<Unicode, UTF>`
* Type Traits
  * `is_scoped_enum` ([P1048R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1048r1.pdf))
  * `is_specialization_of`
  * `front_of`
  * `back_of`
* Utilities
  * `expected<T, E>` ([P0323R12](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html))
  * `out_ptr` ([P1132R8](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1132r8.html))
  * `inout_ptr` ([P1132R8](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1132r8.html))
  * `bind_front`
  * `bind_back` ([P2387R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2387r3.html))
  * `to_underlying` ([P1682R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1682r3.html))
  * `to_unsigned`
  * `to_signed`
  * `semver`
  * `scope_exit`
  * `scope_failure`
  * `scope_success`
* Networking
  * `net::mac_address`
  * `net::ipv4_address`
  * `net::ipv6_address`
  * `net::ip_address`
  * `net::network_interface`
