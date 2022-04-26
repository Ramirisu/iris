
# IRIS

[![Linux CI](https://github.com/Ramirisu/iris/actions/workflows/linux.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/linux.yml)
[![Windows CI](https://github.com/Ramirisu/iris/actions/workflows/windows.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/windows.yml)

## Supported Compilers

* Visual Studio 2022 19.1 (`/std:latest`)
* Visual Studio 2022 19.2 (`/std:c++20`)
* GCC 11.2 (`-std=c++20`)

## Feature

* Range Adaptors
  * `ranges::join_with_view<Range, Pattern>` (***C++23***)
  * `ranges::zip_view<Ranges...>` (***C++23***)
  * `ranges::zip_transform_view<Fn, Ranges...>` (***C++23***)
  * `ranges::adjacent_view<Range, N>` (***C++23***)
  * `ranges::adjacent_transform_view<Range, Fn, N>` (***C++23***)
  * `ranges::chunk_by_view<Range, Pred>` (***C++23***)
  * `ranges::chunk_view<Range>` (***C++23***)
  * `ranges::slide_view<Range>` (***C++23***)
  * `ranges::repeat_view<Value, Bound>` (***C++23***)
  * `ranges::stride_view<Range>` (***C++23***)
  * `ranges::as_rvalue_view<Range>` (***C++23***)
  * `ranges::cartesian_product_view<Ranges...>` (***C++23***)
  * `ranges::enumerate_view<Range>` (***C++26 ?***)
  * `ranges::to_base64_view<Range, Binary, Text>`
  * `ranges::from_base64_view<Range, Binary, Text>`
  * `ranges::to_utf_view<Range, Unicode, UTF>`
  * `ranges::from_utf_view<Range, Unicode, UTF>`
* Range Adaptor Objects
  * `views::join_with` (***C++23***)
  * `views::zip` (***C++23***)
  * `views::zip_transform` (***C++23***)
  * `views::adjacent<N>` (***C++23***)
  * `views::pairwise` (***C++23***)
  * `views::adjacent_transform<N>` (***C++23***)
  * `views::pairwise_transform` (***C++23***)
  * `views::chunk_by` (***C++23***)
  * `views::chunk` (***C++23***)
  * `views::slide` (***C++23***)
  * `views::repeat` (***C++23***)
  * `views::stride` (***C++23***)
  * `views::as_rvalue` (***C++23***)
  * `views::cartesian_product` (***C++23***)
  * `views::enumerate` (***C++26 ?***)
  * `views::unwrap`
  * `views::to_base64`
  * `views::from_base64`
  * `views::to_utf<UTF>`
  * `views::from_utf`
* Range Utilities
  * `ranges::to` (***C++23***)
  * `ranges::elements_of` (***C++23***)
* Range Algorithms
  * `ranges::iota` (***C++23***)
  * `ranges::shift_left` (***C++23***)
  * `ranges::shift_right` (***C++23***)
  * `ranges::starts_with` (***C++23***)
  * `ranges::ends_with` (***C++23***)
  * `ranges::contains` (***C++23***)
  * `ranges::contains_subrange` (***C++23***)
  * `ranges::fold_left` (***C++23***)
  * `ranges::fold_left_first` (***C++23***)
  * `ranges::fold_right` (***C++23***)
  * `ranges::fold_right_last` (***C++23***)
* Coroutine Types
  * `generator<R, V, Allocator>` (***C++23 Partial***)
  * `lazy<T>` (***C++26 ?***)
* Encoding
  * `base64<Binary, Text>`
  * `utf<Unicode, UTF>`
* Type Traits
  * `is_scoped_enum` (***C++23***)
  * `is_specialization_of`
  * `front_of`
  * `back_of`
* Utilities
  * `expected<T, E>` (***C++23***)
  * `out_ptr` (***C++23***)
  * `inout_ptr` (***C++23***)
  * `bind_front` (***C++20***)
  * `bind_back` (***C++23***)
  * `to_underlying` (***C++23***)
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
