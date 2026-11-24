// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra_01.hpp
#pragma once
#include <algorithm>
#include <deque>
#include <vector>
#include <limits>

#include "static_graph.hpp"

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
    deque<int> queue{v};
    vector<unsigned char> settled(n,0);
    d[v] = 0;
    while(!queue.empty()){
      const int u = queue.front();
      queue.pop_front();
      if(settled[u]) continue;
      settled[u] = true;
      const int d0 = d[u];
      for(const auto& edge : G[u]){
        const int candidate = d0+edge.cost;
        if(d[edge.to] <= candidate) continue;
        d[edge.to] = candidate;
        from[edge.to] = u;
        if(edge.cost) queue.push_back(edge.to);
        else queue.push_front(edge.to);
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
