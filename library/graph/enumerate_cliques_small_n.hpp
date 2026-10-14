// https://github.com/soryuusi0219/compro/blob/main/library/graph/enumerate_cliques_small_n.hpp
#pragma once
#include <cstdint>
#include <vector>

namespace soryuusi {

using namespace std;

struct EnumerateCliques{
  using bit = __uint128_t;
  int n;
  vector<bit> G;
  static inline int ctz(bit x) {return (uint64_t)x ? __builtin_ctzll(x) : 64 + __builtin_ctzll(x >> 64);}
  EnumerateCliques(int n_) : n(n_), G(n,0) {}
  void add_edge(int u, int v){
    G[u] |= (bit)1 << v;
    G[v] |= (bit)1 << u;
  }
  template<class F>
  void solve(F f){
    int ans = 0;
    auto dfs = [&](auto self, bit b, int v) -> void {
      f(b);
      b &= G[v];
      while(b){
        int j = ctz(b);
        b &= ~((bit)1 << j);
        self(self,b,j);
      }
    };
    bit b = ((bit) 1 << n) - 1;
    for(int i = 0; i < n; ++i){
      b &= ~((bit)1 << i);
      dfs(dfs,b,i);
    }
  };
  template<class T>
  T weighted_sum(const vector<T>& weight){
    T ans = 0;
    auto dfs = [&](auto self, bit candidates, int vertex, T product) -> void {
      ans += product;
      candidates &= G[vertex];
      while(candidates){
        int next = ctz(candidates);
        candidates &= ~((bit)1 << next);
        self(self, candidates, next, product * weight[next]);
      }
    };
    bit candidates = ((bit)1 << n) - 1;
    for(int i = 0; i < n; ++i){
      candidates &= ~((bit)1 << i);
      dfs(dfs, candidates, i, weight[i]);
    }
    return ans;
  }
};

}  // namespace soryuusi
