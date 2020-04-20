#include <common/common.h>
#include <mpl/identity.h>
#include <mpl/if_static.h>
#include <test_framework/tiny_framework.h>

using namespace dds;
using namespace dds::mpl;

TESTS_BEGIN()

TEST_SUITE_BEGIN(ifstaticTests)

TEST_CASE(Basic) {
   using namespace std::literals;
   constexpr bool cond1 = true; // some constexpr variable
   auto ret1 = if_static(bool_t<cond1>{}, "string value"s, 42);
   static_assert(std::is_same<String, decltype(ret1)>::value, "Type mismatch");
   TEST_CHECK_EQUAL("string value", ret1);

   constexpr bool cond2 = false; // some constexpr variable
   auto ret2 = if_static(bool_t<cond2>{}, "string value"s, 42);
   static_assert(std::is_same<int, decltype(ret2)>::value, "Type mismatch");
   TEST_CHECK_EQUAL(42, ret2);
}

// usage of if_static to implement compile time branching
struct Test {
   template <typename T>
   static String to_str(const T &t) {
      auto strCb = [](const auto &s) { return s; };
      auto otherCb = [](const auto &in) { return std::to_string(in); };
      bool_t<std::is_same<String, T>::value> cond{};
      return if_static(cond, strCb, otherCb)(t);
   };
};

TEST_CASE(sfinae) {
   String s{"value"};
   int i{42};

   String ret1 = Test::to_str(s);
   TEST_CHECK_EQUAL("value", ret1);

   String ret2 = Test::to_str(i);
   TEST_CHECK_EQUAL("42", ret2);
}

TEST_SUITE_END() // ifstaticTests
