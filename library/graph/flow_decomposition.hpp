// https://github.com/soryuusi0219/compro/blob/main/library/graph/flow_decomposition.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>
#include "max_flow.hpp"

namespace soryuusi {

template <class Cap> struct FlowComponent {
  Cap amount;
  std::vector<int> edge_ids;
  bool is_cycle;
};

template <class Cap> std::vector<FlowComponent<Cap>> decompose_flow(int n, const std::vector<FlowEdge<Cap>>& edges, int s = -1, int t = -1) {
  static_assert(std::numeric_limits<Cap>::is_integer && !std::is_same_v<Cap,bool> && sizeof(Cap) <= 8);
  assert(0 <= n && n < std::numeric_limits<int>::max());
  assert((s == -1 && t == -1) || (0 <= s && s < n && 0 <= t && t < n && s != t));
  assert(edges.size() <= std::numeric_limits<int>::max()/2);

  using Wide = __int128_t;
  const Wide cap_max = std::numeric_limits<Cap>::max();
  Wide value = 0;
  std::vector<int> offset(n+1,0);
#ifndef NDEBUG
  std::vector<Wide> balance(n,0);
#endif
  for(const auto& e : edges) {
    assert(0 <= e.from && e.from < n && 0 <= e.to && e.to < n && 0 <= e.flow && e.flow <= e.cap);
    if(e.flow == 0) continue;
    ++offset[e.from+1];
    if(e.from == s) value += e.flow;
    if(e.to == s) value -= e.flow;
#ifndef NDEBUG
    balance[e.from] += e.flow;
    balance[e.to] -= e.flow;
#endif
  }
  assert(value >= 0);
#ifndef NDEBUG
  for(int v = 0; v < n; ++v) assert(balance[v] == (v == s ? value : v == t ? -value : 0));
#endif
  if(value > 0) offset[t+1] += int((value+cap_max-1)/cap_max);
  for(int v = 0; v < n; ++v) offset[v+1] += offset[v];

  struct Arc {
    int to, id;
    Cap remaining;
  };
  std::vector<Arc> arcs(offset[n]);
  auto current = offset;
  for(int id = 0; id < int(edges.size()); ++id) {
    const auto& e = edges[id];
    if(e.flow > 0) arcs[current[e.from]++] = {e.to,id,e.flow};
  }
  // Close each s-t path into a cycle. Split the return capacity if the total does not fit in Cap.
  while(value > 0) {
    const Cap amount = Cap(std::min(value,cap_max));
    arcs[current[t]++] = {s,-1,amount};
    value -= amount;
  }
  std::copy(offset.begin(),offset.end(),current.begin());

  std::vector<int> position(n,-1), vertices, path;
  std::vector<FlowComponent<Cap>> result;
  for(int start = 0; start < n; ++start) {
    int v = start;
    vertices.push_back(start);
    position[start] = 0;
    while(true) {
      int& i = current[v];
      while(i < offset[v+1] && arcs[i].remaining == 0) ++i;
      if(i == offset[v+1]) {
        assert(path.empty());
        break;
      }
      const int to = arcs[i].to;
      const int begin = position[to];
      if(begin < 0) {
        path.push_back(i);
        position[to] = int(vertices.size());
        vertices.push_back(to);
        v = to;
        continue;
      }

      const int end = int(path.size());
      Cap amount = arcs[i].remaining;
      for(int j = begin; j < end; ++j) amount = std::min(amount,arcs[path[j]].remaining);
      auto& component = result.emplace_back(FlowComponent<Cap>{amount,{},true});
      auto& ids = component.edge_ids;
      ids.reserve(end-begin+1);
      int keep = end, return_position = -1;
      for(int j = begin; j <= end; ++j) {
        auto& arc = arcs[j == end ? i : path[j]];
        arc.remaining -= amount;
        if(arc.remaining == 0 && j < keep) keep = j;
        if(arc.id == -1) return_position = int(ids.size());
        else ids.push_back(arc.id);
      }
      if(return_position != -1) {
        component.is_cycle = false;
        std::rotate(ids.begin(),ids.begin()+return_position,ids.end());
      }

      // The prefix up to the first exhausted arc is still positive and can be reused.
      while(int(vertices.size()) > keep+1) {
        position[vertices.back()] = -1;
        vertices.pop_back();
      }
      path.resize(keep);
      v = vertices.back();
    }
    position[start] = -1;
    vertices.clear();
  }
  return result;
}

} // namespace soryuusi
