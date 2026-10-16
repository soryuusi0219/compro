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

namespace soryuusi {

using namespace std;

struct Dijkstra1{

  int n;
  vector<int> h, nxt, to, from;
  vector<int> d;
  Dijkstra1(int n_) : n(n_), h(n,-1){}
  Dijkstra1(int n_, int m) : n(n_), h(n,-1){
    nxt.reserve(m);
    to.reserve(m);
  }
  void add_edge(int u, int v, bool undirected = false){
    nxt.push_back(h[u]);
    h[u] = to.size();
    to.push_back(v);
    if(undirected) {
      nxt.push_back(h[v]);
      h[v] = to.size();
      to.push_back(u);
    }
  }
  vector<int> distance(int v){
    d.assign(n,numeric_limits<int>::max());
    from.assign(n, -1);
    queue<int> que;
    d[v] = 0;
    que.push(v);
    while(que.size()){
      auto u = que.front(); que.pop();
      for(int t = h[u]; ~t; t = nxt[t]){
        if(d[to[t]] > d[u]+1){
          d[to[t]] = d[u]+1;
          from[to[t]] = u;
          que.push(to[t]);
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
