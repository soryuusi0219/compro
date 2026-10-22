// https://github.com/soryuusi0219/compro/blob/main/library/tree/lca_doubling.hpp
#pragma once
#include <utility>
#include <vector>

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
      ancs[v].push_back(p);
      int d = dep >> 1;
      int i = 0;
      while(d){
        ancs[v].push_back(ancs[ancs[v][i]][i]);
        i++;
        d >>= 1;
      }
      depth[v] = dep;
    }
  }
  int up(int u, int n) const {
    int i = 0;
    while(n){
      if(n&1) u = ancs[u][i];
      i++;
      n/=2;
    }
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
    int w = lca(u,v);
    int d = depth[u] + depth[v] - 2 * depth[w];
    if(k > d) return -1;
    if(depth[u] - depth[w] >= k) return up(u, k);
    else return up(v,d-k);
  }
};

}  // namespace soryuusi
