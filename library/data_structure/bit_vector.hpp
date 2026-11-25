// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit_vector.hpp
#pragma once
#include <algorithm>
#include <limits>
#include <type_traits>
#include <vector>

#include "../math/internal/internal_bit.hpp"

namespace soryuusi {

using namespace std;

template<class T>
bool f(vector<T>& base, T a){
  using U = make_unsigned_t<T>;
  U value = static_cast<U>(a);
  if(!value) return false;
  if(base.size() == std::numeric_limits<U>::digits && find(base.begin(),base.end(),T{}) == base.end()) return false;
  for(T row : base) value = min(value,static_cast<U>(value^static_cast<U>(row)));
  if(!value) return false;
  int pivot = soryuusi::bit_width(value)-1;
  if(pivot >= static_cast<int>(base.size())) base.resize(pivot+1);
  for(auto& x : base){
    U row = static_cast<U>(x);
    x = static_cast<T>(min(row,static_cast<U>(row^value)));
  }
  base[pivot] = static_cast<T>(value);
  return true;
}

}  // namespace soryuusi
