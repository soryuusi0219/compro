// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra.hpp
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

template<class T, auto op, auto e, auto infty>
struct Dijkstra{
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(infty)>, "infty must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");

  const T INF = infty();

  int n;
  static_graph<T> G;
  vector<int> from;
  vector<T> d;
  Dijkstra(int n_) : n(n_), G(n){}
  Dijkstra(int n_, int m) : n(n_), G(n,m){}
  void add_edge(int u, int v, T cost, bool undirected = false){
    G.add_edge(u,v,cost,undirected);
  }
  vector<T> distance(int v){
    G.build();
    d.assign(n,infty());
    from.assign(n, -1);
    priority_queue<pair<T,int>, vector<pair<T,int>>, greater<pair<T,int>>> que;
    d[v] = e();
    que.push({e(),v});
    while(!que.empty()){
      auto [d0,u] = que.top(); que.pop();
      if(d[u] < d0) continue;
      for(auto t : G[u]){
        if(d[t.to] > op(d0, t.cost)){
          d[t.to] = op(d0, t.cost);
          from[t.to] = u;
          que.push({d[t.to], t.to});
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


namespace internal {

  template<class T>
  struct dijkstra_sum {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return x+y;};
    static constexpr T (*e)() = []()->T {return T(0);};
    static constexpr T (*infty)() = []()->T {return numeric_limits<T>::max()/2;};
  };

}  // namespace internal

template<typename T>
using Dijkstra_sum = Dijkstra<T,internal::dijkstra_sum<T>::op,internal::dijkstra_sum<T>::e,internal::dijkstra_sum<T>::infty>;
using Dijkstrall = Dijkstra_sum<long long>;

}  // namespace soryuusi
