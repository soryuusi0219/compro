// https://github.com/soryuusi0219/compro/blob/main/library/tree/centroid_decomposition.hpp
#pragma once
#include <vector>

namespace soryuusi {

using namespace std;

struct CentroidDecomposition{
  int n;
  vector<int> h, nxt, to, sz, dfs_order;
  CentroidDecomposition() {}
  CentroidDecomposition(int n_) : n(n_), h(n,-1), sz(n,n) {
    nxt.reserve(2*n-2);
    to.reserve(2*n-2);
    dfs_order.reserve(n);
  }
  void addEdge(int u, int v){
    nxt.emplace_back(h[u]);
    h[u] = to.size();
    to.emplace_back(v);
    nxt.emplace_back(h[v]);
    h[v] = to.size();
    to.emplace_back(u);
  }
  void build(){
    int g = -1, thre = n;
    vector<bool> seen(n,0);
    auto rec = [&](auto self, int v, int p) -> bool {
      sz[v] = 1;
      for(int e = h[v]; ~e; e=nxt[e]) if(!seen[to[e]] && to[e] != p){
        if(self(self,to[e],v)) return true;
        sz[v] += sz[to[e]];
      }
      if(sz[v]*2 >= thre){
        g = v;
        if(~p) sz[p] = thre - sz[v];
        return true;
      }
      return false;
    };
    vector<int> st = {0};
    st.reserve(n);
    while(st.size()){
      int v = st.back(); st.pop_back();
      thre = sz[v];
      rec(rec,v,-1);
      dfs_order.emplace_back(g);
      sz[g] = thre;
      seen[g] = true;
      for(int e = h[g]; ~e; e=nxt[e]) if(!seen[to[e]]) st.push_back(to[e]);
    }
  }
};

}  // namespace soryuusi
