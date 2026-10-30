// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit_vector.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
bool f(vector<T>& base, T a){
  for(auto x : base) a = min(a,a^x);
  if(!a) return false;
  size_t c = bit_width((make_unsigned_t<T>)a)-1;
  if(base.size() <= c) base.resize(c+1);
  for(auto& x : base) x = min(x,x^a);
  base[c] = a;
  return true;
}

}  // namespace soryuusi
