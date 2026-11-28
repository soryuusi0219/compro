// https://github.com/soryuusi0219/compro/blob/main/library/graph/internal/weighted_blossom.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

namespace soryuusi::internal {

// The dense Edmonds primal-dual representation follows qandori/cp-library
// components/weighted_blossom.hpp (CC0-1.0, 97b50074b8c1613bc358316249dc38e54d08a331).
template <class Weight> class WeightedBlossom {
  struct Edge { int u = 0, v = 0; Weight weight = 0; };
  struct Neighbor { int to; Weight weight; };
  enum : signed char { Free = -1, Outer = 0, Inner = 1 };

  int n, stride, nodes;
  unsigned stamp = 0;
  std::vector<Edge> graph;
  std::vector<Weight> dual;
  Weight free_dual;
  std::vector<int> mate, base, parent, slack, from, queue, start;
  std::vector<Neighbor> adjacent;
  std::vector<signed char> state;
  std::vector<int> forest_vertices, forest_blossoms, slack_nodes;
  std::vector<unsigned char> in_forest, in_slack;
  std::vector<unsigned> seen;
  std::vector<std::vector<int>> children;

  Edge& edge(int u, int v) { return graph[std::size_t(u)*stride+v]; }
  const Edge& edge(int u, int v) const { return graph[std::size_t(u)*stride+v]; }
  int& child(int b, int v) { return from[std::size_t(b-n)*(n+1)+v]; }

  // On an edge between distinct blossoms there is no common blossom dual.
  Weight gap(const Edge& e) const { return dual[e.u]+dual[e.v]-2*e.weight; }

  void record_slack(int b) {
    if(!in_slack[b]) {
      in_slack[b] = 1;
      slack_nodes.push_back(b);
    }
  }

  void relax(int u, int b, Weight distance) {
    if(slack[b] == u) return;
    if(!slack[b]) {
      slack[b] = u;
      record_slack(b);
    } else {
      const Edge* row = &edge(b,0);
      if(b > n) distance = gap(row[u]);
      if(distance < gap(row[slack[b]])) slack[b] = u;
    }
  }

  void reset_slack(int b) {
    const Edge* row = &edge(b,0);
    int best = 0;
    Weight distance = std::numeric_limits<Weight>::max();
    for(int u : forest_vertices) if(base[u] != b && state[base[u]] == Outer && row[u].weight > 0) {
      const Weight d = gap(row[u]);
      if(d < distance) {
        distance = d;
        best = u;
      }
    }
    slack[b] = best;
    if(best) record_slack(b);
  }

  template <class F> void for_leaves(int b, const F& f) const {
    if(b <= n) f(b);
    else for(int c : children[b]) for_leaves(c,f);
  }

  void enter_forest(int b, bool outer) {
    if(b > n && !in_forest[b]) {
      in_forest[b] = 1;
      forest_blossoms.push_back(b);
    }
    for_leaves(b,[&](int u) {
      if(!in_forest[u]) {
        in_forest[u] = 1;
        forest_vertices.push_back(u);
      }
      if(outer) queue.push_back(u);
    });
  }

  void set_base(int b, int root) {
    base[b] = root;
    if(b > n) for(int c : children[b]) set_base(c,root);
  }

  int orient(int b, int c) {
    auto& list = children[b];
    int p = static_cast<int>(std::find(list.begin(),list.end(),c)-list.begin());
    assert(p < static_cast<int>(list.size()));
    if(p&1) {
      std::reverse(list.begin()+1,list.end());
      p = static_cast<int>(list.size())-p;
    }
    return p;
  }

  void set_match(int b, int v) {
    const auto e = edge(b,v);
    mate[b] = e.v;
    if(b <= n) return;
    const int c = child(b,e.u), p = orient(b,c);
    auto& list = children[b];
    for(int i = 0; i < p; ++i) set_match(list[i],list[i^1]);
    set_match(c,v);
    std::rotate(list.begin(),list.begin()+p,list.end());
  }

  void augment(int u, int v) {
    while(u) {
      const int next = base[mate[u]];
      set_match(u,v);
      if(!next) break;
      u = base[parent[next]];
      set_match(next,u);
      v = next;
    }
  }

  int lca(int u, int v) {
    if(!++stamp) {
      std::fill(seen.begin(),seen.end(),0);
      ++stamp;
    }
    while(u || v) {
      if(u) {
        if(seen[u] == stamp) return u;
        seen[u] = stamp;
        u = base[mate[u]];
        if(u) u = base[parent[u]];
      }
      std::swap(u,v);
    }
    return 0;
  }

  void contract(int u, int v, int root) {
    int b = n+1;
    while(b <= nodes && base[b]) ++b;
    nodes = std::max(nodes,b);
    dual[b] = 0;
    state[b] = Outer;
    mate[b] = mate[root];
    auto& list = children[b];
    list.clear();
    list.push_back(root);
    auto add_path = [&](int x) {
      while(x != root) {
        const int y = base[mate[x]];
        list.push_back(x);
        list.push_back(y);
        enter_forest(y,true);
        x = base[parent[y]];
      }
    };
    add_path(u);
    std::reverse(list.begin()+1,list.end());
    add_path(v);
    set_base(b,b);
    if(!in_forest[b]) {
      in_forest[b] = 1;
      forest_blossoms.push_back(b);
    }

    Edge* row = &edge(b,0);
    std::fill_n(row,nodes+1,Edge{});
    for(int c : list) {
      const Edge* other = &edge(c,0);
      for(int x = 1; x <= nodes; ++x) if(other[x].weight > 0 && (!row[x].weight || gap(other[x]) < gap(row[x]))) row[x] = other[x];
      for_leaves(c,[&](int x) { child(b,x) = c; });
    }
    row[b] = {};
    for(int x = 1; x <= nodes; ++x) if(x != b) edge(x,b) = {row[x].v,row[x].u,row[x].weight};
    reset_slack(b);
  }

  void expand(int b) {
    auto& list = children[b];
    for(int c : list) set_base(c,c);
    const int entry = child(b,edge(b,parent[b]).u), p = orient(b,entry);
    for(int i = 0; i < p; i += 2) {
      const int u = list[i], v = list[i+1];
      parent[u] = edge(v,u).u;
      state[u] = Inner;
      state[v] = Outer;
      slack[u] = 0;
      reset_slack(v);
      enter_forest(u,false);
      enter_forest(v,true);
    }
    state[entry] = Inner;
    parent[entry] = parent[b];
    slack[entry] = 0;
    enter_forest(entry,false);
    for(int i = p+1; i < static_cast<int>(list.size()); ++i) {
      state[list[i]] = Free;
      reset_slack(list[i]);
    }
    base[b] = 0;
  }

  bool found(const Edge& e) {
    const int u = base[e.u], v = base[e.v];
    if(state[v] == Free) {
      parent[v] = e.u;
      state[v] = Inner;
      const int next = base[mate[v]];
      assert(next);
      slack[v] = slack[next] = 0;
      state[next] = Outer;
      enter_forest(v,false);
      enter_forest(next,true);
    } else if(state[v] == Outer) {
      const int root = lca(u,v);
      if(!root) {
        augment(u,v);
        augment(v,u);
        return true;
      }
      contract(u,v,root);
    }
    return false;
  }

  bool step() {
    std::fill(state.begin(),state.end(),Free);
    std::fill(slack.begin(),slack.end(),0);
    queue.clear();
    forest_vertices.clear();
    forest_blossoms.clear();
    slack_nodes.clear();
    std::fill(in_forest.begin(),in_forest.end(),0);
    std::fill(in_slack.begin(),in_slack.end(),0);
    for(int b = 1; b <= nodes; ++b) if(base[b] == b && !mate[b]) {
      parent[b] = 0;
      state[b] = Outer;
      enter_forest(b,true);
    }
    if(queue.empty()) return false;
    for(int head = 0;; head = 0) {
      while(head < static_cast<int>(queue.size())) {
        const int u = queue[head++];
        int u_base = base[u];
        if(state[u_base] != Outer) continue;
        const Weight u_dual = dual[u];
        const Edge* row = &edge(u,0);
        for(int i = start[u]; i < start[u+1]; ++i) {
          const auto [v,w] = adjacent[i];
          const int b = base[v];
          if(b == u_base) continue;
          const Weight d = u_dual+dual[v]-2*w;
          if(d != 0) relax(u,b,d);
          else {
            if(found(row[v])) return true;
            u_base = base[u];
          }
        }
      }

      Weight delta = free_dual;
      for(int b : forest_blossoms) if(base[b] == b && state[b] == Inner) delta = std::min(delta,Weight(dual[b]/2));
      for(int b : slack_nodes) if(base[b] == b && slack[b]) {
        const Weight d = gap(edge(slack[b],b));
        if(state[b] == Free) delta = std::min(delta,d);
        else if(state[b] == Outer) delta = std::min(delta,Weight(d/2));
      }
      for(int u : forest_vertices) {
        if(state[base[u]] == Outer) dual[u] -= delta;
        else if(state[base[u]] == Inner) dual[u] += delta;
      }
      for(int b : forest_blossoms) if(base[b] == b) {
        if(state[b] == Outer) dual[b] += 2*delta;
        else if(state[b] == Inner) dual[b] -= 2*delta;
      }
      // Unmatched vertices are outer in every step. Their common dual is
      // therefore the minimum of all vertex duals.
      free_dual -= delta;
      if(free_dual == 0) return false;

      queue.clear();
      for(int i = 0; i < static_cast<int>(slack_nodes.size()); ++i) {
        const int b = slack_nodes[i];
        if(base[b] == b && slack[b] && base[slack[b]] != b && gap(edge(slack[b],b)) == 0) {
          if(found(edge(slack[b],b))) return true;
        }
      }
      for(int i = 0; i < static_cast<int>(forest_blossoms.size()); ++i) {
        const int b = forest_blossoms[i];
        if(base[b] == b && state[b] == Inner && dual[b] == 0) expand(b);
      }
    }
  }

  bool check_dual() const {
    using Wide = __int128_t;
    std::vector<int> container(nodes+1);
    std::vector<Wide> prefix(nodes+1), shared(n+1);
    Wide primal = 0, objective = 0;
    auto visit = [&](auto&& self, int b, int p) -> int {
      assert(dual[b] >= 0);
      container[b] = p;
      if(b <= n) {
        objective += dual[b];
        return 1;
      }
      prefix[b] = prefix[p]+dual[b];
      int size = 0;
      for(int c : children[b]) size += self(self,c,b);
      objective += Wide((size-1)/2)*dual[b];
      return size;
    };
    for(int b = 1; b <= nodes; ++b) if(base[b] == b) visit(visit,b,0);
    for(int u = 1; u <= n; ++u) {
      const int v = mate[u];
      if(v && (v == u || mate[v] != u || edge(u,v).weight == 0)) return false;
      if(v > u) primal += Wide(2)*edge(u,v).weight;

      std::fill(shared.begin(),shared.end(),0);
      for(int x = u, p = container[x]; p; x = p, p = container[x]) {
        for(int c : children[p]) if(c != x) for_leaves(c,[&](int t) { shared[t] = prefix[p]; });
      }
      for(int i = start[u]; i < start[u+1]; ++i) {
        const int v = adjacent[i].to;
        if(v > u && Wide(gap(edge(u,v)))+shared[v] < 0) return false;
      }
    }
    return primal == objective;
  }

public:
  // A forest of n leaves and odd cycles of at least three children has
  // at most (n-1)/2 active blossoms. Expanded IDs are reused.
  template <class Edges> WeightedBlossom(int n, const Edges& edges, Weight maximum)
    : n(n), stride(n+(n-1)/2+1), nodes(n), graph(std::size_t(stride)*stride), dual(stride), free_dual(maximum), mate(stride), base(stride), parent(stride), slack(stride), from(std::size_t(stride-n)*(n+1)), start(n+2), state(stride), in_forest(stride), in_slack(stride), seen(stride), children(stride) {
    std::size_t distinct = 0;
    for(auto [a,b,w] : edges) {
      const int u = a+1, v = b+1;
      if(w > edge(u,v).weight) {
        distinct += edge(u,v).weight == 0;
        edge(u,v) = {u,v,Weight(w)};
        edge(v,u) = {v,u,Weight(w)};
      }
    }
    adjacent.reserve(2*distinct);
    for(int u = 1; u <= n; ++u) {
      base[u] = u;
      dual[u] = maximum;
      start[u] = static_cast<int>(adjacent.size());
      const Edge* row = &edge(u,0);
      for(int v = 1; v <= n; ++v) if(row[v].weight > 0) adjacent.push_back({v,row[v].weight});
    }
    start[n+1] = static_cast<int>(adjacent.size());
    queue.reserve(n);
  }

  std::vector<int> solve() {
    while(step()) {}
    assert(check_dual());
    std::vector<int> result(n);
    for(int u = 1; u <= n; ++u) result[u-1] = mate[u]-1;
    return result;
  }
};

}  // namespace soryuusi::internal
