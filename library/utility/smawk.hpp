// https://github.com/soryuusi0219/compro/blob/main/library/utility/smawk.hpp
#pragma once
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, class Compare = less<T>>
vector<pair<int,T>> SMAWK(int h, int w, const auto f, const Compare &comp = Compare()){
  vector<pair<int,T>> res(h);
  auto solve = [&](auto&& self, int k, const vector<int> col) -> void {
    int n = h >> k;
    if(!n) return;
    vector<int> c2;
    c2.reserve(n);
    int i = 0;
    for(int j : col){
      while(i > 0 && !comp(f((i<<k)-1, c2.back()), f((i<<k)-1, j))) {
        c2.pop_back();
        --i;
      }
      if(i < n) {
        c2.push_back(j);
        ++i;
      }
    }
    self(self,k+1,c2);
    i = 1;
    res[(1<<k)-1] = {col[0],f((1<<k)-1, col[0])};
    for(int j : col){
      if(comp(f((i<<k)-1,j), res[(i<<k)-1].second)) res[(i<<k)-1] = {j,f((i<<k)-1,j)};
      while(i+2 <= n && res[((i+1)<<k)-1].first == j){
        i+=2;
        res[(i<<k)-1] = {j,f((i<<k)-1,j)};
      }
    }
  };
  vector<int> row(h), col(w);
  iota(row.begin(),row.end(),0);
  iota(col.begin(),col.end(),0);
  solve(solve,0,col);
  return res;
}

}  // namespace soryuusi
