// https://github.com/soryuusi0219/compro/blob/main/library/graph/min_cost_b_flow.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "bounded_flow.hpp"

namespace soryuusi {

template <class Cap = long long, class Cost = long long, class Total = __int128_t> class MinCostBFlow {
  static_assert(std::numeric_limits<Cap>::is_integer && std::numeric_limits<Cap>::is_signed && sizeof(Cap) <= 8);
  static_assert(std::numeric_limits<Cost>::is_integer && std::numeric_limits<Cost>::is_signed && sizeof(Cost) <= 8);
  static_assert(std::numeric_limits<Total>::is_integer && std::numeric_limits<Total>::is_signed && sizeof(Total) <= 16);
  using Wide = __int128_t;
  using Residual = std::make_unsigned_t<Cap>;

  struct Edge {
    int from, to;
    Cap lower, upper;
    Cost cost;
  };

  int n;
  std::vector<Edge> edges;

  template <class Price> void optimize(std::vector<Cap>& flow, std::vector<Wide>& potential, Price epsilon, bool use_global) const {
    struct Arc {
      int to, rev;
      Residual cap;
      Price cost;
    };
    std::vector<std::vector<Arc>> graph(n);
    std::vector<int> degree(n,0);
    for(const auto& e : edges) if(e.from != e.to && e.lower < e.upper) {
      ++degree[e.from];
      ++degree[e.to];
    }
    for(int v = 0; v < n; ++v) graph[v].reserve(degree[v]);

    // Scaling by n+1 makes 1-optimality sufficient for integral costs.
    const Wide scale = Wide(n)+1;
    for(int i = 0; i < int(edges.size()); ++i) {
      const auto& e = edges[i];
      if(e.from == e.to) {
        flow[i] = e.cost < 0 ? e.upper : e.lower;
        continue;
      }
      if(e.lower == e.upper) continue;
      const Price cost = Price(Wide(e.cost)*scale);
      auto& from = graph[e.from];
      auto& to = graph[e.to];
      const int rev_from = int(from.size()), rev_to = int(to.size());
      from.push_back({e.to,rev_to,Residual(Wide(e.upper)-flow[i]),cost});
      to.push_back({e.from,rev_from,Residual(Wide(flow[i])-e.lower),-cost});
    }

    struct Label {
      Price distance;
      int vertex;
      bool operator<(const Label& rhs) const { return distance > rhs.distance; }
    };
    std::vector<Price> price(n,0), tentative(n);
    std::vector<Label> heap;
    std::vector<Price> excess(n,0);
    std::vector<int> current(n,0), active;
    active.reserve(n);
    const auto push = [&](int v, Arc& e, Residual f) {
      e.cap -= f;
      graph[e.to][e.rev].cap += f;
      excess[v] -= Price(f);
      excess[e.to] += Price(f);
    };

    const auto repair_prices = [&](Price slack) {
      tentative = price;
      for(int pass = 0; pass < 3; ++pass) {
        bool changed = false;
        for(int v = 0; v < n; ++v) {
          const Price p = tentative[v]+slack;
          for(const auto& e : graph[v]) if(e.cap && p+e.cost < tentative[e.to]) {
            tentative[e.to] = p+e.cost;
            changed = true;
          }
        }
        if(!changed) {
          price.swap(tentative);
          return true;
        }
      }
      return false;
    };

    const auto global_price = [&]() {
      const Price inf = std::numeric_limits<Price>::max();
      std::fill(tentative.begin(),tentative.end(),inf);
      heap.clear();
      int remaining = 0;
      for(int v = 0; v < n; ++v) {
        if(excess[v] > 0) ++remaining;
        if(excess[v] < 0) {
          tentative[v] = 0;
          heap.push_back({0,v});
        }
      }
      Price last = 0;
      while(remaining > 0) {
        assert(!heap.empty());
        std::pop_heap(heap.begin(),heap.end());
        const auto [d,v] = heap.back();
        heap.pop_back();
        if(d != tentative[v]) continue;
        last = d;
        if(excess[v] > 0 && --remaining == 0) break;
        for(const auto& e : graph[v]) {
          const auto& rev = graph[e.to][e.rev];
          if(!rev.cap) continue;
          const Price reduced = rev.cost+price[e.to]-price[v];
          const Price length = reduced < 0 ? 0 : reduced/epsilon+1;
          const Price nd = d+length;
          if(nd >= tentative[e.to]) continue;
          tentative[e.to] = nd;
          heap.push_back({nd,e.to});
          std::push_heap(heap.begin(),heap.end());
        }
      }
      for(int v = 0; v < n; ++v) {
        price[v] -= epsilon*std::min(last,tentative[v]);
        current[v] = 0;
      }
    };

    while(true) {
      if(repair_prices(1)) break;
      epsilon = std::max<Price>(1,epsilon/8);
      for(int v = 0; v < n; ++v) {
        for(auto& e : graph[v]) {
          if(e.cap && e.cost+price[v] < price[e.to]) push(v,e,e.cap);
        }
        current[v] = 0;
      }
      for(int v = 0; v < n; ++v) if(excess[v] > 0) active.push_back(v);

      const std::size_t threshold = 4*edges.size()+std::size_t(n);
      std::size_t work = 0;
      while(!active.empty()) {
        const int v = active.back();
        active.pop_back();
        int& i = current[v];
        auto& out = graph[v];
        while(excess[v] > 0) {
          if(i == int(out.size())) {
            Price next = std::numeric_limits<Price>::min();
            for(const auto& e : out) if(e.cap) next = std::max(next,price[e.to]-e.cost);
            assert(next != std::numeric_limits<Price>::min());
            price[v] = next-epsilon;
            i = 0;
            work += out.size();
            if(use_global && work >= threshold) {
              global_price();
              work = 0;
            }
            continue;
          }
          auto& e = out[i];
          if(!e.cap || e.cost+price[v] >= price[e.to]) {
            ++work;
            ++i;
            continue;
          }
          const Price before = excess[e.to];
          push(v,e,Residual(std::min(excess[v],Price(e.cap))));
          if(before <= 0 && excess[e.to] > 0) active.push_back(e.to);
        }
      }
      if(epsilon == 1) break;
    }

    // Choose a global shift that puts no vertex on a rounding boundary.
    // Then flooring the prices also preserves every residual arc of reduced cost -1.
    std::vector<unsigned char> used(std::size_t(n)+1,0);
    for(Price p : price) used[std::size_t((Wide(p)%scale+scale)%scale)] = 1;
    const Wide shift = scale-(std::find(used.begin(),used.end(),0)-used.begin());
    for(int v = 0; v < n; ++v) {
      const Wide p = Wide(price[v])+shift;
      potential[v] = p/scale-(p%scale < 0);
      degree[v] = 0;
    }
    for(int i = 0; i < int(edges.size()); ++i) {
      const auto& e = edges[i];
      if(e.from == e.to || e.lower == e.upper) continue;
      const auto& a = graph[e.from][degree[e.from]++];
      ++degree[e.to];
      flow[i] = Cap(Wide(e.upper)-a.cap);
    }
  }

public:
  struct Result {
    Total cost;
    std::vector<Cap> flow;
    std::vector<Wide> potential;
  };

  explicit MinCostBFlow(int n) : n(n) { assert(0 <= n && n <= std::numeric_limits<int>::max()-2); }

  void reserve_edges(int m) {
    assert(0 <= m && m <= std::numeric_limits<int>::max()/2);
    edges.reserve(m);
  }

  int add_edge(int from, int to, Cap lower, Cap upper, Cost cost) {
    assert(0 <= from && from < n && 0 <= to && to < n && lower <= upper);
    assert(edges.size() < std::numeric_limits<int>::max()/2);
    const int id = int(edges.size());
    edges.push_back({from,to,lower,upper,cost});
    return id;
  }

  std::optional<Result> solve(const std::vector<Cap>& supply = {}) const {
    assert(supply.empty() || supply.size() == std::size_t(n));
    std::vector<Wide> balance(n,0);
    if(!supply.empty()) std::copy(supply.begin(),supply.end(),balance.begin());
    std::vector<Cap> flow;
    flow.reserve(edges.size());
    Wide largest = 0, span = 0;
    for(const auto& e : edges) {
      const Cap f = e.cost < 0 ? e.upper : e.lower;
      balance[e.from] -= f;
      balance[e.to] += f;
      flow.push_back(f);
      if(e.from != e.to && e.lower < e.upper) {
        largest = std::max(largest,e.cost < 0 ? -Wide(e.cost) : Wide(e.cost));
        span += Wide(e.upper)-e.lower;
      }
    }

    std::vector<Wide> potential(n,0);
    if(std::any_of(balance.begin(),balance.end(),[](Wide b) { return b != 0; })) {
      auto feasible = [&]() {
        BoundedFlow<Cap> graph(n);
        graph.reserve_edges(int(edges.size()));
        for(const auto& e : edges) graph.add_edge(e.from,e.to,e.lower,e.upper);
        return graph.solve(supply);
      }();
      if(!feasible) return std::nullopt;
      flow = std::move(*feasible);
      if(largest) {
        const Wide scale = Wide(n)+1, epsilon = largest*scale;
        const bool use_global = n >= 256 && edges.size() <= 8*std::size_t(n) && epsilon <= std::numeric_limits<Wide>::max()/(16*scale*scale);
        // Across global updates an inactive vertex can move as much as all active vertices combined.
        const Wide headroom = 16*scale*(use_global ? scale : 1);
        constexpr auto small_max = std::numeric_limits<std::int64_t>::max();
        if(span <= small_max && epsilon <= small_max/headroom) {
          optimize(flow,potential,static_cast<std::int64_t>(epsilon),use_global);
        } else {
          optimize(flow,potential,epsilon,use_global);
        }
      } else {
        for(int i = 0; i < int(edges.size()); ++i) if(edges[i].from == edges[i].to && edges[i].cost < 0) flow[i] = edges[i].upper;
      }
    }

    Wide cost = 0;
    for(int i = 0; i < int(edges.size()); ++i) {
      if(__builtin_add_overflow(cost,Wide(flow[i])*edges[i].cost,&cost)) throw std::overflow_error("MinCostBFlow: total cost overflow");
    }
    if(cost < std::numeric_limits<Total>::min() || cost > std::numeric_limits<Total>::max()) throw std::overflow_error("MinCostBFlow: total cost overflow");
    return Result{Total(cost),std::move(flow),std::move(potential)};
  }
};

} // namespace soryuusi
