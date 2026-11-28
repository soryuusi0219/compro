// https://github.com/soryuusi0219/compro/blob/main/library/graph/min_cost_flow.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "internal/min_cost_paths.hpp"

namespace soryuusi {

template <class Cap = long long, class Cost = long long, class Total = __int128_t> class MinCostFlow {
  static_assert(std::numeric_limits<Cap>::is_integer && !std::is_same_v<Cap,bool> && sizeof(Cap) <= 8);
  static_assert(std::numeric_limits<Cost>::is_integer && std::numeric_limits<Cost>::is_signed && sizeof(Cost) <= 8);
  static_assert(std::numeric_limits<Total>::is_integer && std::numeric_limits<Total>::is_signed && sizeof(Total) <= 16);
  using Price = __int128_t;

  struct Arc {
    int to;
    Cap cap;
  };

  int n;
  bool initialized = false, has_negative = false, has_flow = false;
  std::vector<Arc> arcs;
  std::vector<Cost> costs;
  std::vector<int> start, order;
  internal::MinCostPaths<Price> shortest;

  Price arc_cost(int e) const {
    const Price cost = costs[e/2];
    return e%2 == 0 ? cost : -cost;
  }

  void build_adjacency() {
    if(!start.empty()) return;
    order.resize(arcs.size());
    start.assign(std::size_t(n)+1,0);
    for(const auto& arc : arcs) ++start[arc.to+1];
    for(int v = 0; v < n; ++v) start[v+1] += start[v];
    for(int i = 0; i < int(arcs.size()); ++i) order[start[arcs[i^1].to]++] = i;
    std::move_backward(start.begin(),start.end()-1,start.end());
    start[0] = 0;
  }

  void push(int e, Cap amount) {
    arcs[e].cap -= amount;
    arcs[e^1].cap += amount;
  }

  Cap zero_cost_flow(int s, int t, Cap limit, std::vector<int>& level, std::vector<int>& current, std::vector<int>& work, std::vector<int>& path) {
    const auto admissible = [&](int v, int e) { return arcs[e].cap > 0 && arc_cost(e)+shortest.price[v] == shortest.price[arcs[e].to]; };
    Cap sent = 0;
    level.resize(n);
    current.resize(n);
    while(sent < limit) {
      std::fill(level.begin(),level.end(),-1);
      level[t] = 0;
      work.clear();
      work.push_back(t);
      for(int i = 0; i < int(work.size()) && level[s] < 0; ++i) {
        const int v = work[i];
        for(int j = start[v]; j < start[v+1]; ++j) {
          const int e = order[j], u = arcs[e].to;
          if(level[u] >= 0 || !admissible(u,e^1)) continue;
          level[u] = level[v]+1;
          if(u == s) break;
          work.push_back(u);
        }
      }
      if(level[s] < 0) break;

      std::copy_n(start.begin(),n,current.begin());
      path.clear();
      int v = s;
      while(sent < limit) {
        if(v == t) {
          Cap amount = limit-sent;
          for(int e : path) amount = std::min(amount,arcs[e].cap);
          for(int e : path) push(e,amount);
          sent += amount;
          path.clear();
          v = s;
          continue;
        }
        int& i = current[v];
        while(i < start[v+1] && (level[arcs[order[i]].to] != level[v]-1 || !admissible(v,order[i]))) ++i;
        if(i < start[v+1]) {
          path.push_back(order[i]);
          v = arcs[order[i]].to;
        } else {
          level[v] = -1;
          if(path.empty()) break;
          v = arcs[path.back()^1].to;
          ++current[v];
          path.pop_back();
        }
      }
    }
    return sent;
  }

  template <class Emit> std::pair<Cap,Total> run(int s, int t, Cap limit, const Emit& emit) {
    assert(0 <= s && s < n && 0 <= t && t < n && s != t && limit >= 0);
    if(limit == 0 || arcs.empty()) return {0,0};
    build_adjacency();
    const auto scan = [&](int v, const auto& relax) {
      for(int j = start[v]; j < start[v+1]; ++j) {
        const int e = order[j];
        if(arcs[e].cap > 0) relax(arcs[e].to,e,[&] { return arc_cost(e); });
      }
    };
    if(!initialized) {
      if((has_negative || has_flow) && !shortest.initialize(scan)) throw std::domain_error("MinCostFlow: negative residual cycle");
      initialized = true;
    }

    Cap sent = 0;
    Total total = 0;
    Price prev = 0;
    std::vector<int> level, current, work, path;
    while(sent < limit && shortest.search(s,t,scan)) {
      const Price marginal = shortest.price[t]-shortest.price[s];
      Cap amount = limit-sent;
      for(int u = t; u != s; u = arcs[shortest.parent[u]^1].to) amount = std::min(amount,arcs[shortest.parent[u]].cap);
      for(int u = t; u != s; u = arcs[shortest.parent[u]^1].to) push(shortest.parent[u],amount);
      has_flow = true;
      if(sent > 0 && prev == marginal && amount < limit-sent) {
        amount += zero_cost_flow(s,t,Cap(limit-sent-amount),level,current,work,path);
      }
      Price extra;
      if(__builtin_mul_overflow(Price(amount),marginal,&extra) || __builtin_add_overflow(total,extra,&total)) {
        throw std::overflow_error("MinCostFlow: total cost overflow");
      }
      sent += amount;
      emit(sent,total,sent > amount && prev == marginal);
      prev = marginal;
    }
    return {sent,total};
  }

public:
  struct Edge {
    int from, to;
    Cap cap, flow;
    Cost cost;
  };

  explicit MinCostFlow(int n = 0) : n(n), shortest(n,std::numeric_limits<Price>::max()) { assert(n >= 0); }

  void reserve_edges(int m) {
    assert(0 <= m && m <= std::numeric_limits<int>::max()/2);
    arcs.reserve(2*m);
    costs.reserve(m);
  }

  int add_edge(int from, int to, Cap cap, Cost cost) {
    assert(0 <= from && from < n && 0 <= to && to < n && cap >= 0);
    assert(costs.size() < std::numeric_limits<int>::max()/2);
    const int id = int(costs.size());
    costs.push_back(cost);
    try {
      arcs.resize(2*id+2);
    } catch(...) {
      costs.pop_back();
      throw;
    }
    arcs[2*id] = {to,cap};
    arcs[2*id+1] = {from,0};
    has_negative |= cap > 0 && cost < 0;
    initialized = false;
    start.clear();
    return id;
  }

  Edge get_edge(int id) const {
    assert(0 <= id && id < int(costs.size()));
    const auto& a = arcs[2*id];
    const auto& b = arcs[2*id+1];
    return {b.to,a.to,Cap(a.cap+b.cap),b.cap,costs[id]};
  }

  std::vector<Edge> edges() const {
    std::vector<Edge> answer;
    answer.reserve(costs.size());
    for(int id = 0; id < int(costs.size()); ++id) answer.push_back(get_edge(id));
    return answer;
  }

  void reset() {
    for(int e = 0; e < int(arcs.size()); e += 2) {
      arcs[e].cap += arcs[e+1].cap;
      arcs[e+1].cap = 0;
    }
    std::fill(shortest.price.begin(),shortest.price.end(),0);
    initialized = has_flow = false;
  }

  std::pair<Cap,Total> flow(int s, int t, Cap limit = std::numeric_limits<Cap>::max()) {
    return run(s,t,limit,[](Cap, Total, bool) {});
  }

  std::vector<std::pair<Cap,Total>> slope(int s, int t, Cap limit = std::numeric_limits<Cap>::max()) {
    std::vector<std::pair<Cap,Total>> answer{{0,0}};
    run(s,t,limit,[&](Cap sent, Total cost, bool same_marginal) {
      if(same_marginal) answer.back() = {sent,cost};
      else answer.emplace_back(sent,cost);
    });
    return answer;
  }
};

} // namespace soryuusi
