#include <common/common.h>
#include <mpl/exec_if.h>
#include <mpl/identity.h>
#include <test_framework/tiny_framework.h>

using namespace dds;
using namespace dds::mpl;

TESTS_BEGIN()

TEST_SUITE_BEGIN(execifTests)

// usage of exec_if to implement compile time branching
struct Test {
   template <typename T>
   static String to_str(const T &in) {
      auto strCb = [&](auto _) { return _(in); };
      auto otherCb = [&](auto _) { return std::to_string(_(in)); };
      bool_t<std::is_same<String, T>::value> cond{};
      return exec_if(cond, strCb, otherCb);
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
