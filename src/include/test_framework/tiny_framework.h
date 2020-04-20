#pragma once

/*
 * Tiny test framework.
 * It allows writing a unittests, do checks and report results.
 *
 * Here is basic example how to use it:
 *
 * #include <test_framework/tiny_framework.h>
 *
 * // begin tests. It should be used only once per binary.
 * TESTS_BEGIN()
 *
 * // add test suite
 * TEST_SUITE_BEGIN(suite_name1)
 *
 * // add test case
 * TEST_CASE(case_name1) {
 *    std::string s1{"one"};
 *    std::string s2{"two"};
 *    TEST_CHECK(s1 == s2);
 *    TEST_CHECK_EQUAL(s1,s2);
 * }
 *
 * TEST_SUITE_BEGIN(suite_name2) // nested suite inside suite_name1
 *
 * TEST_CASE(case_name2) {
 *    int *p = nullptr;
 *    TEST_REQUIRE(p); // this check will fail and will interrupt tast case
 *
 *    TEST_CHECK_EQUAL(5, *p); // this check will never be called
 * }
 *
 * TEST_SUITE_END() // end of suite  suite_name2
 *
 * TEST_SUITE_END() // end of suite  suite_name1
 *
 *
 */

#include <common/common.h>
#include <string>
#include <test_framework/config.h>
#include <test_framework/tools.h>

#include <functional>
#include <list>
#include <sstream>

namespace DDS_ROOT_NAMESPACE {
namespace DDS_TINYTEST_NAMESPACE {

struct __config_t;

// only declaration
__config_t &__get_config();

enum __check_return_e { __check_ok, __check_err };

using __test_report_cb_t = std::function<void(__check_return_e)>;
using __test_cb_t = std::function<void(const __config_t &, __test_report_cb_t)>;
using __test_info_t = std::pair<String /*name*/, __test_cb_t>;
using __tests_t = std::list<__test_info_t>;
using __list_suites_t = std::list<String>;

struct __static_test_object_t {
   __tests_t tests;
   __list_suites_t suites;
   const String test_separator{"/"};
};

// only declaration
__static_test_object_t &__get_sobject();

struct __config_t {
   enum Level {
      ERROR,          // print only error
      MESSAGE,        // print TEST_MESSAGE
      TEST_CASE_NAME, // print called test-name
      ALL,            // print also postive checks
   };

   bool parse_args(int argc, char **argv) {
      if (argc < 2) {
         return true;
      }
      for (int i = 1; i < argc; ++i) {
         StringView opt{argv[i]};
         if ("--help" == opt) {
            print_help(argv[0]);
            return false;
         } else if ("--log_level=message" == opt) {
            level = MESSAGE;
         } else if ("--log_level=testnames" == opt) {
            level = TEST_CASE_NAME;
         } else if ("--log_level=all" == opt) {
            level = ALL;
         } else if ("--log_level=error" == opt) {
            level = ERROR;
         } else {
            print_help(argv[0]);
            return false;
         }
      }
      return true;
   }

   void trace(Level type, const String &msg) const {
      if (type > level) {
         return;
      }
      println(msg);
   }

   int run_tests(const __static_test_object_t &obj) const {
      int count_test_cases{};
      unsigned checks{};
      unsigned errors{};
      auto test_report_cb = [&checks, &errors](__check_return_e value) {
         switch (value) {
         case __check_err: ++errors; break;
         case __check_ok: ++checks; break;
         default: DdsVerify(!"Unhandled case");
         }
      };
      for (auto &test : obj.tests) {
         trace(TEST_CASE_NAME, "Enter: " + test.first);
         if (filter(test)) {
            auto stored_checks = checks;
            auto stored_errors = errors;
            test.second(*this, test_report_cb);
            ++count_test_cases;
            auto ran_checks = checks - stored_checks;
            auto ran_errors = errors - stored_errors;
            if (ran_checks == 0 && ran_errors == 0) {
               trace(MESSAGE,
                     "[warning] Test case " + test.first + " doesn't check anything");
            } else {
               trace(ALL,
                     "[info] Test case " + test.first + " ran " +
                        std::to_string(ran_checks + ran_errors) + " cheks (" +
                        std::to_string(ran_errors) + " of them failed)");
            }
         }
         trace(TEST_CASE_NAME, "Leave: " + test.first);
      }
      String errors_report;
      if (errors) {
         errors_report = std::to_string(errors) + " checks failed.";
      }
      std::cerr << "*** run " << count_test_cases << " tests. " << errors_report << "\n";
      return 0;
   }

