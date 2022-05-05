#pragma once

#if defined(__cpp_lib_ranges_zip) && __cpp_lib_ranges_zip >= 202110L
// already implemented basic_common_reference partial specialization for pair
// and tuple.
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
struct basic_common_reference<std::pair<T1, T2>,
                              std::pair<U1, U2>,
                              TQual,
                              UQual> {
    using type = pair<common_reference_t<TQual<T1>, UQual<U1>>,
                      common_reference_t<TQual<T2>, UQual<U2>>>;
};

template <class... Ts,
          class... Us,
          template <class>
          class TQual,
          template <class>
          class UQual>
    requires requires
    {
        typename tuple<std::common_reference_t<TQual<Ts>, UQual<Us>>...>;
    }
struct basic_common_reference<tuple<Ts...>, tuple<Us...>, TQual, UQual> {
    using type = tuple<common_reference_t<TQual<Ts>, UQual<Us>>...>;
};

}

#endif
