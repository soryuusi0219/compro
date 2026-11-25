// https://github.com/soryuusi0219/compro/blob/main/library/utility/smawk.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

namespace internal {

template<class T>
struct SmawkBuffer {
  vector<pair<int,T>> result;
  vector<int> reduced;

  template<class F, class Compare>
  void solve(int h, int w, const F& f, const Compare& comp){
    assert(h >= 0 && w >= 0 && (h == 0 || w > 0));
    result.resize(h);
    if(!h) return;
    size_t workspace_size = 0;
    if(w > 1) for(int rows = h; rows > 1; rows /= 2) workspace_size += min(rows,w);
    reduced.resize(workspace_size);
    // At this depth the rows have indices step-1, 2*step-1, ... .
    auto search = [&]<bool Implicit>(auto&& self, int step, int width, const int* columns, int* buffer) -> void {
      int rows = h/step;
      auto column = [&](int i){if constexpr (Implicit) return i; else return columns[i];};
      if(width == 1){
        int col = column(0);
        for(int i = 1; i <= rows; ++i) result[i*step-1] = {col,f(i*step-1,col)};
        return;
      }
      if(rows > 1){
        int count = 0;
        for(int i = 0; i < width; ++i){
          int col = column(i);
          while(count && comp(f(count*step-1,col),f(count*step-1,buffer[count-1]))) --count;
          if(count < rows){
            buffer[count] = col;
            ++count;
          }
        }
        self.template operator()<false>(self,step*2,count,buffer,buffer+count);
      }
      // The intervening rows share the column bounds supplied by the recursion.
      int i = 0, col = column(0);
      for(int row = 1; row <= rows; row += 2){
        int output = row*step-1;
        int last = row < rows ? result[output+step].first : column(width-1);
        int best = col;
        T value = f(output,best);
        while(col != last){
          col = column(++i);
          T candidate = f(output,col);
          if(comp(candidate,value)) best = col, value = std::move(candidate);
        }
        result[output] = {best,std::move(value)};
      }
    };
    search.template operator()<true>(search,1,w,nullptr,reduced.data());
  }
};

}  // namespace internal

template<class T, class Compare = less<T>>
vector<pair<int,T>> SMAWK(int h, int w, const auto f, const Compare &comp = Compare()){
  internal::SmawkBuffer<T> buffer;
  buffer.solve(h,w,f,comp);
  return std::move(buffer.result);
}

}  // namespace soryuusi