   static void print_help(const char *name) {
      std::cerr << "Usage:\n"
                << name << "\n --log_level=[error/message/testnames/all]\n"
                << " --help (print this help message)\n";
   }

   bool filter(const __test_info_t &test) const {
      (void)test.first; // TODO: add filtering by name
      return true;
   }

   Level level{ERROR};
};

struct __add_remove_suite_t {
   __add_remove_suite_t(__static_test_object_t &obj, const String &name) {
      obj.suites.emplace_back(name);
   }
   __add_remove_suite_t(__static_test_object_t &obj) { obj.suites.pop_back(); }
};

struct __add_test_t {
   template <typename test_info>
   __add_test_t(__static_test_object_t &obj, test_info test) {
      obj.tests.emplace_back(test.__test_name, test);
   }
};

/*
 * This line defines a `int main()` function and should be used only once for binary.
 * When used is should be the first macro used from test file.
 */
#define TESTS_BEGIN()                                                                    \
   ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__static_test_object_t                  \
      & ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_sobject() {                  \
      static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__static_test_object_t        \
         sobject;                                                                        \
      return sobject;                                                                    \
   }                                                                                     \
   ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__config_t                              \
      & ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_config() {                   \
      static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__config_t scfg;              \
      return scfg;                                                                       \
   }                                                                                     \
   int main(int argc, char **argv) {                                                     \
      auto &__cfg = ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_config();        \
      if (!__cfg.parse_args(argc, argv)) {                                               \
         return 1;                                                                       \
      }                                                                                  \
      return __cfg.run_tests(                                                            \
         ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_sobject());                 \
   }

/*
 * Begin test suite with given `name`
 * used to group test cases.
 * Test cases with same names can appear if they are in different suites.
 */
#define TEST_SUITE_BEGIN(name)                                                           \
   namespace name {                                                                      \
   static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__add_remove_suite_t             \
      __suite_##name{::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_sobject(),      \
                     #name};

/*
 * end of current suite
 */
#define TEST_SUITE_END()                                                                 \
   static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__add_remove_suite_t             \
      __suite_end_##name{::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_sobject()}; \
   }

/*
 * define a test case with `name`.
 * It can define a variables, call expressions (including test expressions as TEST_CHECK,
 * TEST_REQUIRE, TEST_CHECK_EQUAL, etc ...
 */
#define TEST_CASE(name)                                                                  \
   struct __type_case_##name {                                                           \
      __type_case_##name(                                                                \
         const ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__static_test_object_t      \
            &obj) {                                                                      \
         ::DDS_ROOT_NAMESPACE::String calc;                                              \
         for (const auto &suite : obj.suites) {                                          \
            calc += suite;                                                               \
            calc += obj.test_separator;                                                  \
         }                                                                               \
         __test_name = calc + #name;                                                     \
      }                                                                                  \
      void operator()(const ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__config_t &,  \
                      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__test_report_cb_t); \
      ::DDS_ROOT_NAMESPACE::String __test_name;                                          \
      std::list<::DDS_ROOT_NAMESPACE::String> __list_info;                               \
   };                                                                                    \
   static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__add_test_t case_##name{        \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_sobject(),                     \
      __type_case_##name{                                                                \
         ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_sobject()}};                \
   void __type_case_##name::operator()(                                                  \
      const ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__config_t &__cfg,             \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__test_report_cb_t __test_report_cb)

/*
 * Internal defined used from this framework
 */
