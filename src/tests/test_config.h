#pragma once

#include <iostream>
#include <string>
#include <initializer_list>


#define TEST_CHECK(expr) \
   if(expr) {println("'"#expr"'", "passed");} \
   else { println("'"#expr"'", "failed", __FILE__, __LINE__);}
