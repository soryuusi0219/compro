// https://github.com/soryuusi0219/compro/blob/main/library/utility/monotone_minima.hpp
#pragma once
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

namespace internal {

template<class T, class F, class Compare>
void monotone_minima_into(vector<pair<int,T>>& result, int h, int w, const F& f, const Compare& comp){
  assert(h >= 0 && w >= 0 && (h == 0 || w > 0));
  result.resize(h);
  if(!h) return;
  auto solve = [&](auto&& self, int top, int bottom, int left, int right) -> void {
    int row = top+(bottom-top)/2, best = left;
    T value = f(row,left);
    for(int col = left+1; col <= right; ++col){
      T candidate = f(row,col);
      if(comp(candidate,value)) best = col, value = std::move(candidate);
    }
    result[row] = {best,std::move(value)};
    if(top < row) self(self,top,row,left,best);
    if(row+1 < bottom) self(self,row+1,bottom,best,right);
  };
  solve(solve,0,h,0,w-1);
}

}  // namespace internal

template<class T, class F, class Compare = less<T>>
vector<pair<int,T>> monotoneminima(int h, int w, const F& f, const Compare &comp = Compare()){
  vector<pair<int,T>> result;
  internal::monotone_minima_into(result,h,w,f,comp);
  return result;
}

}  // namespace soryuusi
