// https://github.com/soryuusi0219/compro/blob/main/library/utility/time.hpp
#pragma once
#include <ctime>
#include <iomanip>
#include <iostream>

namespace soryuusi {

using namespace std;

inline void tick(){
  static std::clock_t start_t;
  std::cout << std::fixed << std::setprecision(15) << (long double)(std::clock() - start_t) / CLOCKS_PER_SEC * 1000 << "ms" << std::endl;
  start_t = std::clock();
}

}  // namespace soryuusi
