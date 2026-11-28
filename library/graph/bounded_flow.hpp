// https://github.com/soryuusi0219/compro/blob/main/library/graph/bounded_flow.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <vector>
#include "max_flow.hpp"

namespace soryuusi {

template <class Cap = long long> class BoundedFlow {
  static_assert(std::numeric_limits<Cap>::is_integer && std::numeric_limits<Cap>::is_signed && sizeof(Cap) <= 8);
  using Wide = __int128_t;

  struct Edge {
    int from, to;
    Cap lower, upper;
  };

  int n;
  std::vector<Edge> edges;

  std::optional<std::vector<Cap>> solve_impl(const std::vector<Cap>& supply, const Edge* return_edge) const {
    assert(supply.empty() || supply.size() == std::size_t(n));
    std::vector<Wide> balance(n);
    if(!supply.empty()) std::copy(supply.begin(),supply.end(),balance.begin());
    auto normalize = [&](const Edge& e) {
      balance[e.from] -= e.lower;
      balance[e.to] += e.lower;
    };
    for(const auto& e : edges) normalize(e);
    if(return_edge) normalize(*return_edge);

    Wide positive = 0, negative = 0;
    int auxiliary_edges = 0;
    for(Wide b : balance) {
      if(b > 0) positive += b;
      if(b < 0) negative -= b;
      auxiliary_edges += b != 0;
    }
    if(positive != negative) return std::nullopt;

    const int m = int(edges.size())+(return_edge != nullptr);
    std::vector<Cap> result;
    result.reserve(m);
    if(positive == 0) {
      for(const auto& e : edges) result.push_back(e.lower);
      if(return_edge) result.push_back(return_edge->lower);
      return result;
    }

    auto run = [&]<class Residual>() {
      MaxFlow<Residual> graph(n+2);
      const int source = n, sink = n+1;
      assert(m <= std::numeric_limits<int>::max()/2-auxiliary_edges);
      graph.reserve_edges(m+auxiliary_edges);
      auto add = [&](const Edge& e) {
        // An s-t flow of value positive can be chosen without any circulation.
        graph.add_edge(e.from,e.to,e.from == e.to ? 0 : Residual(std::min(positive,Wide(e.upper)-e.lower)));
      };
      for(const auto& e : edges) add(e);
      if(return_edge) add(*return_edge);
      for(int v = 0; v < n; ++v) {
        if(balance[v] > 0) graph.add_edge(source,v,Residual(balance[v]));
        if(balance[v] < 0) graph.add_edge(v,sink,Residual(-balance[v]));
      }
      if(graph.flow_push_relabel(source,sink) != positive) return false;
      for(int i = 0; i < m; ++i) {
        const Edge& e = i < int(edges.size()) ? edges[i] : *return_edge;
        result.push_back(Cap(Wide(e.lower)+graph.get_edge(i).flow));
      }
      return true;
    };
    if(positive <= std::numeric_limits<Cap>::max()) {
      if(!run.template operator()<Cap>()) return std::nullopt;
    } else if(!run.template operator()<Wide>()) {
      return std::nullopt;
    }
    return result;
  }

public:
  explicit BoundedFlow(int n) : n(n) {
    assert(0 <= n && n <= std::numeric_limits<int>::max()-2);
  }

  void reserve_edges(int m) {
    assert(0 <= m && m <= std::numeric_limits<int>::max()/2);
    edges.reserve(m);
  }

  int add_edge(int from, int to, Cap lower, Cap upper) {
    assert(0 <= from && from < n && 0 <= to && to < n && lower <= upper);
    assert(edges.size() < std::numeric_limits<int>::max()/2);
    const int id = int(edges.size());
    edges.push_back({from,to,lower,upper});
    return id;
  }

  std::optional<std::vector<Cap>> solve(const std::vector<Cap>& supply = {}) const {
    return solve_impl(supply,nullptr);
  }

  std::optional<std::pair<Cap,std::vector<Cap>>> solve(int s, int t, Cap lower, Cap upper) const {
    assert(0 <= s && s < n && 0 <= t && t < n && s != t && lower <= upper);
    const Edge return_edge{t,s,lower,upper};
    auto result = solve_impl({},&return_edge);
    if(!result) return std::nullopt;
    const Cap value = result->back();
    result->pop_back();
    return std::pair{value,std::move(*result)};
  }
};

} // namespace soryuusi
