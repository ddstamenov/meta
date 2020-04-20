#include <test_framework/tiny_framework.h>

TEST_SUITE_BEGIN(suite_part1)

TEST_SUITE_BEGIN(suite_nested)

TEST_CASE(case1) {
   TEST_CHECK(true);
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
