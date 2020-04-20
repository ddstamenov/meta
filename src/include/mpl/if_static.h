#pragma once

#include <mpl/config.h>

#include <type_traits>

namespace DDS_ROOT_NAMESPACE {
namespace DDS_MPL_NAMESPACE {

template <bool value>
using bool_t = std::integral_constant<bool, value>;

using true_t = bool_t<true>;
using false_t = bool_t<false>;

namespace detail {
struct if_static_impl_t {
   template <typename T, typename F>
   static auto apply(true_t, T &&t, F &&) {
      return t;
   }

   template <typename T, typename F>
   static auto apply(false_t, T &&, F &&f) {
      return f;
   }
};
} // namespace detail

struct if_static_t {
   template <typename Cond, typename T, typename F>
   decltype(auto) operator()(Cond cond, T &&t, F &&f) const {
      return detail::if_static_impl_t::apply(
         cond, static_cast<T &&>(t), static_cast<F &&>(f));
   }
};

constexpr if_static_t if_static{};

inline void avoid_unused_if_static() { (void)if_static; }

} // namespace DDS_MPL_NAMESPACE
} // namespace DDS_ROOT_NAMESPACE
