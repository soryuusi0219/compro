// https://github.com/soryuusi0219/compro/blob/main/library/graph/maximal_independent_set.hpp
#pragma once
#include <cassert>
#include <functional>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

vector<long long> maximal_independent_set(vector<vector<long long>>& G){
  using ull = unsigned long long;
  using ulll = __uint128_t;
  static constexpr int MAX = 128;
  int n = G.size();
  assert(n <= MAX);
  const int M = (n * 2) / 23;
  if(n <= 64){
    vector<ull> nbh(n);
    for(int i = 0; i < n; i++) for(auto j:G[i]) nbh[i] |= (1ull << j);
    unordered_map<ull,pair<int,ull>> memo;
    function<pair<int,ull>(ull)> rec = [&](ull x){
      int degmax = -1, vmax = -1;
      ull y = x;
      int ret = 0;
      ull rets = 0;
      while(y){
        int v = __builtin_ctzll(y);
        y &= ~(1ull << v);
        if(!(nbh[v] & x)) ret++, x &= ~(1ull << v), rets |= (1ull << v);
        int deg = __builtin_popcountll(nbh[v] & x);
        if(deg == 1) {
          int w = __builtin_ctzll(nbh[v] & x);
          auto [cret, crets] = rec(x & ~(1ull << v) & ~(1ull << w));
          return make_pair(ret + cret + 1, rets | crets | (1ull << v));
        }
        if(degmax < deg){
          degmax = deg;
          vmax = v;
        } 
      }
      if(memo.count(x)) return make_pair(ret + memo[x].first, rets + memo[x].second);
      if(!x) return make_pair(ret, rets);
      ull z = x & ~(1ull << vmax);
      auto [cret1, crets1] = rec(z);
      if(degmax > 2){
        auto [cret2, crets2] = rec(z & ~nbh[vmax]);
        if(cret1 < cret2 + 1){
          cret1 = cret2 + 1;
          crets1 = crets2 | (1ull << vmax);
        }
      }
      if(__builtin_popcountll(x) <= M) memo[x] = {cret1, crets1};
      return  make_pair(ret + cret1, rets + crets1);
    };
    auto [_, ansset] = rec(n == 64 ? ~(0ull) : (1ull << n)-1);
    vector<long long> ret;
    while(ansset){
      int v = __builtin_ctzll(ansset);
      ansset &= ~(1ull << v);
      ret.push_back(v);
    }
    return ret;
  }
  else{
    vector<ulll> nbh(n);
    for(int i = 0; i < n; i++) for(auto j:G[i]) nbh[i] |= (((ulll)1) << j);
    map<ulll,pair<int,ulll>> memo;
    auto ctzint128 = [&](ulll x){
      if((ull)x) return __builtin_ctzll((ull)x);
      return __builtin_ctzll((ull)(x >> 64)) + 64;
    };
    auto popcountint128 = [&](ulll x){
      return __builtin_popcountll((ull)x) +  __builtin_popcountll((ull)(x >> 64));
    };
    function<pair<int,ulll>(ulll)> rec = [&](ulll x){
      int degmax = -1, vmax = -1;
      ulll y = x;
      int ret = 0;
      ulll rets = 0;
      while(y){
        int v = ctzint128(y);
        y &= ~(((ulll)1) << v);
        if(!(nbh[v] & x)) ret++, x &= ~(((ulll)1) << v), rets |= (((ulll)1) << v);
        int deg = popcountint128(nbh[v] & x);
        if(deg == 1) {
          int w = ctzint128(nbh[v] & x);
          auto [cret, crets] = rec(x & ~(((ulll)1) << v) & ~(((ulll)1) << w));
          return make_pair(ret + cret + 1, rets | crets | (((ulll)1) << v));
        }
        if(degmax < deg){
          degmax = deg;
          vmax = v;
        } 
      }
      if(memo.count(x)) return make_pair(ret + memo[x].first, rets + memo[x].second);
      if(!x) return make_pair(ret, rets);
      ulll z = x & ~(((ulll)1) << vmax);
      auto [cret1, crets1] = rec(z);
      if(degmax > 2){
        auto [cret2, crets2] = rec(z & ~nbh[vmax]);
        if(cret1 < cret2 + 1){
          cret1 = cret2 + 1;
          crets1 = crets2 | (((ulll)1) << vmax);
        }
      }
      if(popcountint128(x) <= M) memo[x] = {cret1, crets1};
      return  make_pair(ret + cret1, rets + crets1);
    };
    auto [_, ansset] = rec(n == 128 ? ~((ulll)0) : (((ulll)1) << n)-1);
    vector<long long> ret;
    while(ansset){
      int v = ctzint128(ansset);
      ansset &= ~(((ulll)1) << v);
      ret.push_back(v);
    }
    return ret;
  }
}

}  // namespace soryuusi
