// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra_01.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <iterator>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>
#include <limits>

#include "library/graph/static_graph.hpp"

namespace soryuusi {

using namespace std;

struct Dijkstra01{

  int n;
  static_graph<bool> G;
  vector<int> from;
  vector<int> d;
  Dijkstra01(int n_) : n(n_), G(n){}
  Dijkstra01(int n_, int m) : n(n_), G(n,m){}
  void add_edge(int u, int v, bool cost, bool undirected = false){
    G.add_edge(u,v,cost,undirected);
  }
  vector<int> distance(int v){
    G.build();
    d.assign(n,numeric_limits<int>::max());
    from.assign(n, -1);
    deque<pair<int,int>> que;
    d[v] = 0;
    que.emplace_back(0,v);
    while(que.size()){
      auto [d0,u] = que.front(); que.pop_front();
      if(d[u] < d0) continue;
      for(auto t : G[u]){
        if(d[t.to] > d0+t.cost){
          d[t.to] = d0+t.cost;
          from[t.to] = u;
          if(t.cost) que.emplace_back(d[t.to], t.to);
          else que.emplace_front(d[t.to], t.to);
        }
      }
    }
    return d;
  }
  vector<int> path(int v){
    vector<int> res = {v};
    while(from[v] != -1){
      v = from[v];
      res.push_back(v);
    }
    reverse(res.begin(),res.end());
    return res;
  }
};

}  // namespace soryuusi
