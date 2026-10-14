// https://github.com/soryuusi0219/compro/blob/main/library/graph/maximal_weighted_independent_set.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
pair<T, vector<long long>> maximal_weighted_independent_set(vector<vector<long long>>& G, vector<T> weight){
  using ull = __uint64_t;
  using ulll = __uint128_t;
  static constexpr int MAX = 128;
  int n = G.size();
  assert(n <= MAX);
  const int M = (n * 2) / 23;
  vector<ulll> nbh(n);
  for(int i = 0; i < n; i++) for(auto j:G[i]) nbh[i] |= (((ulll)1) << j);
  map<ulll,pair<T,ulll>> memo;
  auto ctzint128 = [&](ulll x){
    if((ull)x) return __builtin_ctzll((ull)x);
    return __builtin_ctzll((ull)(x >> 64)) + 64;
  };
  auto popcountint128 = [&](ulll x){
    return __builtin_popcountll((ull)x) +  __builtin_popcountll((ull)(x >> 64));
  };
  function<pair<T,ulll>(ulll)> rec = [&](ulll x){
    int degmax = -1, vmax = -1;
    ulll y = x;
    T ret = 0;
    ulll rets = 0;
    while(y){
      int v = ctzint128(y);
      y &= ~(((ulll)1) << v);
      if(!(nbh[v] & x)) ret += weight[v], x &= ~(((ulll)1) << v), rets |= (((ulll)1) << v);
      int deg = popcountint128(nbh[v] & x);
      if(degmax < deg){
        degmax = deg;
        vmax = v;
      } 
    }
    if(degmax == 1){
      ulll y = x;
      while(y){
        int v = ctzint128(y);
        int w = ctzint128(nbh[v] & x);
        y &= ~((((ulll)1) << v) | (((ulll)1) << w));
        if(weight[v] < weight[w]) swap(v,w);
        ret += weight[v], rets |= (((ulll)1) << v);
      }
      return make_pair(ret, rets);
    }
    if(degmax == 2){
      ulll y = x;
      while(y){
        int v = ctzint128(y);
        y &= ~(((ulll)1) << v);
        if(popcountint128(nbh[v] & x) > 1) continue;
        vector<T> a = {weight[v]};
        vector<int> index = {v};
        while(nbh[v] & x){
          int w = ctzint128(nbh[v] & x);
          x &= ~(((ulll)1) << v);
          y &= ~(((ulll)1) << w);
          a.push_back(weight[w]);
          index.push_back(w);
          v = w;
        }
        x &= ~(((ulll)1) << v);
        int m = a.size();
        vector<pair<T,ulll>> dp(m+1, {0,0});
        dp[1] = {a[0], (((ulll)1) << index[0])};
        dp[2] = {a[1], (((ulll)1) << index[1])};
        for(int i = 2; i < m; i++){
          dp[i+1] = max(dp[i-2],dp[i-1]);
          dp[i+1].first += a[i];
          dp[i+1].second |= (((ulll)1) << index[i]);
        }
        pair<T,ulll> tmp = max(dp[m-1], dp[m]);
        ret += tmp.first;
        rets |= tmp.second;
      }
      y = x;
      while(y){
        int v = ctzint128(y);
        y &= ~(((ulll)1) << v);
        vector<T> a = {weight[v]};
        vector<int> index = {v};
        while(nbh[v] & x){
          int w = ctzint128(nbh[v] & x);
          x &= ~(((ulll)1) << v);
          y &= ~(((ulll)1) << w);
          a.push_back(weight[w]);
          index.push_back(w);
          v = w;
        }
        x &= ~(((ulll)1) << v);
        int m = a.size();
        vector<pair<T,ulll>> dp(m, {0,0});
        dp[1] = {a[0], (((ulll)1) << index[0])};
        dp[2] = {a[1], (((ulll)1) << index[1])};
        for(int i = 2; i < m-1; i++){
          dp[i+1] = max(dp[i-2],dp[i-1]);
          dp[i+1].first += a[i];
          dp[i+1].second |= (((ulll)1) << index[i]);
        }
        pair<T,ulll> tmp = max(dp[m-2], dp[m-1]);
        dp[1] = {a[1], (((ulll)1) << index[1])};
        dp[2] = {a[2], (((ulll)1) << index[2])};
        for(int i = 2; i < m-1; i++){
          dp[i+1] = max(dp[i-2],dp[i-1]);
          dp[i+1].first += a[i+1];
          dp[i+1].second |= (((ulll)1) << index[i+1]);
        }
        tmp = max(tmp, max(dp[m-2], dp[m-1]));
        ret += tmp.first;
        rets |= tmp.second;
      }
      return make_pair(ret, rets);
    }
    if(memo.count(x)) return make_pair(ret + memo[x].first, rets + memo[x].second);
    if(!x) return make_pair(ret, rets);
    y = x;
    degmax = -1, vmax = -1;
    while(y){
      int v = ctzint128(y);
      y &= ~(((ulll)1) << v);
      int deg = popcountint128(nbh[v] & x);
      if(degmax < deg){
        degmax = deg;
        vmax = v;
      } 
    }
    ulll z = x & ~(((ulll)1) << vmax);
    auto [cret1, crets1] = rec(z);
    auto [cret2, crets2] = rec(z & ~nbh[vmax]);
    if(cret1 < cret2 + weight[vmax]){
      cret1 = cret2 + weight[vmax];
      crets1 = crets2 | (((ulll)1) << vmax);
    }
    if(popcountint128(x) <= M) memo[x] = {cret1, crets1};
    return make_pair(ret + cret1, rets + crets1);
  };
  auto [wmax, ansset] = rec(n == 128 ? ~((ulll)0) : (((ulll)1) << n)-1);
  vector<long long> ret;
  while(ansset){
    int v = ctzint128(ansset);
    ansset &= ~(((ulll)1) << v);
    ret.push_back(v);
  }
  return {wmax, ret};
}

}  // namespace soryuusi
