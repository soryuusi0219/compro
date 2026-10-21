// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra_1.hpp
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
    queue<int> que;
    d[v] = 0;
    que.push(v);
    while(que.size()){
      auto u = que.front(); que.pop();
      for(int c : G.vertices(u)){
        if(d[c] > d[u]+1){
          d[c] = d[u]+1;
          from[c] = u;
          que.push(c);
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
