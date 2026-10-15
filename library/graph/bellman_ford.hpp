// https://github.com/soryuusi0219/compro/blob/main/library/graph/bellman_ford.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, auto e, auto infty, auto op>
struct BellmanFord{
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(infty)>, "infty must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");

  struct Edge{
    int from;
    int to;
    T cost;
  };
  int n;
  vector<Edge> graph;
  BellmanFord(int n0){
    n = n0;
    graph.resize(n);
  }
  void add_edge(int from, int to, T cost, bool undirected = false){
    assert(0 <= from && from < n);
    assert(0 <= to && to < n);
    graph.push_back({from,to,cost});
    if(undirected) graph.push_back({to,from,cost});
  }
  pair<bool,vector<T>> distance(int s){
    vector<T> dis(n,infty());
    dis[s] = e();
    for(int _ = 0; _ < n-1; _++){
      for(Edge edge:graph){
        dis[edge.to] = min(dis[edge.to], op(dis[edge.from], edge.cost));
      }
    }
    for(Edge edge:graph){
      if(dis[edge.to] > op(dis[edge.from], edge.cost)){
        return {false,dis}; 
      }
    }
    return {true,dis};
  }
};
 
long long  e() {return 0;}
long long  op(long long  x, long long  y) {return x+y;}
long long  infty() {return 1ll << 60;}
using BellmanFordll = BellmanFord<long long, e, infty, op>;

}  // namespace soryuusi
