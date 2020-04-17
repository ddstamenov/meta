#include <iostream>
#include <string>
#include <initializer_list>

template<typename... Ts>
void print_(Ts&&... args) {
   std::string sep=",";
   std::initializer_list<int> tmp = {(std::cout << static_cast<Ts&&>(args) << sep, 0)...};
}

struct PrintT {
   constexpr PrintT(char sep_ = ' ') : sep{sep_}{}

   template<typename... Ts>
      void operator()(Ts&&... args) const {
       std::initializer_list<int> tmp = {(std::cout << static_cast<Ts&&>(args) << sep, 0)...};
      }

   // std::string sep = " ";
   char sep = ' ';
};

constexpr PrintT print{};

struct PrintLnT {
   constexpr PrintLnT(char sep_ = ' ') : sep{sep_}{}

   template<typename... Ts>
   void operator()(Ts&&... args) const {
      if(sizeof...(args)) {
         PrintT{sep} (std::forward<Ts>(args)...);
        std::cout << std::endl;
      }
      }

   char sep = ' ';
};

constexpr PrintLnT println{};
