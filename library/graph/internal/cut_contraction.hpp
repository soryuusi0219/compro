// https://github.com/soryuusi0219/compro/blob/main/library/graph/internal/cut_contraction.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

#include "../undirected_cut_edge.hpp"

namespace soryuusi::internal {

template <class Cap> class CutContraction {
  struct Arc {
    int to, next, prev;
    Cap cap;
  };
  std::vector<Arc> arcs;
  std::vector<int> head, slot, stamp;

  void link(int u, int e) {
    const int old = head[u];
    arcs[e].next = old;
    arcs[e].prev = -1;
    if(old >= 0) arcs[old].prev = e;
    head[u] = e;
  }

  void unlink(int u, int e) {
    const int next = arcs[e].next, prev = arcs[e].prev;
    if(prev >= 0) arcs[prev].next = next;
    else head[u] = next;
    if(next >= 0) arcs[next].prev = prev;
  }

public:
  std::vector<Cap> degree;

  CutContraction(int n, const std::vector<UndirectedCutEdge<Cap>>& edges) : head(n,-1), slot(n), stamp(n,-1), degree(n) {
    assert(edges.size() <= std::size_t(std::numeric_limits<int>::max()/2));
    arcs.reserve(2*edges.size());
    for(auto [u,v,w] : edges) {
      assert(0 <= u && u < n && 0 <= v && v < n && w >= 0);
      if(u == v || w == 0) continue;
      const int e = static_cast<int>(arcs.size());
      arcs.push_back({v,-1,-1,w});
      arcs.push_back({u,-1,-1,w});
      link(u,e);
      link(v,e^1);
      degree[u] += w;
      degree[v] += w;
    }
    // Coalesce parallel edges once so that ordering scans only distinct neighbors.
    for(int u = 0; u < n; ++u) {
      for(int e = head[u]; e >= 0;) {
        const int v = arcs[e].to, next = arcs[e].next;
        if(v > u) {
          if(stamp[v] == u) {
            const int keep = slot[v];
            arcs[keep].cap += arcs[e].cap;
            arcs[keep^1].cap += arcs[e].cap;
            unlink(u,e);
            unlink(v,e^1);
          } else {
            stamp[v] = u;
            slot[v] = e;
          }
        }
        e = next;
      }
    }
    std::fill(stamp.begin(),stamp.end(),-1);
  }

  template <class Emit> void for_each(int u, Emit&& emit) const {
    for(int e = head[u]; e >= 0; e = arcs[e].next) emit(arcs[e].to,arcs[e].cap);
  }

  void contract(int u, int v) {
    // The vertex absorbed in this round is unique, so it can serve as a timestamp.
    for(int e = head[u]; e >= 0; e = arcs[e].next) {
      const int x = arcs[e].to;
      stamp[x] = v;
      slot[x] = e;
    }
    Cap between = 0;
    for(int e = head[v]; e >= 0;) {
      const int x = arcs[e].to, next = arcs[e].next;
      const Cap w = arcs[e].cap;
      if(x == u) {
        between += w;
        unlink(u,e^1);
      } else if(stamp[x] == v) {
        arcs[slot[x]].cap += w;
        arcs[slot[x]^1].cap += w;
        unlink(x,e^1);
      } else {
        link(u,e);
        arcs[e^1].to = u;
        stamp[x] = v;
        slot[x] = e;
      }
      e = next;
    }
    head[v] = -1;
    degree[u] = (degree[u]-between)+(degree[v]-between);
  }
};

// Indexed binary heap with only priority improvements between extractions.
template <class Cap, bool Minimum> class CutOrderingHeap {
  std::vector<int> order, position;
  std::vector<Cap> key;

  bool before(int u, int v) const {
    if constexpr(Minimum) return key[u] < key[v];
    else return key[v] < key[u];
  }

  void put(int i, int u) {
    order[i] = u;
    position[u] = i;
  }

  void down(int i, int u) {
    const int n = static_cast<int>(order.size());
    while(i < n/2) {
      int j = 2*i+1;
      if(j+1 < n && before(order[j+1],order[j])) ++j;
      if(!before(order[j],u)) break;
      put(i,order[j]);
      i = j;
    }
    put(i,u);
  }

public:
  explicit CutOrderingHeap(int n) : position(n,-1), key(n) { order.reserve(n); }

  void reset(const std::vector<int>& active, const Cap* initial = nullptr) {
    order = active;
    for(int i = 0; i < static_cast<int>(order.size()); ++i) {
      const int v = order[i];
      position[v] = i;
      key[v] = initial ? initial[v] : Cap(0);
    }
    if(initial) for(int i = static_cast<int>(order.size())/2; i-- > 0;) down(i,order[i]);
  }

  int size() const { return static_cast<int>(order.size()); }
  int top() const { return order[0]; }
  Cap value(int u) const { return key[u]; }

  int pop() {
    const int u = order.front(), last = order.back();
    order.pop_back();
    position[u] = -1;
    if(!order.empty()) down(0,last);
    return u;
  }

  void improve(int u, Cap delta) {
    int i = position[u];
    if(i < 0) return;
    if constexpr(Minimum) key[u] -= delta;
    else key[u] += delta;
    const int start = i;
    while(i && before(u,order[(i-1)/2])) {
      const int parent = (i-1)/2;
      put(i,order[parent]);
      i = parent;
    }
    if(i != start) put(i,u);
  }
};

}  // namespace soryuusi::internal
