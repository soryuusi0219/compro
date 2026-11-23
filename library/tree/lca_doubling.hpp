// https://github.com/soryuusi0219/compro/blob/main/library/tree/lca_doubling.hpp
#pragma once
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"

namespace soryuusi {

using namespace std;


struct lca_doubling{
  int N = 0;
  vector<int> par, deg;
  vector<vector<int>> ancs;
  vector<int> depth;
  lca_doubling(int n) : N(n), par(N,0), deg(N,0) {
    ancs.resize(N);
    depth.resize(N);
  }
  void add_edge(int u, int v){
    par[u] ^= v, par[v] ^= u, ++deg[u], ++deg[v];
  }
  void build() {
    vector<int> ord;
    ord.reserve(N-1);
    deg[0] = 0;
    for(int i = 1; i < N; i++) if(deg[i] == 1) ord.push_back(i);
    for(int i = 0; i < (int)ord.size(); i++){
      int v = ord[i], p = par[v];
      par[p] ^= v;
      if(--deg[p] == 1) ord.push_back(p);
    }
    ancs[0].push_back(-1);
    for(int j = (int)ord.size()-1; j >= 0; j--){
      int v = ord[j], p = par[v], dep = depth[p]+1;
      auto& row = ancs[v];
      const int length = soryuusi::bit_width(static_cast<unsigned>(dep));
      row.reserve(length);
      row.push_back(p);
      for(int i = 0; i+1 < length; ++i){
        row.push_back(ancs[row[i]][i]);
      }
      depth[v] = dep;
    }
  }
  int up(int u, int n) const {
    if(n > depth[u]) return -1;
    for(unsigned k = n; k; k &= k-1) u = ancs[u][soryuusi::countr_zero(k)];
    return u;
  }
  int lca(int u, int v) const {
    if(depth[v] < depth[u]) swap(u,v);
    v = up(v, depth[v] - depth[u]);
    if(u==v) return u;
    int i = ancs[u].size()-1;
    while(i >= 0){
      if(i < (int)ancs[u].size() && ancs[u][i] != ancs[v][i]){
        u = ancs[u][i];
        v = ancs[v][i];
      }
      i--;
    }
    return ancs[u][0];
  }
  int dist(int u, int v) const {
    return depth[u] + depth[v] - 2 * depth[lca(u,v)];
  }
  int jump(int u, int v, int k) const {
    if(k == 0) return u;
    int w = lca(u,v);
    int d = depth[u] + depth[v] - 2 * depth[w];
    if(k > d) return -1;
    if(depth[u] - depth[w] >= k) return up(u, k);
    else return up(v,d-k);
  }
};

}  // namespace soryuusi
