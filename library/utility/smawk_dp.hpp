// https://github.com/soryuusi0219/compro/blob/main/library/utility/smawk_dp.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "library/utility/smawk.hpp"

namespace soryuusi {

using namespace std;

template<class T, class F, class Compare = less<T>>
vector<T> SMAWK_dp(int n, const F& f, const Compare &comp = Compare()){
  vector<T> dp(n+1);
  for(int i = 0; i <= n; ++i) dp[i] = f(0,i);
  int l,m,r;
  auto get_cost = [&](int y, int x){return dp[x+l] + f(x+l,y+m);};
  for(m = 1; m <= n; ++m){
    int c = m&-m;
    l = m-c, r = min(m+c, n+1);
    auto res = SMAWK<T>(r-m,m-l,get_cost,comp);
    for(int i = m; i < r; ++i) if(comp(res[i-m].second,dp[i])) dp[i] = res[i-m].second;
  }
  return dp;
}

}  // namespace soryuusi
