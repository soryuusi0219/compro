// https://github.com/soryuusi0219/compro/blob/main/library/graph/dijkstra.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>
#include <limits>

#include "../math/internal/internal_bit.hpp"
#include "static_graph.hpp"

namespace soryuusi {

using namespace std;

namespace internal {

  template<class T>
  struct dijkstra_sum {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return x+y;};
    static constexpr T (*e)() = []()->T {return T(0);};
    static constexpr T (*infty)() = []()->T {return numeric_limits<T>::max()/2;};
  };

}  // namespace internal

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
    if constexpr (use_radix_heap()) distance_radix(v);
    else distance_binary(v);
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

private:
  static constexpr bool use_radix_heap(){
    if constexpr (is_integral_v<T> && sizeof(T) <= sizeof(uint64_t) && is_same_v<decltype(op),T(*)(T,T)> && is_same_v<decltype(e),T(*)()> && is_same_v<decltype(infty),T(*)()>){
      using Sum = internal::dijkstra_sum<T>;
      return op == Sum::op && e == Sum::e && infty == Sum::infty;
    }else return false;
  }
  void distance_binary(int v){
    priority_queue<pair<T,int>, vector<pair<T,int>>, greater<pair<T,int>>> que;
    d[v] = e();
    que.push({e(),v});
    while(!que.empty()){
      auto [d0,u] = que.top(); que.pop();
      if(d[u] < d0) continue;
      for(const auto& t : G[u]){
        const T candidate = op(d0,t.cost);
        if(d[t.to] > candidate){
          d[t.to] = candidate;
          from[t.to] = u;
          que.emplace(candidate,t.to);
        }
      }
    }
  }
  void distance_radix(int v){
    using Key = uint64_t;
    array<vector<pair<Key,int>>,64> buckets;
    Key last = 0, occupied = 0;
    auto push = [&](Key key, int to){
      const int bucket = soryuusi::bit_width(key^last);
      buckets[bucket].emplace_back(key,to);
      occupied |= Key{1}<<bucket;
    };
    d[v] = T(0);
    push(0,v);
    while(occupied){
      if(!(occupied&1)){
        const int k = soryuusi::countr_zero(occupied);
        auto& bucket = buckets[k];
        last = min_element(bucket.begin(),bucket.end())->first;
        occupied ^= Key{1}<<k;
        for(auto [key,to] : bucket) push(key,to);
        bucket.clear();
      }
      auto [d0,u] = buckets[0].back();
      buckets[0].pop_back();
      if(buckets[0].empty()) occupied ^= 1;
      if(static_cast<Key>(d[u]) != d0) continue;
      for(const auto& edge : G[u]){
        const Key candidate = d0+static_cast<Key>(edge.cost);
        if constexpr (is_unsigned_v<T> && sizeof(T) == sizeof(Key)) if(candidate < d0) continue;
        if(candidate < static_cast<Key>(d[edge.to])){
          d[edge.to] = static_cast<T>(candidate);
          from[edge.to] = u;
          push(candidate,edge.to);
        }
      }
    }
  }
};


template<typename T>
using Dijkstra_sum = Dijkstra<T,internal::dijkstra_sum<T>::op,internal::dijkstra_sum<T>::e,internal::dijkstra_sum<T>::infty>;
using Dijkstrall = Dijkstra_sum<long long>;

}  // namespace soryuusi
