// https://github.com/soryuusi0219/compro/blob/main/library/graph/dulmage_mendelsohn_decomposition.hpp
#pragma once
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include "bipartite_matching.hpp"
#include "scc.hpp"
#include "static_graph.hpp"

namespace soryuusi {

struct DulmageMendelsohnDecomposition {
  enum class Region : unsigned char { right_deficient, balanced, left_deficient };

  std::vector<int> left_block, right_block;
  std::vector<Region> region;
  std::vector<bool> allowed, essential;

  bool unique() const {
    for(int i = 0; i < int(allowed.size()); ++i) if(allowed[i] && !essential[i]) return false;
    return true;
  }
};

inline DulmageMendelsohnDecomposition dulmage_mendelsohn_decomposition(BipartiteMatching& graph) {
  const int l = graph.left_size(), r = graph.right_size(), m = graph.edge_count();
  if(l >= std::numeric_limits<int>::max()-r) throw std::length_error("too many bipartite vertices");
  const int matched = graph.solve();
  if(m > std::numeric_limits<int>::max()-matched) throw std::length_error("too many alternating arcs");
  const auto cover = graph.minimum_vertex_cover();
  const auto& left_edge = graph.left_match_edges();

  // Reverse alternating reachability from the unmatched right vertices.
  auto reaches_free_right = [&] {
    if(matched == r) return std::vector<bool>(l+r,false);
    static_graph<> reverse(l+r,m+matched);
    for(const auto& [u,v] : graph.edges()) reverse.add_edge(l+v,u);
    for(int u = 0; u < l; ++u) if(left_edge[u] >= 0) reverse.add_edge(u,l+graph.left_partner(u));
    reverse.build();
    std::vector<bool> reached(l+r,false);
    std::vector<int> work;
    for(int v = 0; v < r; ++v) if(graph.right_partner(v) < 0) {
      reached[l+v] = true;
      work.push_back(l+v);
    }
    for(int i = 0; i < int(work.size()); ++i) {
      for(int v : reverse.vertices(work[i])) if(!reached[v]) {
        reached[v] = true;
        work.push_back(v);
      }
    }
    return reached;
  }();

  // Each balanced matched pair is one vertex of the graph for the fine SCCs.
  bool has_left_deficient = false, has_right_deficient = false;
  int pairs = 0;
  std::vector<int> pair(l,-1);
  for(int u = 0; u < l; ++u) {
    if(!cover.left[u]) has_left_deficient = true;
    else if(!reaches_free_right[u]) pair[u] = pairs++;
  }
  for(bool x : reaches_free_right) if(x) {
    has_right_deficient = true;
    break;
  }
  auto [blocks,scc_id] = [&] {
    SCC scc(pairs);
    for(const auto& [u,v] : graph.edges()) {
      if(pair[u] < 0 || cover.right[v] || reaches_free_right[l+v]) continue;
      const int w = graph.right_partner(v);
      if(u != w) scc.add_edge(pair[u],pair[w]);
    }
    return scc.scc_ids();
  }();

  using Region = DulmageMendelsohnDecomposition::Region;
  DulmageMendelsohnDecomposition answer;
  if(has_right_deficient) answer.region.push_back(Region::right_deficient);
  answer.region.insert(answer.region.end(),blocks,Region::balanced);
  const int last = int(answer.region.size());
  if(has_left_deficient) answer.region.push_back(Region::left_deficient);
  answer.left_block.resize(l);
  for(int u = 0; u < l; ++u) {
    answer.left_block[u] = !cover.left[u] ? last : reaches_free_right[u] ? 0 : has_right_deficient+scc_id[pair[u]];
  }
  answer.right_block.resize(r);
  for(int v = 0; v < r; ++v) {
    answer.right_block[v] = cover.right[v] ? last : reaches_free_right[l+v] ? 0 : answer.left_block[graph.right_partner(v)];
  }

  answer.allowed.resize(m);
  answer.essential.resize(m);
  std::vector<int> allowed_degree(l,0);
  for(int id = 0; id < m; ++id) {
    const auto [u,v] = graph.get_edge(id);
    const bool allowed = answer.left_block[u] == answer.right_block[v];
    answer.allowed[id] = allowed;
    allowed_degree[u] += allowed;
  }
  for(int u = 0; u < l; ++u) {
    if(left_edge[u] >= 0 && pair[u] >= 0 && allowed_degree[u] == 1) answer.essential[left_edge[u]] = true;
  }
  return answer;
}

} // namespace soryuusi
