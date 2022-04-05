
# IRIS

[![Linux CI](https://github.com/Ramirisu/iris/actions/workflows/linux.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/linux.yml)
[![Windows CI](https://github.com/Ramirisu/iris/actions/workflows/windows.yml/badge.svg)](https://github.com/Ramirisu/iris/actions/workflows/windows.yml)

## Feature

* Range Adaptors
  * `ranges::base64_encode_view<Range, Binary, Text>`
  * `ranges::base64_decode_view<Range, Binary, Text>`
  * `ranges::to_utf_view<Range, Unicode, UTF>`
  * `ranges::from_utf_view<Range, Unicode, UTF>`
* Range Utilities
  * `ranges::to`
  * `ranges::elements_of`
* Range Adaptor Objects
  * `views::base64_encode`
  * `views::base64_decode`
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
* Utilities
  * `expected<T, E>`
  * `semver`
  * `on_scope_exit`
  * `on_scope_failure`
  * `on_scope_success`
  * `to_underlying`
