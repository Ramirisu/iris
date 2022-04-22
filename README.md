
# IRIS

[![Linux CI](https://github.com/Ramirisu/iris/actions/workflows/linux.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/linux.yml)
[![Windows CI](https://github.com/Ramirisu/iris/actions/workflows/windows.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/windows.yml)

## Supported Compilers

* Visual Studio 2022 19.2 (`/std:c++20`)
* GCC 11.2 (`-std=c++20`)

## Feature

* Range Adaptors
  * `ranges::to_base64_view<Range, Binary, Text>`
  * `ranges::from_base64_view<Range, Binary, Text>`
  * `ranges::to_utf_view<Range, Unicode, UTF>`
  * `ranges::from_utf_view<Range, Unicode, UTF>`
  * `ranges::join_with_view<Range, Pattern>`
  * `ranges::zip_view<Ranges...>`
  * `ranges::zip_transform_view<Fn, Ranges...>`
  * `ranges::adjacent_view<Range, N>`
  * `ranges::adjacent_transform_view<Range, Fn, N>`
  * `ranges::chunk_by_view<Range, Pred>`
  * `ranges::chunk_view<Range>`
  * `ranges::slide_view<Range>`
  * `ranges::repeat_view<Value, Bound>`
  * `ranges::stride_view<Range>`
* Range Utilities
  * `ranges::to`
  * `ranges::elements_of`
* Range Adaptor Objects
  * `views::to_base64`
  * `views::from_base64`
  * `views::to_utf<UTF>`
  * `views::from_utf`
  * `views::join_with`
  * `views::zip`
  * `views::zip_transform`
  * `views::adjacent<N>`
  * `views::pairwise`
  * `views::adjacent_transform<N>`
  * `views::pairwise_transform`
  * `views::chunk_by`
  * `views::chunk`
  * `views::slide`
  * `views::enumerate`
  * `views::repeat`
  * `views::stride`
* Range Algorithms
  * `ranges::iota`
  * `ranges::shift_left`
  * `ranges::shift_right`
  * `ranges::starts_with`
  * `ranges::ends_with`
  * `ranges::contains`
  * `ranges::contains_subrange`
  * `ranges::fold_left`
  * `ranges::fold_left_first`
  * `ranges::fold_right`
  * `ranges::fold_right_last`
* Coroutine Types
  * `lazy<T>`
  * `generator<R, V, Allocator>`
* Networking
  * `net::mac_address`
  * `net::ipv4_address`
  * `net::ipv6_address`
  * `net::ip_address`
  * `net::network_interface`
* Encoding
  * `base64<Binary, Text>`
  * `utf<Unicode, UTF>`
* Type Traits
  * `is_specialization_of`
  * `is_scoped_enum`
  * `front_of`
  * `back_of`
* Utilities
  * `expected<T, E>`
  * `semver`
  * `scope_exit`
  * `scope_failure`
  * `scope_success`
  * `out_ptr`
  * `inout_ptr`
  * `to_underlying`
  * `bind_front`
  * `bind_back`
