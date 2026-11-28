// https://github.com/soryuusi0219/compro/blob/main/library/graph/gomory_hu_tree.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "../data_structure/dsu.hpp"
#include "max_flow.hpp"
#include "undirected_cut_edge.hpp"

namespace soryuusi {

enum class GomoryHuFlow { Dinic, PushRelabel };

template <class Cap = long long> class GomoryHuTree {
  std::vector<int> par, depth, in, subtree;
  std::vector<Cap> weight;

public:
  GomoryHuTree(int n, const std::vector<UndirectedCutEdge<Cap>>& edges, GomoryHuFlow method = GomoryHuFlow::Dinic) : par(n,0), depth(n), in(n), subtree(n,1), weight(n) {
    assert(n >= 0 && edges.size() <= std::size_t(std::numeric_limits<int>::max()/4));
    if(n == 0) return;
    par[0] = -1;
    DSU components(n);
    for(auto [u,v,w] : edges) {
      assert(0 <= u && u < n && 0 <= v && v < n && w >= 0);
      if(u != v && w > 0) components.merge(u,v);
    }
    auto groups = components.groups();
    std::vector<int> group(n), index(n);
    for(int i = 0; i < static_cast<int>(groups.size()); ++i) {
      const auto& vertices = groups[i];
      for(int j = 0; j < static_cast<int>(vertices.size()); ++j) {
        group[vertices[j]] = i;
        index[vertices[j]] = j;
      }
    }
    std::vector<std::vector<UndirectedCutEdge<Cap>>> grouped(groups.size());
    for(auto [u,v,w] : edges) if(u != v && w > 0) grouped[group[u]].push_back({index[u],index[v],w});
    for(int i = 0; i < static_cast<int>(groups.size()); ++i) {
      const auto& vertices = groups[i];
      const int k = static_cast<int>(vertices.size());
      if(k == 1) continue;
      if(grouped[i].size() == static_cast<std::size_t>(k-1)) {
        // The minimum edge on a weighted tree path already witnesses a minimum cut.
        std::vector<std::vector<std::pair<int,Cap>>> adjacent(k);
        for(auto [u,v,w] : grouped[i]) {
          adjacent[u].emplace_back(v,w);
          adjacent[v].emplace_back(u,w);
        }
        std::vector<int> order = {0};
        order.reserve(k);
        for(int j = 0; j < static_cast<int>(order.size()); ++j) {
          const int u = vertices[order[j]];
          for(auto [next,w] : adjacent[order[j]]) {
            const int v = vertices[next];
            if(v == par[u]) continue;
            par[v] = u;
            weight[v] = w;
            order.push_back(next);
          }
        }
        continue;
      }

      MaxFlow<Cap> graph(k);
      graph.reserve_edges(2*static_cast<int>(grouped[i].size()));
      for(auto [u,v,w] : grouped[i]) {
        graph.add_edge(u,v,w);
        graph.add_edge(v,u,w);
      }
      std::vector<int> parent(k,0);
      std::vector<Cap> cut(k);
      for(int s = 1; s < k; ++s) {
        const int t = parent[s];
        if(s > 1) graph.reset();
        cut[s] = method == GomoryHuFlow::Dinic ? graph.flow(s,t) : graph.flow_push_relabel(s,t);
        const auto side = graph.min_cut(s);
        // Process earlier children as well, to retain the cuts induced by tree edges.
        for(int v = 1; v < k; ++v) if(v != s && parent[v] == t && side[v]) parent[v] = s;
        if(t != 0 && side[parent[t]]) {
          parent[s] = parent[t];
          parent[t] = s;
          std::swap(cut[s],cut[t]);
        }
      }
      for(int v = 1; v < k; ++v) {
        par[vertices[v]] = vertices[parent[v]];
        weight[vertices[v]] = cut[v];
      }
    }

    std::vector<int> head(n,-1), next(n,-1), order;
    for(int v = 1; v < n; ++v) {
      next[v] = head[par[v]];
      head[par[v]] = v;
    }
    order.reserve(n);
    int v = 0;
    while(v >= 0) {
      in[v] = static_cast<int>(order.size());
      order.push_back(v);
      if(head[v] >= 0) {
        depth[head[v]] = depth[v]+1;
        v = head[v];
      } else {
        while(v >= 0 && next[v] < 0) v = par[v];
        if(v >= 0) {
          depth[next[v]] = depth[v];
          v = next[v];
        }
      }
    }
    for(int i = n-1; i > 0; --i) subtree[par[order[i]]] += subtree[order[i]];
  }

  int size() const { return static_cast<int>(par.size()); }
  int parent(int v) const { return par[v]; }
  Cap capacity(int child) const { assert(0 < child && child < size()); return weight[child]; }

  int min_cut_edge(int u, int v) const {
    assert(0 <= u && u < size() && 0 <= v && v < size() && u != v);
    int best = -1;
    while(u != v) {
      if(depth[u] < depth[v]) std::swap(u,v);
      if(best < 0 || weight[u] < weight[best]) best = u;
      u = par[u];
    }
    return best;
  }

  Cap min_cut_value(int u, int v) const { return weight[min_cut_edge(u,v)]; }

  std::vector<bool> cut_side(int child) const {
    assert(0 < child && child < size());
    std::vector<bool> side(size());
    for(int v = 0; v < size(); ++v) side[v] = in[child] <= in[v] && in[v] < in[child]+subtree[child];
    return side;
  }
};

}  // namespace soryuusi
