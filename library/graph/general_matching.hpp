// https://github.com/soryuusi0219/compro/blob/main/library/graph/general_matching.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

class GeneralMatching {
public:
  struct Edge { int u, v; };

private:
  struct Arc { int to, id; };
  int n, cardinality = 0;
  bool solved = false;
  std::vector<Edge> input;
  std::vector<int> mate, matched;

  void match(int id) {
    const auto [u,v] = input[id];
    mate[u] = v;
    mate[v] = u;
    matched[u] = matched[v] = id;
  }

public:
  explicit GeneralMatching(int n = 0) : n(n), mate(n,-1), matched(n,-1) { assert(0 <= n && n < std::numeric_limits<int>::max()); }

  int size() const { return n; }
  int edge_count() const { return static_cast<int>(input.size()); }
  void reserve_edges(int m) { input.reserve(m); }

  int add_edge(int u, int v) {
    assert(0 <= u && u < n && 0 <= v && v < n && input.size() < std::size_t(std::numeric_limits<int>::max()/2));
    input.push_back({u,v});
    if(u != v) solved = false;
    return edge_count()-1;
  }

  Edge get_edge(int id) const { return input[id]; }
  int partner(int v) const { return mate[v]; }
  int matched_edge(int v) const { return matched[v]; }

  std::vector<int> matching_edges() const {
    std::vector<int> result;
    result.reserve(cardinality);
    for(int v = 0; v < n; ++v) if(mate[v] > v) result.push_back(matched[v]);
    return result;
  }

