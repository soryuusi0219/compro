// https://github.com/soryuusi0219/compro/blob/main/library/string/sort_concatenation_min.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

#include "library/string/z_algorithm.hpp"

namespace soryuusi {

template<class T>
void sort_concatenation_min(std::vector<T>& v) {
  const int n = v.size();
  std::vector<std::vector<int>> z;
  z.reserve(n);
  for(const auto& w : v) {
    if(w.empty()) z.emplace_back();
    else z.emplace_back(z_algo(std::vector<typename T::value_type>(w.begin(),w.end())));
  }
  std::vector<int> ord(n);
  std::iota(ord.begin(),ord.end(),0);
  std::sort(ord.begin(), ord.end(), [&](int l, int r) {
    int ls = v[l].size();
    int rs = v[r].size();
    if(!rs) return false;
    if(!ls) return true;
    int lcp = 0;
    while(lcp < std::min(ls,rs) && v[l][lcp] == v[r][lcp]) ++lcp;
    if(lcp < std::min(ls,rs)) return v[l][lcp] < v[r][lcp];
    if(ls == rs) return false;
    else if(ls < rs){
      lcp = z[r][ls];
      if(lcp < rs-ls) return v[r][lcp] < v[r][ls+lcp];
      lcp = 0;
      while(lcp < ls && v[l][lcp] == v[r][rs-ls+lcp]) ++lcp;
      if(lcp < ls) return v[r][rs-ls+lcp] < v[l][lcp];
      return false;
    }else{
      lcp = z[l][rs];
      if(lcp < ls-rs) return v[l][rs+lcp] < v[l][lcp];
      lcp = 0;
      while(lcp < rs && v[l][ls-rs+lcp] == v[r][lcp]) ++lcp;
      if(lcp < rs) return v[r][lcp] < v[l][ls-rs+lcp];
      return false;
    }
  });
  std::vector<T> tmp;
  tmp.reserve(n);
  for(int i : ord) tmp.emplace_back(std::move(v[i]));
  v = std::move(tmp);
}

}  // namespace soryuusi
