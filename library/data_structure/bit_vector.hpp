// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit_vector.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
bool f(vector<T>& base, T a){
  for(auto x : base) a = min(x,a);
  if(!a) return false;
  T c = __builtin_ffsll(a)-1;
  for(auto& x : base) x = min(x,a);
  base[c] = a;
  return true;
}

}  // namespace soryuusi
