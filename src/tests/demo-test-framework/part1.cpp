#include <test_framework/tiny_framework.h>

TEST_SUITE_BEGIN(suite_part1)

TEST_SUITE_BEGIN(suite_nested)

bool foo(bool in) {
   // call test message outside of test case
   TEST_MESSAGE("called 'bool foo(bool);'");
   return in;
}

TEST_CASE(case1) {
   TEST_CHECK(foo(true));
   TEST_MESSAGE("msg in case1");
   TEST_CHECK_EQUAL(2, 2);
}

TEST_SUITE_END() // end of suite

TEST_CASE(case2) {
   TEST_INFO("msg1");
   TEST_INFO("msg2");
   TEST_CHECK(true);
}

// same name, different suite - OK
TEST_CASE(case1) { TEST_CHECK(true); }

TEST_SUITE_END()
