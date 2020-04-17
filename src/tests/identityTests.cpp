#include <mpl/identity.h>
#include <test_framework/tools.h>

#include "test_config.h"

#include <iostream>


void test0() {
   PrintT{'_'}(1, 2, 3);
   println(" ");
   PrintLnT{'+'}('a', 3.14, "string");
  TEST_CHECK(true);
  TEST_CHECK(false);
}

int main() {
   test0();

   std::cout << "TODO\n";
}
