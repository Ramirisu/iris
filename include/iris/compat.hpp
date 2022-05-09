#pragma once

#if defined(__cpp_lib_ranges_zip) && __cpp_lib_ranges_zip >= 202110L
#else

#include <tuple>
#include <type_traits>
#include <utility>

namespace std {

template <class T1,
          class T2,
          class U1,
          class U2,
          template <class>
          class TQual,
          template <class>
          class UQual>
    requires requires
    {
        typename pair<common_reference_t<TQual<T1>, UQual<U1>>,
                      common_reference_t<TQual<T2>, UQual<U2>>>;
    }
struct basic_common_reference<pair<T1, T2>, pair<U1, U2>, TQual, UQual> {
    using type = pair<common_reference_t<TQual<T1>, UQual<U1>>,
                      common_reference_t<TQual<T2>, UQual<U2>>>;
};

template <class T1, class T2, class U1, class U2>
    requires requires
    {
        typename pair<common_type_t<T1, U1>, common_type_t<T2, U2>>;
    }
struct common_type<pair<T1, T2>, pair<U1, U2>> {
    using type = pair<common_type_t<T1, U1>, common_type_t<T2, U2>>;
};

template <class... Ts,
          class... Us,
          template <class>
          class TQual,
          template <class>
          class UQual>
    requires requires
    {
        typename tuple<common_reference_t<TQual<Ts>, UQual<Us>>...>;
    }
struct basic_common_reference<tuple<Ts...>, tuple<Us...>, TQual, UQual> {
    using type = tuple<common_reference_t<TQual<Ts>, UQual<Us>>...>;
};

template <class... Ts, class... Us>
    requires requires
    {
        typename tuple<common_type_t<Ts, Us>...>;
    }
struct common_type<tuple<Ts...>, tuple<Us...>> {
    using type = tuple<common_type_t<Ts, Us>...>;
};

}

#endif
