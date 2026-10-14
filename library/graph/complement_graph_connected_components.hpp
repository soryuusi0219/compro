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
    int x = min_element(deg.begin(), deg.end()) - deg.begin();
    int k = deg[x];
    vector<int> id(n,k), le(k);
    for(int i = 0, e = h[x]; ~e; e = nxt[e]) le[i] = to[e], id[to[e]] = i++;
    vector<bool> G(k*k,true);
    vector<int> cnt(k,n-k);
    for(int i : le) for(int e = h[i]; ~e; e = nxt[e]) {
      if(id[to[e]] < k) G[id[i] + id[to[e]] * k] = false;
      else --cnt[id[i]];
    }
    vector<vector<int>> ans(1,vector<int>(n-k));
    for(int i = 0, j = 0; i < n; ++i) if(id[i] == k) ans[0][j++] = i;
    vector<bool> seen(k+1);
    vector<int> st;
    for(int i = 0; i < k; ++i) if(cnt[i]){
      ans[0].push_back(le[i]);
      st.push_back(i);
      seen[i] = true;
    }
    for(int v = k; ~v; --v) if(!seen[v]){
      if(v < k) st.push_back(v), ans.push_back({le[v]});
      seen[v] = true;
      while(st.size()){
        int w = st.back(); st.pop_back();
        for(int i = 0; i < k; ++i) if(G[i+w*k] && !seen[i]){
          seen[i] = true;
          ans.back().push_back(le[i]);
          st.push_back(i);
        }
      }
    }
    return ans;
  }
};

}  // namespace soryuusi
