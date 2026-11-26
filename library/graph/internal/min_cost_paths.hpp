// https://github.com/soryuusi0219/compro/blob/main/library/graph/internal/min_cost_paths.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi::internal {

template <class Price> struct MinCostPaths {
  struct Item {
    Price distance;
    int vertex;
    bool operator<(const Item& other) const { return distance > other.distance; }
  };

  std::vector<Price> price, distance;
  std::vector<int> parent, settled, zero;
  std::vector<unsigned char> visited;
  std::vector<Item> heap;
  Price inf, origin;

  MinCostPaths(int n, Price inf, Price origin = 0) : price(n,origin), distance(n), parent(n), visited(n), inf(inf), origin(origin) {}

  // emit(to, edge_id, cost_callback) for each positive-capacity residual arc from v.
  template <class Scan> bool initialize(const Scan& scan) {
    std::fill(price.begin(),price.end(),origin);
    const int n = int(price.size());
    std::vector<int> indegree(n);
    bool negative = false;
    for(int v = 0; v < n; ++v) scan(v,[&](int u, int, const auto& edge_cost) {
      ++indegree[u];
      negative |= edge_cost() < origin;
    });
    if(!negative) return true;

    std::vector<int> queue;
    for(int v = 0; v < n; ++v) if(indegree[v] == 0) queue.push_back(v);
    for(int i = 0; i < int(queue.size()); ++i) {
      const int v = queue[i];
      scan(v,[&](int u, int, const auto& edge_cost) {
        const Price candidate = price[v]+edge_cost();
        if(candidate < price[u]) price[u] = candidate;
        if(--indegree[u] == 0) queue.push_back(u);
      });
    }
    if(int(queue.size()) == n) return true;

    // Kahn's pass only applies to DAGs. All vertices have a zero-cost super-source arc.
    std::fill(price.begin(),price.end(),origin);
    for(int i = 0; i < n; ++i) {
      bool changed = false;
      for(int v = 0; v < n; ++v) scan(v,[&](int u, int, const auto& edge_cost) {
        const Price candidate = price[v]+edge_cost();
        if(candidate < price[u]) {
          price[u] = candidate;
          changed = true;
        }
      });
      if(!changed) return true;
    }
    return false;
  }

  template <class Scan> bool search(int s, int t, const Scan& scan) {
    std::fill(distance.begin(),distance.end(),inf);
    for(int v : settled) visited[v] = false;
    settled.clear();
    heap.clear();
    zero.clear();
    zero.push_back(s);
    distance[s] = origin;
    while(!zero.empty() || !heap.empty()) {
      int v;
      if(!zero.empty()) {
        v = zero.back();
        zero.pop_back();
      } else {
        std::pop_heap(heap.begin(),heap.end());
        v = heap.back().vertex;
        heap.pop_back();
      }
      if(visited[v]) continue;
      visited[v] = true;
      settled.push_back(v);
      if(v == t) {
        for(int u : settled) price[u] -= distance[t]-distance[u];
        return true;
      }
      const Price d = distance[v], p = price[v];
      scan(v,[&](int u, int id, const auto& edge_cost) {
        if(visited[u]) return;
        const Price reduced = edge_cost()+p-price[u];
        if(distance[u]-d <= reduced) return;
        distance[u] = d+reduced;
        parent[u] = id;
        if(reduced == origin) zero.push_back(u);
        else {
          heap.push_back({distance[u],u});
          std::push_heap(heap.begin(),heap.end());
        }
      });
    }
    return false;
  }
};

} // namespace soryuusi::internal
