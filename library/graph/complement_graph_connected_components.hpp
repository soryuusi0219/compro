// https://github.com/soryuusi0219/compro/blob/main/library/graph/complement_graph_connected_components.hpp
#pragma once
#include <vector>
#include <algorithm>

namespace soryuusi {

using namespace std;


struct ComplementConnectedComponents{
  int n;
  vector<int> deg, to, nxt, h;
  ComplementConnectedComponents(int n_) : n(n_), deg(n,0), h(n,-1){}
  ComplementConnectedComponents(int n_, int m) : n(n_), deg(n,0), h(n,-1){to.reserve(2*m), nxt.reserve(2*m);}
  void add_edge(int u, int v){
    nxt.push_back(h[u]);
    h[u] = to.size();
    to.push_back(v);
    nxt.push_back(h[v]);
    h[v] = to.size();
    to.push_back(u);
    ++deg[u];
    ++deg[v];
  }
  vector<vector<int>> solve(){
    if(n == 0) return {};
    const int root = min_element(deg.begin(),deg.end())-deg.begin();
    vector<int> next(n+1), marked(n,-1);
    vector<vector<int>> groups(1,vector<int>{root});
    groups[0].reserve(max(1,n-deg[root]));
    for(int e = h[root]; e != -1; e = nxt[e]) marked[to[e]] = root;
    int tail = n;
    for(int v = 0; v < n; ++v) if(v != root){
      if(marked[v] == root) next[tail] = v, tail = v;
      else groups[0].push_back(v);
    }
    next[tail] = n;
    auto expand = [&](vector<int>& component, int i){
      for(; i < (int)component.size() && next[n] != n; ++i){
        const int v = component[i];
        for(int e = h[v]; e != -1; e = nxt[e]) marked[to[e]] = v;
        for(int p = n, u = next[p]; u != n; u = next[p]){
          if(marked[u] == v) p = u;
          else{
            next[p] = next[u];
            component.push_back(u);
          }
        }
      }
    };
    expand(groups.front(),1);
    while(next[n] != n){
      const int first = next[n];
      next[n] = next[first];
      expand(groups.emplace_back(1,first),0);
    }
    return groups;
  }
};

}  // namespace soryuusi
