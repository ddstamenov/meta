#pragma once

#include <initializer_list>
#include <iostream>
#include <string>

#define TEST_CHECK(expr)                                                                 \
   if (expr) {                                                                           \
      println("'" #expr "'", "passed");                                                  \
   } else {                                                                              \
      println("'" #expr "'", "failed", __FILE__, __LINE__);                              \
   }
