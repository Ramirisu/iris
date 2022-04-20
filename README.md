
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
  * `ranges::chunk_by_view<Range, Pred>`
  * `ranges::chunk_view<Range, Size>`
  * `ranges::slide_view<Range, Size>`
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
  * `views::chunk_by`
  * `views::chunk`
  * `views::slide`
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
