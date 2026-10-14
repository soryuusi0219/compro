// https://github.com/soryuusi0219/compro/blob/main/library/graph/enumerate_cliques.hpp
#pragma once
#include <bitset>
#include <vector>

namespace soryuusi {

using namespace std;

template<int MAXN>
struct EnumerateCliques{
  using bit = bitset<MAXN>;
  int n;
  vector<bit> G;
  EnumerateCliques(int n_) : n(n_), G(n,0) {}
  void add_edge(int u, int v){
    G[u][v] = 1;
    G[v][u] = 1;
  }
  template<class F>
  void solve(F f){
    int ans = 0;
    auto dfs = [&](auto self, bit b, int v) -> void {
      f(b);
      b &= G[v];
      while(b.any()){
        int j = b._Find_first();
        b[j] = 0;
        self(self,b,j);
      }
    };
    bit b;
    for(int i = 0; i < n; ++i) b[i] = 1;
    for(int i = 0; i < n; ++i){
      b[i] = 0;
      dfs(dfs,b,i);
    }
  };
  template<class T>
  T weighted_sum(const vector<T>& weight){
    T ans = 0;
    auto dfs = [&](auto self, bit candidates, int vertex, T product) -> void {
      ans += product;
      candidates &= G[vertex];
      while(candidates.any()){
        int next = candidates._Find_first();
        candidates[next] = 0;
        self(self, candidates, next, product * weight[next]);
      }
    };
    bit candidates;
    for(int i = 0; i < n; ++i) candidates[i] = 1;
    for(int i = 0; i < n; ++i){
      candidates[i] = 0;
      dfs(dfs, candidates, i, weight[i]);
    }
    return ans;
  }
};

}  // namespace soryuusi
