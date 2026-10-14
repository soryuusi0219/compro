// https://github.com/soryuusi0219/compro/blob/main/library/utility/monotone_minima.hpp
#pragma once
#include <functional>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, class F, class Compare = less<T>>
vector<pair<int,T>> monotoneminima(int h, int w, const F& f, const Compare &comp = Compare()){
  vector<pair<int,T>> res(h);
  int k = 1; while(k < h+1) k <<= 1;
  for(k >>= 1; k; k >>= 1){
    res[k-1] = {0,f(k-1,0)};
    for(int i = k-1, j = 0; j < w; ++j){
      T tmp = f(i,j);
      if(comp(tmp, res[i].second)) res[i] = {j,tmp};
      if(i + k < h && res[i+k].first == j){
        if((i += (k << 1)) >= h) break;
        res[i] = {j,f(i,j)};
        --j;
      }
    }
  }
  return res;
}

}  // namespace soryuusi
