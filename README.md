
# IRIS

[![Linux CI](https://github.com/Ramirisu/iris/actions/workflows/linux.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/linux.yml)
[![Windows CI](https://github.com/Ramirisu/iris/actions/workflows/windows.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/windows.yml)

## Feature

* Range Adaptors
  * `ranges::to_base64_view<Range, Binary, Text>`
  * `ranges::from_base64_view<Range, Binary, Text>`
  * `ranges::to_utf_view<Range, Unicode, UTF>`
  * `ranges::from_utf_view<Range, Unicode, UTF>`
* Range Utilities
  * `ranges::to`
  * `ranges::elements_of`
* Range Adaptor Objects
  * `views::to_base64`
  * `views::from_base64`
  * `views::to_utf8_char`
  * `views::to_utf8`
  * `views::to_utf16`
  * `views::to_utf32`
  * `views::from_utf`
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
  * `is_specialized_of`
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