  int solve() {
    if(solved) return cardinality;
    auto finish = [&] {
      solved = true;
      return cardinality;
    };
    cardinality = 0;
    std::fill(mate.begin(),mate.end(),-1);
    std::fill(matched.begin(),matched.end(),-1);
    auto greedy = [&] {
      for(int id = 0; id < edge_count(); ++id) {
        const auto [u,v] = input[id];
        if(u != v && mate[u] < 0 && mate[v] < 0) {
          match(id);
          if(++cardinality == n/2) break;
        }
      }
    };
    if(n < 2) return finish();
    const bool sparse = input.size() <= 4*std::size_t(n);
    if(!sparse) {
      greedy();
      if(cardinality == n/2) return finish();
      // Try length-three augmentations before allocating the dense adjacency.
      struct Neighbor { int to = -1, id = -1; };
      std::vector<std::array<Neighbor,2>> options(n);
      auto offer = [&](int u, int v, int id) {
        auto& row = options[u];
        if(row[0].to < 0) row[0] = {v,id};
        else if(row[0].to != v && row[1].to < 0) row[1] = {v,id};
      };
      for(int id = 0; id < edge_count(); ++id) {
        const auto [u,v] = input[id];
        if(mate[u] < 0 && mate[v] >= 0) offer(v,u,id);
        if(mate[v] < 0 && mate[u] >= 0) offer(u,v,id);
      }
      for(int u = 0; u < n; ++u) if(u < mate[u]) {
        const int v = mate[u];
        for(auto [a,id] : options[u]) if(a >= 0 && mate[a] < 0) {
          for(auto [b,jd] : options[v]) if(b >= 0 && b != a && mate[b] < 0) {
            match(id);
            match(jd);
            ++cardinality;
            break;
          }
          if(mate[u] != v) break;
        }
        if(cardinality == n/2) return finish();
      }
    }

    std::vector<int> start(n+1), degree(n);
    for(auto [u,v] : input) if(u != v) {
      ++degree[u];
      ++degree[v];
    }
    std::partial_sum(degree.begin(),degree.end(),start.begin()+1);
    std::vector<Arc> adj(start.back());
    auto cursor = start;
    for(int id = 0; id < edge_count(); ++id) {
      const auto [u,v] = input[id];
      if(u != v) {
        adj[cursor[u]++] = {v,id};
        adj[cursor[v]++] = {u,id};
      }
    }

    std::vector<int> queue;
    queue.reserve(n);
    if(sparse) {
      for(int v = 0; v < n; ++v) if(degree[v] == 1) queue.push_back(v);
      for(int i = 0; i < static_cast<int>(queue.size()); ++i) {
        const int u = queue[i];
        if(degree[u] != 1) continue;
        for(int j = start[u]; j < start[u+1]; ++j) if(degree[adj[j].to] > 0) {
          const auto [v,id] = adj[j];
          match(id);
          ++cardinality;
          degree[u] = degree[v] = 0;
          for(int k = start[v]; k < start[v+1]; ++k) {
            const int x = adj[k].to;
            if(degree[x] > 0 && --degree[x] == 1) queue.push_back(x);
          }
          break;
        }
      }
      greedy();
      if(cardinality == n/2) return finish();
    }

    // Only the endpoints fixed by leaf peeling are excluded from further searches.
    std::vector<int> color(n,-1), roots, by_component = {0}, remainder;
    for(int root = 0; root < n; ++root) if(color[root] < 0 && degree[root] > 0) {
      queue.clear();
      queue.push_back(root);
      color[root] = 0;
      int count[2] = {1,0}, covered = 0;
      bool bipartite = true;
      for(int i = 0; i < static_cast<int>(queue.size()); ++i) {
        const int u = queue[i];
        covered += mate[u] >= 0;
        for(int j = start[u]; j < start[u+1]; ++j) {
          const int v = adj[j].to;
          if(!degree[v]) continue;
          if(color[v] < 0) {
            color[v] = color[u]^1;
            ++count[color[v]];
            queue.push_back(v);
          } else if(color[v] == color[u]) bipartite = false;
        }
      }
      const int limit = bipartite ? std::min(count[0],count[1]) : static_cast<int>(queue.size())/2;
      if(covered/2 == limit) continue;
      const int smaller = count[0] > count[1];
      for(int v : queue) if(mate[v] < 0 && (!bipartite || color[v] == smaller)) roots.push_back(v);
      by_component.push_back(static_cast<int>(roots.size()));
      remainder.push_back(limit-covered/2);
    }
    if(roots.empty()) return finish();

    struct Link { int u, v, id; };
    std::vector<Link> link(n), flips;
    std::vector<int> group(n), outer(n,-1);
    std::vector<unsigned> seen(n);
    unsigned stamp = 0;
    int search = -1;

    // The contracted set is named by its first inner vertex, or -1 at the root.
    auto base = [&](int u) {
      int root = group[u];
      while(root >= 0 && outer[root] == search) root = group[root];
      while(group[u] != root) {
        const int p = group[u];
        group[u] = root;
        u = p;
      }
      return root;
    };
    auto assign = [&](int u, int v, int id) {
      mate[u] = v;
      matched[u] = id;
    };
    auto augment = [&](int root) {
      ++search;
      queue.clear();
      queue.push_back(root);
      outer[root] = search;
      group[root] = -1;
      for(int i = 0; i < static_cast<int>(queue.size()); ++i) {
        const int u = queue[i];
        for(int j = start[u]; j < start[u+1]; ++j) {
          const auto [v,id] = adj[j];
          if(!degree[v] || v == root) continue;
          if(mate[v] < 0) {
            assign(v,u,id);
            flips.clear();
            flips.push_back({u,v,id});
            while(!flips.empty()) {
              const auto [a,b,e] = flips.back();
              flips.pop_back();
              const int previous = mate[a];
              assign(a,b,e);
              if(previous < 0 || mate[previous] != a) continue;
              const auto [x,y,via] = link[a];
              if(y < 0) {
                assign(previous,x,via);
                flips.push_back({x,previous,via});
              } else {
                // A blossom link rematches both directions across the edge
                // that created the blossom, then follows the older links.
                flips.push_back({y,x,via});
                flips.push_back({x,y,via});
              }
            }
            return true;
          }
          if(outer[v] != search) {
            const int next = mate[v];
            if(outer[next] != search) {
              outer[next] = search;
              group[next] = v;
              link[next] = {u,-1,id};
              queue.push_back(next);
            }
            continue;
          }
          int a = base(u), b = base(v);
          if(a == b) continue;
          if(!++stamp) {
            std::fill(seen.begin(),seen.end(),0);
            ++stamp;
          }
          // Record the cross edge on the inner bases that will become outer.
          while(a != -1 || b != -1) {
            if(b != -1) std::swap(a,b);
            if(seen[a] == stamp) break;
            seen[a] = stamp;
            link[a] = {u,v,id};
            a = base(link[mate[a]].u);
          }
          for(int x : {base(u),base(v)}) {
            while(x != a) {
              const int up = base(link[mate[x]].u);
              outer[x] = search;
              group[x] = a;
              queue.push_back(x);
              x = up;
            }
          }
        }
      }
      return false;
    };
    for(int c = 0; c < static_cast<int>(remainder.size()); ++c) {
      for(int i = by_component[c]; i < by_component[c+1]; ++i) {
        const int root = roots[i];
        if(mate[root] < 0 && augment(root)) {
          ++cardinality;
          if(--remainder[c] == 0) break;
        }
      }
    }
    return finish();
  }
};

}  // namespace soryuusi
