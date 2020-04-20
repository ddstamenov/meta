#include <test_framework/tiny_framework.h>

#include <sstream>

TESTS_BEGIN();

static std::stringstream &StreamObject() {
   static std::stringstream obj;
   return obj;
}

static void reset_stream() {
   StreamObject().str("");
   StreamObject().clear();
}

static std::string get_string_and_reset() {
   auto ret = StreamObject().str();
   reset_stream();
   return ret;
}

template <>
struct StreamGetter<std::stringstream> {
   static auto &GetStream() { return StreamObject(); }
};

using StringStreamGetter = StreamGetter<std::stringstream>;
using StreamPrintT = PrintBaseT<StringStreamGetter>;
using StreamPrintLnT = PrintBaseLnT<StringStreamGetter>;

constexpr StreamPrintT sprint{};
constexpr StreamPrintLnT sprintln{};

TEST_CASE(testEmptyPrint) {
   reset_stream();
   sprint();
   auto str = get_string_and_reset();
   TEST_CHECK("" == str);

   sprintln();
   str = get_string_and_reset();
   TEST_CHECK("" == str);
}

TEST_CASE(testDefaultSeparator) {
   reset_stream();
   sprint(5);
   auto str = get_string_and_reset();
   TEST_CHECK("5" == str);

   sprintln(5);
   str = get_string_and_reset();
   TEST_CHECK("5\n" == str);

   sprint("one", "two", 3);
   str = get_string_and_reset();
   TEST_CHECK("one two 3" == str);

   sprintln("one", "two", 3);
   str = get_string_and_reset();
   TEST_CHECK("one two 3\n" == str);
}

TEST_CASE(testCustomSeparator) {
   reset_stream();
   StreamPrintT csprint{'+'};
   StreamPrintLnT csprintln{'+'};

   csprint();
   auto str = get_string_and_reset();
   TEST_CHECK("" == str);
   csprintln();
   str = get_string_and_reset();
   TEST_CHECK("" == str);

   csprint("string");
   str = get_string_and_reset();
   TEST_CHECK("string" == str);
   csprintln("string");
   str = get_string_and_reset();
   TEST_CHECK("string\n" == str);

   int a1{1}, a2{2}, a3{3}, a4{4}, a5{5}, a6{6}, a7{7}, a8{8}, a9{9}, a10{10}, a11{11},
      a12{12}, a13{13};

   csprint(a1, a2);
   str = get_string_and_reset();
   TEST_CHECK("1+2" == str);
   csprintln(a1, a2);
   str = get_string_and_reset();
   TEST_CHECK("1+2\n" == str);

   csprint(a1, a2, a3);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3" == str);
   csprintln(a1, a2, a3);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3\n" == str);

   csprint(a1, a2, a3, a4);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4" == str);
   csprintln(a1, a2, a3, a4);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4\n" == str);

   csprint(a1, a2, a3, a4, a5);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5" == str);
   csprintln(a1, a2, a3, a4, a5);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5\n" == str);

   csprint(a1, a2, a3, a4, a5, a6);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6" == str);
   csprintln(a1, a2, a3, a4, a5, a6);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7, a8);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7, a8);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7, a8, a9);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7, a8, a9);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10+11" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10+11\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10+11+12" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10+11+12\n" == str);

   csprint(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10+11+12+13" == str);
   csprintln(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
   str = get_string_and_reset();
   TEST_CHECK("1+2+3+4+5+6+7+8+9+10+11+12+13\n" == str);
}
