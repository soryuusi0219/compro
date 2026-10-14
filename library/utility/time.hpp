// https://github.com/soryuusi0219/compro/blob/main/library/utility/time.hpp
#pragma once
#include <iomanip>
#include <iostream>

namespace soryuusi {

using namespace std;

void tick(){
  static clock_t start_t;
  cout << fixed << setprecision(15) << (long double)(clock() - start_t) / CLOCKS_PER_SEC * 1000 << "ms" << endl;  
  start_t = clock();  
}

}  // namespace soryuusi
