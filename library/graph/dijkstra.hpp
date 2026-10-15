// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <iterator>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, auto e, auto infty, auto op>
struct Dijkstra{
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(infty)>, "infty must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");

  int n;
  vector<int> h, nxt, to, from;
  vector<T> w, d;
  Dijkstra(int n_) : n(n_), h(n,-1){}
  Dijkstra(int n_, int m) : n(n_), h(n,-1){
    nxt.reserve(m);
    to.reserve(m);
    w.reserve(m);
  }
  void add_edge(int u, int v, T cost, bool undirected = false){
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
  vector<T> distance(int v){
    d.assign(n,infty());
    from.assign(n, -1);
    priority_queue<pair<T,int>, vector<pair<T,int>>, greater<pair<T,int>>> que;
    d[v] = e();
    que.push({e(),v});
    while(!que.empty()){
      auto [d0,u] = que.top(); que.pop();
      if(d[u] < d0) continue;
      for(int t = h[u]; ~t; t = nxt[t]){
        if(d[to[t]] > op(d0, w[t])){
          d[to[t]] = op(d0, w[t]);
          from[to[t]] = u;
          que.push({d[to[t]], to[t]});
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

long long  e() {return 0;}
long long  op(long long  x, long long  y) {return x+y;}
long long  infty() {return 1ll << 60;}
using Dijkstrall = Dijkstra<long long, e, infty, op>;

}  // namespace soryuusi
