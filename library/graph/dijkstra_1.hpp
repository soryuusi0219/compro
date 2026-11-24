// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra_1.hpp
#pragma once
#include <algorithm>
#include <vector>
#include <limits>

#include "static_graph.hpp"

namespace soryuusi {

using namespace std;

struct Dijkstra1{

  int n;
  static_graph<> G;
  vector<int> from;
  vector<int> d;
  Dijkstra1(int n_) : n(n_), G(n){}
  Dijkstra1(int n_, int m) : n(n_), G(n,m){}
  void add_edge(int u, int v, bool undirected = false){
    G.add_edge(u,v,undirected);
  }
  vector<int> distance(int v){
    G.build();
    d.assign(n,numeric_limits<int>::max());
    from.assign(n, -1);
    vector<int> queue{v};
    queue.reserve(n);
    d[v] = 0;
    for(int i = 0; i < (int)queue.size(); ++i){
      const int u = queue[i], candidate = d[u]+1;
      for(int to : G.vertices(u)) if(d[to] == numeric_limits<int>::max()){
        d[to] = candidate;
        from[to] = u;
        queue.push_back(to);
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
