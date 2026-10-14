// https://github.com/soryuusi0219/compro/blob/main/library/tree/lca_doubling.hpp
#pragma once
#include <functional>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;


struct lca_doubling{
  int N = 0;
  vector<vector<int>> G;
  vector<vector<int>> ancs;
  vector<int> depth;
  lca_doubling(int n) : N(n) {
    G.resize(N);
    ancs.resize(N);
    depth.resize(N);
  }
  void add_edge(int u, int v){
    G[u].push_back(v);
    G[v].push_back(u);
  }
  void build() {
    function<void(int, int, int)> dfs = [&](int v, int p, int dep){
      ancs[v].push_back(p);
      int d = dep >> 1;
      int i = 0;
      while(d){
        ancs[v].push_back(ancs[ancs[v][i]][i]);
        i++;
        d >>= 1;
      }
      depth[v] = dep;
      for(int c : G[v]){
        if(c==p) continue;
        dfs(c,v,dep+1);
      }
    };
    dfs(0,-1,0);
  }
  int up(int u, int n){
    int i = 0;
    while(n){
      if(n&1) u = ancs[u][i];
      i++;
      n/=2;
    }
    return u;
  }
  int lca(int u, int v){
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
  int dist(int u, int v){
    return depth[u] + depth[v] - 2 * depth[lca(u,v)];
  }
  int jump(int u, int v, int k){
    int w = lca(u,v);
    int d = depth[u] + depth[v] - 2 * depth[w];
    if(k > d) return -1;
    if(depth[u] - depth[w] >= k) return up(u, k);
    else return up(v,d-k);
  }
};

}  // namespace soryuusi
