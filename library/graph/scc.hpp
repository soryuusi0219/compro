// https://github.com/soryuusi0219/compro/blob/main/library/graph/scc.hpp
#pragma once
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

#include "static_graph.hpp"

namespace soryuusi{

using namespace std;

struct SCC{
  private:
    int n = 0;
    static_graph<> G;
  public:
    SCC() {}
    SCC(int n_) : n(n_), G(n) {}
    SCC(int n_, int m) : n(n_), G(n,m) {}
    void add_edge(int u, int v){
      G.add_edge(u,v);
    }
    pair<int,vector<int>> scc_ids(){
      G.build();
      vector<int> ids(n,-1), ord(n,-1), active;
      struct Frame{int vertex, next, begin, low;};
      vector<Frame> stack;
      active.reserve(n);
      int index = 0, groups = 0;
      auto enter = [&](int v){
        ord[v] = index++;
        active.push_back(v);
        stack.push_back({v,G.start[v+1],G.start[v],ord[v]});
      };
      for(int root = 0; root < n; ++root) if(ord[root] == -1){
        enter(root);
        while(!stack.empty()){
          auto& [v,next,begin,low] = stack.back();
          if(next != begin){
            const int to = G.es[--next].to;
            if(ord[to] == -1) enter(to);
            else low = min(low,ord[to]);
            continue;
          }
          if(low == ord[v]){
            int member;
            do {
              member = active.back();
              active.pop_back();
              ids[member] = groups;
              ord[member] = n;
            } while(member != v);
            ++groups;
          }
          const int subtree_low = low;
          stack.pop_back();
          if(!stack.empty()) stack.back().low = min(stack.back().low,subtree_low);
        }
      }
      for(int& id : ids) id = groups-id-1;
      return {groups,move(ids)};
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
      for(int i = 0; i < n; ++i) for(int c : G.vertices(i)) if(allow_self_edge || ids[i] != ids[c]) graph[ids[i]].push_back(ids[c]);
      if(!allow_double_edge) for(auto& v : graph) {
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
      }
      return {g,move(graph),move(ids)};
    }
};

} //namespace  soryuusi
