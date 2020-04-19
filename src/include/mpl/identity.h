#pragma once

#include <mpl/config.h>


namespace DDS_ROOT_NAMESPACE {
namespace DDS_MPL_NAMESPACE {

struct identity_t {
   template <typename T>
   decltype(auto) operator()(T &&t) const {
      return static_cast<T &&>(t);
   }
};

constexpr identity_t identity{};

inline void avoid_unused_identity() { (void)identity; }

} // namespace DDS_MPL_NAMESPACE
} // namespace DDS_ROOT_NAMESPACE
