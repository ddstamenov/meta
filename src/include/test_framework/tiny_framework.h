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

struct config_t;

enum check_return_e { check_ok, check_err };

using test_report_cb_t = std::function<void(check_return_e)>;
using test_cb_t = std::function<void(const config_t &, test_report_cb_t)>;
using test_info_t = std::pair<String /*name*/, test_cb_t>;
using tests_t = std::list<test_info_t>;
using list_suites_t = std::list<String>;

struct static_test_object_t {
   tests_t tests;
   list_suites_t suites;
   const String test_separator{"/"};
};

// only declaration
static_test_object_t &get_sobject();

struct config_t {
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

   int run_tests(const static_test_object_t &obj) const {
      int count_test_cases{};
      unsigned checks{};
      unsigned errors{};
      auto test_report_cb = [&checks, &errors](check_return_e value) {
         switch (value) {
         case check_err: ++errors; break;
         case check_ok: ++checks; break;
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
      std::cerr << "Usage:\n" << name << "--log_level=[error/message/testnames/all]\n";
   }

   bool filter(const test_info_t &test) const {
      (void)test.first; // TODO: add filtering by name
      return true;
   }

   Level level{ERROR};
};

struct add_remove_suite_t {
   add_remove_suite_t(static_test_object_t &obj, const String &name) {
      obj.suites.emplace_back(name);
   }
   add_remove_suite_t(static_test_object_t &obj) { obj.suites.pop_back(); }
};

struct add_test_t {
   template <typename test_info>
   add_test_t(static_test_object_t &obj, test_info test) {
      obj.tests.emplace_back(test.test_name, test);
   }
};

/*
 * This line defines a `int main()` function and should be used only once for binary.
 * When used is should be the first macro used from test file.
 */
#define TESTS_BEGIN()                                                                    \
   ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::static_test_object_t                    \
      & ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::get_sobject() {                    \
      static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::static_test_object_t sobject; \
      return sobject;                                                                    \
   }                                                                                     \
   int main(int argc, char **argv) {                                                     \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::config_t cfg{};                      \
      if (!cfg.parse_args(argc, argv)) {                                                 \
         return 1;                                                                       \
      }                                                                                  \
      return cfg.run_tests(::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::get_sobject()); \
   }

/*
 * Begin test suite with given `name`
 * used to group test cases.
 * Test cases with same names can appear if they are in different suites.
 */
#define TEST_SUITE_BEGIN(name)                                                           \
   namespace name {                                                                      \
   static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::add_remove_suite_t suite_##name{ \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::get_sobject(), #name};

/*
 * end of current suite
 */
#define TEST_SUITE_END()                                                                 \
   static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::add_remove_suite_t               \
      suite_end_##name{::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::get_sobject()};     \
   }

/*
 * define a test case with `name`.
 * It can define a variables, call expressions (including test expressions as TEST_CHECK,
 * TEST_REQUIRE, TEST_CHECK_EQUAL, etc ...
 */
#define TEST_CASE(name)                                                                  \
   struct type_case_##name {                                                             \
      type_case_##name(                                                                  \
         const ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::static_test_object_t        \
            &obj) {                                                                      \
         ::DDS_ROOT_NAMESPACE::String calc;                                              \
         for (const auto &suite : obj.suites) {                                          \
            calc += suite;                                                               \
            calc += obj.test_separator;                                                  \
         }                                                                               \
         test_name = calc + #name;                                                       \
      }                                                                                  \
      void operator()(const ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::config_t &,    \
                      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::test_report_cb_t);   \
      ::DDS_ROOT_NAMESPACE::String test_name;                                            \
      std::list<::DDS_ROOT_NAMESPACE::String> list_info;                                 \
   };                                                                                    \
   static ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::add_test_t case_##name{          \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::get_sobject(),                       \
      type_case_##name{::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::get_sobject()}};    \
   void type_case_##name::operator()(                                                    \
      const ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::config_t &cfg,                 \
      ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::test_report_cb_t test_report_cb)

/*
 * Internal defined used from this framework
 */
#define TEST_BASE_CHECK(stop_on_error, expr)                                             \
   if (expr) {                                                                           \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      test_report_cb(check_ok);                                                          \
      String log = String{"Ok: '"} + #expr + String{"' passed"};                         \
      cfg.trace(config_t::ALL, log);                                                     \
      list_info.clear();                                                                 \
   } else {                                                                              \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      test_report_cb(check_err);                                                         \
      String sExpr = String{"'"} + #expr + String{"'"};                                  \
      std::stringstream strm;                                                            \
      strm << "[error] " << (stop_on_error ? "(required check)" : "")                    \
           << " File: " __FILE__ << " Line: " << __LINE__ << " " << sExpr << " failed";  \
      String log = strm.str();                                                           \
      for (const auto &info : list_info) {                                               \
         cfg.trace(config_t::ERROR, "   Failed in context:" + info);                     \
      }                                                                                  \
      list_info.clear();                                                                 \
      cfg.trace(config_t::ERROR, log);                                                   \
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
      test_report_cb(check_ok);                                                          \
      String log = String{"Ok: '"} + #lhs + String{"="} + #rhs + String{"' passed"};     \
      cfg.trace(config_t::ALL, log);                                                     \
      list_info.clear();                                                                 \
   } else {                                                                              \
      using namespace ::DDS_ROOT_NAMESPACE;                                              \
      using namespace ::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE;                      \
      test_report_cb(check_err);                                                         \
      std::stringstream strm;                                                            \
      strm << "[error] " << (stop_on_error ? "(required check)" : "")                    \
           << " File: " __FILE__ << " Line: " << __LINE__                                \
           << " " #lhs "==" #rhs " (failed)"                                             \
           << "[`" << lhs << "` != `" << rhs << "`]";                                    \
      String log = strm.str();                                                           \
      for (const auto &info : list_info) {                                               \
         cfg.trace(config_t::ERROR, "   Failed in context:" + info);                     \
      }                                                                                  \
      list_info.clear();                                                                 \
      cfg.trace(config_t::ERROR, log);                                                   \
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
   cfg.trace(::DDS_ROOT_NAMESPACE::DDS_TINYTEST_NAMESPACE::config_t::MESSAGE, msg);

/*
 * passed `msg` will be printed if next check (TEST_CHECK, TEST_CHECK_EQUAL, ...) fails.
 * Every check will reset info.
 */
#define TEST_INFO(msg) list_info.emplace_back(msg);

} // namespace DDS_TINYTEST_NAMESPACE
} // namespace DDS_ROOT_NAMESPACE
