#include <test_framework/tiny_framework.h>

// define only in this file
TESTS_BEGIN()

TEST_SUITE_BEGIN(suite_part2)

bool bar(bool in) {
   // call test message outside of test case
   TEST_MESSAGE("called 'bool bar(bool);'");
   return in;
}

TEST_CASE(case3) {
   TEST_CHECK(bar(true));
   std::string s1{"same"};
   std::string s2{"same"};
   TEST_CHECK_EQUAL(s1, s2);
}

TEST_CASE(case4) { TEST_CHECK(true); }

TEST_SUITE_END()
