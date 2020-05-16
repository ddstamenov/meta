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

struct type1_t {};

int foo(double) {
   TEST_MESSAGE("void foo(double) is called");
   return 1;
}

int foo(String) {
   TEST_MESSAGE("void foo(String) is called");
   return 2;
}

int foo(type1_t) {
   TEST_MESSAGE("void foo(type1_t) is called");
   return 3;
}

template <int val>
struct complex_t {
   using type1 = double;
   using type2 = String;

   static constexpr int value() { return val; }
};

template <typename T>
auto func_return_different_types_depend_on_template(T in) {
   /*
    * Note: the following implementation is not allowed - have different returned types
    * if(T::value() >5) {
    *    return typename decltype(in)::type1{42};
    * } else {
    *    return typename decltype(in)::type2{"string-value"};
    * }
    */
   auto tcb = [&]() { return typename decltype(in)::type1{42}; };
   auto fcb = [&]() { return typename decltype(in)::type2{"string-value"}; };
   auto cond = bool_t<(T::value() > 5)>{};
   return if_static(cond, tcb, fcb)();
};

TEST_CASE(tricky) {

   /*
    * condition used in execute if complex_t `val` > 5
    */
   constexpr complex_t<7> complex1{};
   constexpr complex_t<3> complex2{};

   double d = func_return_different_types_depend_on_template(complex1);
   TEST_CHECK_EQUAL(42, d);

   String s = func_return_different_types_depend_on_template(complex2);
   TEST_CHECK_EQUAL("string-value", s);

   int i1 = foo(func_return_different_types_depend_on_template(complex1));
   TEST_CHECK_EQUAL(1, i1);

   int i2 = foo(func_return_different_types_depend_on_template(complex2));
   TEST_CHECK_EQUAL(2, i2);
}

TEST_SUITE_END() // ifstaticTests
