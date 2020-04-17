#include <initializer_list>
#include <iostream>
#include <ostream>
#include <string>

#define PRINT_SEPARATOR ' '

template <typename Stream>
struct StreamGetter {
   static auto getSteam();
};

template <>
struct StreamGetter<std::ostream> {
   static std::ostream &GetStream() { return std::cout; }
};

using OstreamGetter = StreamGetter<std::ostream>;

template <typename Stream>
struct PrintBaseT {
   constexpr PrintBaseT(char sep_ = PRINT_SEPARATOR)
      : sep{sep_} {}

   void static call(char /*separator*/) {}

   template <typename T>
   static void call(char sep, T &&arg) {
      Stream::GetStream() << static_cast<T &&>(arg);
   }

   template <typename T1, typename T2>
   static void call(char sep, T1 &&a1, T2 &&a2) {
      Stream::GetStream() << static_cast<T1 &&>(a1) << sep << static_cast<T2 &&>(a2);
   }

   template <typename T1, typename T2, typename T3>
   static void call(char sep, T1 &&a1, T2 &&a2, T3 &&a3) {
      Stream::GetStream() << static_cast<T1 &&>(a1) << sep << static_cast<T2 &&>(a2)
                          << sep << static_cast<T3 &&>(a3);
   }

   template <typename T1, typename T2, typename T3, typename T4>
   static void call(char sep, T1 &&a1, T2 &&a2, T3 &&a3, T4 &&a4) {
      Stream::GetStream() << static_cast<T1 &&>(a1) << sep << static_cast<T2 &&>(a2)
                          << sep << static_cast<T3 &&>(a3) << sep
                          << static_cast<T4 &&>(a4);
   }

   template <typename T1, typename T2, typename T3, typename T4, typename T5>
   static void call(char sep, T1 &&a1, T2 &&a2, T3 &&a3, T4 &&a4, T5 &&a5) {
      Stream::GetStream() << static_cast<T1 &&>(a1) << sep << static_cast<T2 &&>(a2)
                          << sep << static_cast<T3 &&>(a3) << sep
                          << static_cast<T4 &&>(a4) << sep << static_cast<T5 &&>(a5);
   }

   template <typename T1,
             typename T2,
             typename T3,
             typename T4,
             typename T5,
             typename... Ts>
   static void
   call(char sep, T1 &&a1, T2 &&a2, T3 &&a3, T4 &&a4, T5 &&a5, Ts &&... args) {
      Stream::GetStream() << static_cast<T1 &&>(a1) << sep << static_cast<T2 &&>(a2)
                          << sep << static_cast<T3 &&>(a3) << sep
                          << static_cast<T4 &&>(a4) << sep << static_cast<T5 &&>(a5);
      if (sizeof...(args)) {
         Stream::GetStream() << sep;
         call(sep, args...);
      }
   }

   template <typename... Ts>
   void operator()(Ts &&... args) const {
      call(sep, static_cast<Ts &&>(args)...);
   }

   char sep = PRINT_SEPARATOR;
};

using PrintT = PrintBaseT<OstreamGetter>;

constexpr PrintT print{};

template <typename Stream>
struct PrintBaseLnT {
   constexpr PrintBaseLnT(char sep_ = PRINT_SEPARATOR)
      : sep{sep_} {}

   template <typename... Ts>
   void operator()(Ts &&... args) const {
      if (sizeof...(args)) {
         PrintBaseT<Stream>{sep}(std::forward<Ts>(args)...);
         Stream::GetStream() << std::endl;
      }
   }

   char sep = PRINT_SEPARATOR;
};

using PrintLnT = PrintBaseLnT<OstreamGetter>;

constexpr PrintLnT println{};
