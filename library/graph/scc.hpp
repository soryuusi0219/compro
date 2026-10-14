// https://github.com/soryuusi0219/compro/blob/main/library/graph/scc.hpp
#pragma once
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

namespace soryuusi{

using namespace std;

struct SCC{
  private:
    int n;
    vector<int> h, nxt, to, from;
  public:
    SCC() {}
    SCC(int n_) : n(n_), h(n,-1) {}
    SCC(int n_, int m) : n(n_), h(n,-1) {
      nxt.reserve(m);
      to.reserve(m);
    }
    void add_edge(int u, int v){
      nxt.push_back(h[u]);
      h[u] = to.size();
      to.push_back(v);
    }
    pair<int,vector<int>> scc_ids(){
      vector<int> ids(n,-1);
      vector<int> recst, st, ord(n,-1), low(n,-1), par(n,-1);
      recst.reserve(n);
      st.reserve(n);
      int k = 0, g = 0;
      for(int i = 0; i < n; ++i) if(!~ord[i]){
        recst.push_back(i);
        int p = -1;
        st.push_back(i);
        while(recst.size()){
          int v = recst.back();
          if(v >= 0){
            if(~ord[v]){
              if(!~ids[v]) low[p] = min(low[p], ord[v]);
              recst.pop_back();
            }else{
              recst.back() -= n;
              low[v] = ord[v] = k++;
              par[v] = p;
              p = v;
              st.push_back(v);
              for(int e = h[v]; ~e; e = nxt[e]) recst.push_back(to[e]);
            }
          }else{
            recst.pop_back();
            v += n;
            if(low[v] == ord[v]){
              ids[v] = g;
              while(st.back() != v){
                ids[st.back()] = g;
                st.pop_back();
              }
              st.pop_back();
              ++g;
            }
            p = par[v];
            if(~p) low[p] = min(low[p], low[v]);
          }
        }
      }
      for(auto& i : ids) i = g-i-1;
      return {g,ids};
    }
    vector<vector<int>> scc(){
      auto [g,ids] = scc_ids();
      vector<vector<int>> res(g);
      for(int i = 0; i < n; ++i) res[ids[i]].push_back(i);
      return res;
    }
    tuple<int, vector<vector<int>>, vector<int>> scc_compressed(bool allow_self_edge = false, bool allow_double_edge = false) {
      auto [g, ids] = scc_ids();
      vector<vector<int>> graph(g);
      for(int i = 0; i < n; ++i) for(int e = h[i]; ~e; e = nxt[e]) if(allow_self_edge || ids[i] != ids[to[e]])  graph[ids[i]].push_back(ids[to[e]]);
      if(!allow_double_edge) for(auto& v : graph) {
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
      }
      return {g,graph,ids};
    }
};

} //namespace  soryuusi