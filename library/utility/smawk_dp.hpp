// https://github.com/soryuusi0219/compro/blob/main/library/utility/smawk_dp.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "smawk.hpp"

namespace soryuusi {

using namespace std;

template<class T, class F, class Compare = less<T>>
vector<T> SMAWK_dp(int n, const F& f, const Compare &comp = Compare()){
  vector<T> dp(n+1);
  for(int i = 0; i <= n; ++i) dp[i] = f(0,i);
  internal::SmawkBuffer<T> buffer;
  for(int m = 1; m <= n; ++m){
    int c = m&-m;
    if(c == 1){
      T value = dp[m-1]+f(m-1,m);
      if(comp(value,dp[m])) dp[m] = std::move(value);
      continue;
    }
    int l = m-c, r = m+min(c,n+1-m);
    auto get_cost = [&](int y, int x){return dp[x+l]+f(x+l,y+m);};
    buffer.solve(r-m,m-l,get_cost,comp);
    auto& res = buffer.result;
    for(int i = m; i < r; ++i) if(comp(res[i-m].second,dp[i])) dp[i] = res[i-m].second;
  }
  return dp;
}

}  // namespace soryuusi
