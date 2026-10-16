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

namespace soryuusi {

using namespace std;

struct Dijkstra01{

  int n;
  vector<int> h, nxt, to, from;
  vector<bool> w;
  vector<int> d;
  Dijkstra01(int n_) : n(n_), h(n,-1){}
  Dijkstra01(int n_, int m) : n(n_), h(n,-1){
    nxt.reserve(m);
    to.reserve(m);
    w.reserve(m);
  }
  void add_edge(int u, int v, bool cost, bool undirected = false){
    nxt.push_back(h[u]);
    h[u] = to.size();
    to.push_back(v);
    w.push_back(cost);
    if(undirected) {
      nxt.push_back(h[v]);
      h[v] = to.size();
      to.push_back(u);
      w.push_back(cost);
    }
  }
  vector<int> distance(int v){
    d.assign(n,numeric_limits<int>::max());
    from.assign(n, -1);
    deque<pair<int,int>> que;
    d[v] = 0;
    que.emplace_back(0,v);
    while(que.size()){
      auto [d0,u] = que.front(); que.pop_front();
      if(d[u] < d0) continue;
      for(int t = h[u]; ~t; t = nxt[t]){
        if(d[to[t]] > d0+w[t]){
          d[to[t]] = d0+w[t];
          from[to[t]] = u;
          if(w[t]) que.emplace_back(d[to[t]], to[t]);
          else que.emplace_front(d[to[t]], to[t]);
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
