// https://github.com/soryuusi0219/compro/blob/main/library/tree/centroid_decomposition.hpp
#pragma once
#include <vector>

#include "../graph/static_graph.hpp"

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
    vector<unsigned char> seen(n,0);
    struct Frame {int vertex, next;};
    vector<Frame> dfs;
    auto enter = [&](int v){
      sz[v] = 1;
      dfs.push_back({v,G.start[v]});
    };
    vector<int> st = {0};
    st.reserve(n);
    while(st.size()){
      int start = st.back();
      st.pop_back();
      int thre = sz[start], g;
      enter(start);
      while(true){
        auto& frame = dfs.back();
        int v = frame.vertex;
        int p = dfs.size() > 1 ? dfs[dfs.size()-2].vertex : -1;
        if(frame.next != G.start[v+1]){
          int to = G.es[frame.next++].to;
          if(to != p && !seen[to]) enter(to);
          continue;
        }
        if(sz[v]*2 >= thre){
          g = v;
          if(p != -1) sz[p] = thre-sz[v];
          break;
        }
        dfs.pop_back();
        sz[p] += sz[v];
      }
      dfs.clear();
      dfs_order.emplace_back(g);
      sz[g] = thre;
      seen[g] = true;
      for(int c : G.vertices(g)) if(!seen[c]) st.push_back(c);
    }
  }
};

}  // namespace soryuusi
