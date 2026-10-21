// https://github.com/soryuusi0219/compro/blob/main/library/tree/centroid_decomposition.hpp
#pragma once
#include <vector>

#include "library/graph/static_graph.hpp"

namespace soryuusi {

using namespace std;

struct CentroidDecomposition{
  int n;
  static_graph<> G;
  vector<int> sz, dfs_order;
  CentroidDecomposition() {}
  CentroidDecomposition(int n_) : n(n_), G(n,2*n-2), sz(n,n) {
    dfs_order.reserve(n);
  }
  void addEdge(int u, int v){
    G.add_edge(u,v,true);
  }
  void build(){
    G.build();
    int g = -1, thre = n;
    vector<bool> seen(n,0);
    auto rec = [&](auto self, int v, int p) -> bool {
      sz[v] = 1;
      for(int c : G.vertices(v)) if(!seen[c] && c != p){
        if(self(self,c,v)) return true;
        sz[v] += sz[c];
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
      for(int c : G.vertices(g)) if(!seen[c]) st.push_back(c);
    }
  }
};

}  // namespace soryuusi
