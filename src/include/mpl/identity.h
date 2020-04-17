#pragma once

#include <mpl/config.h>

MPL_NAMESPACE_START

struct identity_t {
   template<typename T>
   decltype(auto) operator()(T&& t){
      return static_cast<T&&>(t);
   }
};

constexpr identity_t identity{};


inline void avoid_unused_identity() { (void) identity; }

MPL_NAMESPACE_END

