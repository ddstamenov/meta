#include <mpl/identity.h>
#include <test_framework/tiny_framework.h>

#include <cstring>

using namespace dds;
using namespace dds::mpl;

TESTS_BEGIN()

TEST_CASE(ByValue) {
   String s{"value"};
   int i{23};
   char arr[] = "string";
   static_assert(std::is_same<decltype(arr), char[7]>::value, "type mismatch");
   auto ptr = "something";
   static_assert(std::is_same<decltype(ptr), const char *>::value, "type mismatch");

   auto is = identity(s);
   static_assert(std::is_same<decltype(is), String>::value, "type mismatch");
   TEST_CHECK_EQUAL(s, is);

   auto ii = identity(i);
   static_assert(std::is_same<decltype(ii), int>::value, "type mismatch");
   TEST_CHECK_EQUAL(i, ii);

   auto iarr = identity(arr);
   static_assert(std::is_same<decltype(iarr), char *>::value, "type mismatch");
   TEST_CHECK_EQUAL(0, strcmp(arr, iarr));

   auto iptr = identity(ptr);
   static_assert(std::is_same<decltype(iptr), const char *>::value, "type mismatch");
   TEST_CHECK_EQUAL(0, strcmp(ptr, iptr));
}

TEST_CASE(GetByLRef) {
   String s{"value"};
   int i{23};
   char arr[] = "string";
   static_assert(std::is_same<decltype(arr), char[7]>::value, "type mismatch");
   auto &ptr = "something";
   static_assert(std::is_same<decltype(ptr), const char(&)[10]>::value, "type mismatch");

   auto &is = identity(s);
   static_assert(std::is_same<decltype(is), String &>::value, "type mismatch");
   TEST_CHECK_EQUAL(s, is);

   const auto &is2 = identity(std::move(s));
   static_assert(std::is_same<decltype(is2), const String &>::value, "type mismatch");
   TEST_CHECK_EQUAL("value", is);

   auto &ii = identity(i);
   static_assert(std::is_same<decltype(ii), int &>::value, "type mismatch");
   TEST_CHECK_EQUAL(i, ii);

   auto &iarr = identity(arr);
   static_assert(std::is_same<decltype(iarr), char(&)[7]>::value, "type mismatch");
   TEST_CHECK_EQUAL(0, strcmp(arr, iarr));

   auto &iptr = identity(ptr);
   static_assert(std::is_same<decltype(iptr), const char(&)[10]>::value, "type mismatch");
   TEST_CHECK_EQUAL(0, strcmp(ptr, iptr));
}

template <typename T>
struct err;

TEST_CASE(GetByGenericRef) {
   String s{"value"};
   int i{23};
   char arr[] = "string";
   static_assert(std::is_same<decltype(arr), char[7]>::value, "type mismatch");
   auto &ptr = "something";
   static_assert(std::is_same<decltype(ptr), const char(&)[10]>::value, "type mismatch");

   auto &&is = identity(s);
   static_assert(std::is_same<decltype(is), String &>::value, "type mismatch");
   TEST_CHECK_EQUAL(s, is);
   auto &&is2 = identity(std::move(s));
   static_assert(std::is_same<decltype(is2), String &&>::value, "type mismatch");
   TEST_CHECK_EQUAL("value", is);

   auto &&ii = identity(i);
   static_assert(std::is_same<decltype(ii), int &>::value, "type mismatch");
   TEST_CHECK_EQUAL(i, ii);

   auto &&ii2 = identity(42);
   static_assert(std::is_same<decltype(ii2), int &&>::value, "type mismatch");
   // ii2 cannot be accessed but it is useful to check its type

   auto &&iarr = identity(arr);
   static_assert(std::is_same<decltype(iarr), char(&)[7]>::value, "type mismatch");
   TEST_CHECK_EQUAL(0, strcmp(arr, iarr));

   auto &&iptr = identity(ptr);
   static_assert(std::is_same<decltype(iptr), const char(&)[10]>::value, "type mismatch");
   TEST_CHECK_EQUAL(0, strcmp(ptr, iptr));
}

TESTS_END()