#define TEST_BASE_CHECK(stop_on_error, expr)                                             \
   if (expr) {                                                                           \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      __test_report_cb(__check_ok);                                                      \
      String __log = String{"Ok: '"} + #expr + String{"' passed"};                       \
      __cfg.trace(__config_t::ALL, __log);                                               \
      __list_info.clear();                                                               \
   } else {                                                                              \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      __test_report_cb(__check_err);                                                     \
      String sExpr = String{"'"} + #expr + String{"'"};                                  \
      std::stringstream __strm;                                                          \
      __strm << "[error] " << (stop_on_error ? "(required check)" : "") << __test_name   \
             << " File: " __FILE__ << ":" << __LINE__ << " " << sExpr << " failed";      \
      String __log = __strm.str();                                                       \
      for (const auto &info : __list_info) {                                             \
         __cfg.trace(__config_t::ERROR, "   Failed in context:" + info);                 \
      }                                                                                  \
      __list_info.clear();                                                               \
      __cfg.trace(__config_t::ERROR, __log);                                             \
      if (stop_on_error) {                                                               \
         return;                                                                         \
      }                                                                                  \
   }

/*
 * Internal defined used from this framework
 */
#define TEST_BASE_EQUAL(stop_on_error, lhs, rhs)                                         \
   if (lhs == rhs) {                                                                     \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      __test_report_cb(__check_ok);                                                      \
      String __log = String{"Ok: '"} + #lhs + String{"="} + #rhs + String{"' passed"};   \
      __cfg.trace(__config_t::ALL, __log);                                               \
      __list_info.clear();                                                               \
   } else {                                                                              \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      __test_report_cb(__check_err);                                                     \
      std::stringstream __strm;                                                          \
      __strm << "[error] " << (stop_on_error ? "(required check)" : "") << __test_name   \
             << " File: " __FILE__ << ":" << __LINE__ << " " #lhs "==" #rhs " (failed)"  \
             << "[`" << lhs << "` != `" << rhs << "`]";                                  \
      String __log = __strm.str();                                                       \
      for (const auto &info : __list_info) {                                             \
         __cfg.trace(__config_t::ERROR, "   Failed in context:" + info);                 \
      }                                                                                  \
      __list_info.clear();                                                               \
      __cfg.trace(__config_t::ERROR, __log);                                             \
      if (stop_on_error) {                                                               \
         return;                                                                         \
      }                                                                                  \
   }

/*
 * Check 'expr' and if false, report an error.
 * Test case will continue after it
 */
#define TEST_CHECK(expr) TEST_BASE_CHECK(false, expr)

/*
 * Check 'expr' and if false, report an error.
 * Test case will stop if fails, which can be used if someone requires expression and
 * there is no need to check it himself.
 * Example:
 *  object_t *ptr = some_function_ruturning_ptr();
 *  TEST_REQUIRE(ptr);
 *  ptr->method(); // no need to check ptr if REQUIRE is already used.
 */
#define TEST_REQUIRE(expr) TEST_BASE_CHECK(true, expr)

/*
 * Check if `lhs` and `rhs` are equals. Report an error if they are not. Test case will
 * continue after it
 */
#define TEST_CHECK_EQUAL(lhs, rhs) TEST_BASE_EQUAL(false, lhs, rhs)

/*
 * Check if `lhs` and `rhs` are equals. Report an error if they are not. Test case will
 * be terminated if this check fails.
 */
#define TEST_REQUIRE_EQUAL(lhs, rhs) TEST_BASE_EQUAL(true, lhs, rhs)

/*
 * this will print `msg` if `value` of log_level=<value> is greater or equal to message
 */
#define TEST_MESSAGE(msg)                                                                \
   ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__get_config().trace(                   \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::__config_t::MESSAGE, msg);

/*
 * passed `msg` will be printed if next check (TEST_CHECK, TEST_CHECK_EQUAL, ...) fails.
 * Every check will reset info.
 */
#define TEST_INFO(msg) __list_info.emplace_back(msg);

} // namespace DDS_TINYTEST_NAMESPACE
} // namespace DDS_ROOT_NAMESPACE
