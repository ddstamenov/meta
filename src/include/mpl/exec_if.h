#pragma once

#include <mpl/config.h>
#include <mpl/identity.h>
#include <mpl/if_static.h>

#include <type_traits>

namespace DDS_ROOT_NAMESPACE {
namespace DDS_MPL_NAMESPACE {

struct exec_if_t {
   template <typename Cond, typename T, typename F>
   decltype(auto) operator()(Cond cond, T &&t, F &&f) const {
      return if_static(cond, static_cast<T &&>(t), static_cast<F &&>(f))(identity);
   }
};

constexpr exec_if_t exec_if{};

} // namespace DDS_MPL_NAMESPACE
} // namespace DDS_ROOT_NAMESPACE
