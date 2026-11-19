// https://github.com/soryuusi0219/compro/blob/main/library/graph/directed_mst.hpp
#pragma once
#include <cassert>
#include <numeric>
#include <utility>
#include <vector>

#include "../data_structure/dsu_undo.hpp"
#include "../data_structure/skew_heap.hpp"

namespace soryuusi {

template<class T>
class DirectedMST {
  struct Edge {int from, to; T cost;};
  int n;
  std::vector<Edge> edges;

public:
  explicit DirectedMST(int n_, int edge_capacity = 0) : n(n_){
    assert(n > 0 && edge_capacity >= 0);
    edges.reserve(edge_capacity);
  }
  int add_edge(int from, int to, T cost){
    assert(0 <= from && from < n && 0 <= to && to < n);
    int id = edges.size();
    edges.push_back({from,to,cost});
    return id;
  }
  std::pair<T,std::vector<int>> solve(int root) const {
    assert(0 <= root && root < n);
    SkewHeap<T,int> arena;
    std::vector<typename SkewHeap<T,int>::Heap> heaps;
    heaps.reserve(2*n);
    for(int v = 0; v < n; ++v) heaps.push_back(arena.new_heap());
    for(int id = 0; id < (int)edges.size(); ++id){
      const auto& [from,to,cost] = edges[id];
      if(to != root && from != to) heaps[to].push(cost,id);
    }

    struct Contraction {int state, leader, old_component;};
    dsu_undo dsu(2*n);
    std::vector<int> component(2*n);
    std::iota(component.begin(),component.end(),0);
    auto current = [&](int v){return component[dsu.leader(v)];};
    std::vector<Contraction> contractions;
    std::vector<int> path, incoming(2*n,-1);
    std::vector<bool> seen(2*n,false);
    path.reserve(n);
    contractions.reserve(n-1);
    seen[root] = true;
    T total{};
    for(int u = 0; u < n; ++u){
      if(u == root || incoming[u] != -1) continue;
      int v = u;
      while(!seen[v]){
        if(incoming[v] != -1){
          int merged = heaps.size(), state = dsu.get_state();
          heaps.push_back(arena.new_heap());
          int w;
          do {
            w = path.back();
            path.pop_back();
            heaps[merged].meld(std::move(heaps[w]));
            dsu.merge(merged,w);
          } while(w != v);
          int leader = dsu.leader(merged);
          contractions.push_back({state,leader,component[leader]});
          component[leader] = merged;
          v = merged;
        }
        auto& heap = heaps[v];
        while(!heap.empty() && current(edges[heap.top().second].from) == v) heap.pop();
        if(heap.empty()) return {};
        auto [cost,id] = heap.top();
        incoming[v] = id;
        path.push_back(v);
        total += cost;
        heap.pop();
        heap.add_all(T{}-cost);
        v = current(edges[id].from);
      }
      for(int w : path) seen[w] = true;
      path.clear();
    }
    for(int v = (int)heaps.size()-1; v >= n; --v){
      auto [state,leader,old_component] = contractions.back();
      contractions.pop_back();
      dsu.rollback(state);
      component[leader] = old_component;
      incoming[current(edges[incoming[v]].to)] = incoming[v];
    }
    incoming.resize(n);
    return {total,std::move(incoming)};
  }
};

}  // namespace soryuusi
