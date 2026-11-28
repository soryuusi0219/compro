// https://github.com/soryuusi0219/compro/blob/main/library/graph/bipartite_matching.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <limits>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>

namespace soryuusi {

class BipartiteMatching {
  struct Arc { int right, id; };

  std::vector<std::pair<int,int>> input;
  std::vector<int> left_match, right_owner, start;
  std::vector<Arc> arcs;
  int matched = 0;
  bool maximum = false;

  void build() {
    if(!start.empty()) return;
    arcs.resize(input.size());
    start.assign(left_match.size()+1,0);
    for(const auto& [u,v] : input) ++start[u+1];
    std::partial_sum(start.begin(),start.end(),start.begin());
    for(int id = 0; id < edge_count(); ++id) {
      const auto [u,v] = input[id];
      arcs[start[u]++] = {v,id};
    }
    std::move_backward(start.begin(),start.end()-1,start.end());
    start[0] = 0;
  }

  void match(int id) {
    const auto [u,v] = input[id];
    left_match[u] = id;
    right_owner[v] = u;
  }

  void build_reverse(std::vector<int>& offset, std::vector<int>& ids) const {
    if(!offset.empty()) return;
    offset.assign(right_owner.size()+1,0);
    ids.resize(input.size());
    for(const auto& e : input) ++offset[e.second];
    std::partial_sum(offset.begin(),offset.end(),offset.begin());
    for(int id = edge_count()-1; id >= 0; --id) ids[--offset[input[id].second]] = id;
  }

  bool contract_degree_two(int active_left, const std::vector<int>& reverse_start, const std::vector<int>& reverse) {
    std::vector<int> component(left_size(),-1), child_edge(left_size(),-1), parent_edge(left_size(),-1);
    std::vector<bool> tree_right(right_size(),false);
    std::vector<int> work;
    int groups = 0, forest_edges = 0;
    for(int root = 0; root < left_size(); ++root) {
      if(component[root] >= 0 || start[root] == start[root+1]) continue;
      component[root] = groups++;
      work.clear();
      work.push_back(root);
      for(int i = 0; i < int(work.size()); ++i) {
        const int u = work[i];
        for(int j = start[u]; j < start[u+1]; ++j) {
          const auto [v,id] = arcs[j];
          const int first = reverse_start[v];
          if(reverse_start[v+1]-first != 2) continue;
          const int other = reverse[first] == id ? reverse[first+1] : reverse[first];
          const int w = input[other].first;
          if(component[w] >= 0) continue;
          component[w] = component[u];
          tree_right[v] = true;
          child_edge[w] = other;
          parent_edge[w] = id;
          ++forest_edges;
          work.push_back(w);
        }
      }
    }
    if(forest_edges == 0 || 8LL*forest_edges < active_left) return false;

    std::vector<int> right_id(right_size(),-1);
    int columns = 0, reduced_edges = 0;
    for(int v = 0; v < right_size(); ++v) if(!tree_right[v] && reverse_start[v] != reverse_start[v+1]) {
      right_id[v] = columns++;
      reduced_edges += reverse_start[v+1]-reverse_start[v];
    }
    BipartiteMatching reduced(groups,columns);
    reduced.reserve_edges(reduced_edges);
    std::vector<int> original;
    original.reserve(reduced_edges);
    for(int id = 0; id < edge_count(); ++id) {
      const auto [u,v] = input[id];
      if(right_id[v] < 0) continue;
      reduced.add_edge(component[u],right_id[v]);
      original.push_back(id);
    }
    const int outside = reduced.solve_impl(false);

    // The tree on k left vertices contributes k-1 matches and one free root.
    // An external match moves that root along the tree path to its left endpoint.
    for(int e : child_edge) if(e >= 0) match(e);
    for(int id : reduced.left_match) if(id >= 0) {
      for(int e = original[id]; e >= 0; e = parent_edge[input[e].first]) match(e);
    }
    matched = forest_edges+outside;
    maximum = true;
    return true;
  }

  int prepare_matching(std::vector<int>& reverse_start, std::vector<int>& reverse, bool allow_contraction) {
    std::vector<int> left_degree(left_size()), right_degree(right_size(),0), leaves;
    for(const auto& edge : input) ++right_degree[edge.second];
    int active_left = 0, active_right = 0, degree_two_right = 0;
    for(int u = 0; u < left_size(); ++u) {
      left_degree[u] = start[u+1]-start[u];
      active_left += left_degree[u] > 0;
      if(left_degree[u] == 1) leaves.push_back(u);
    }
    for(int v = 0; v < right_size(); ++v) {
      active_right += right_degree[v] > 0;
      degree_two_right += right_degree[v] == 2;
      if(right_degree[v] == 1) leaves.push_back(~v);
    }
    const int bound = std::min(active_left,active_right);
    if(allow_contraction && leaves.empty() && 4LL*degree_two_right >= active_right) {
      build_reverse(reverse_start,reverse);
      if(contract_degree_two(active_left,reverse_start,reverse)) return matched;
    }
    const bool sparse = input.size() <= 4*std::size_t(bound);
    if(leaves.empty() && !sparse) return bound;

    build_reverse(reverse_start,reverse);
    std::size_t i = 0;
    int cursor = 0;
    while(true) {
      int leaf;
      if(i < leaves.size()) {
        leaf = leaves[i++];
      } else {
        if(!sparse) break;
        while(cursor < left_size() && (left_match[cursor] >= 0 || left_degree[cursor] == 0)) ++cursor;
        if(cursor == left_size()) break;
        leaf = cursor++;
      }
      int id = -1;
      if(leaf >= 0) {
        if(left_match[leaf] >= 0 || left_degree[leaf] == 0) continue;
        for(int j = start[leaf]; j < start[leaf+1]; ++j) if(right_owner[arcs[j].right] < 0) {
          id = arcs[j].id;
          break;
        }
      } else {
        const int v = ~leaf;
        if(right_owner[v] >= 0 || right_degree[v] != 1) continue;
        for(int j = reverse_start[v]; j < reverse_start[v+1]; ++j) if(left_match[input[reverse[j]].first] < 0) {
          id = reverse[j];
          break;
        }
      }
      assert(id >= 0);
      const auto [u,v] = input[id];
      match(id);
      ++matched;
      if(matched == bound) break;
      for(int j = start[u]; j < start[u+1]; ++j) {
        const int x = arcs[j].right;
        if(right_owner[x] < 0 && --right_degree[x] == 1) leaves.push_back(~x);
      }
      for(int j = reverse_start[v]; j < reverse_start[v+1]; ++j) {
        const int x = input[reverse[j]].first;
        if(left_match[x] < 0 && --left_degree[x] == 1) leaves.push_back(x);
      }
    }
    return bound;
  }

  void warm_start(int bound, std::vector<int>& reverse_start, std::vector<int>& reverse) {
    build_reverse(reverse_start,reverse);
    const int infinity = left_size();
    std::vector<int> depth(left_size(),infinity), queue;
    for(int u = 0; u < left_size(); ++u) if(left_match[u] < 0 && start[u] != start[u+1]) {
      depth[u] = 0;
      queue.push_back(u);
    }
    for(std::size_t i = 0; i < queue.size(); ++i) {
      const int u = queue[i];
      for(int j = start[u]; j < start[u+1]; ++j) {
        const int w = right_owner[arcs[j].right];
        if(w >= 0 && depth[w] == infinity) {
          depth[w] = depth[u]+1;
          queue.push_back(w);
        }
      }
    }

    std::deque<int> active;
    for(int v = 0; v < right_size(); ++v) if(right_owner[v] < 0 && reverse_start[v] != reverse_start[v+1]) active.push_back(v);
    std::size_t work = 0;
    const std::size_t budget = 12*(input.size()+left_match.size()+right_owner.size());
    // Reassign a free right endpoint toward the nearest free left endpoint.
    // The work cap keeps the warm start linear even with stale distance labels.
    while(!active.empty() && matched < bound) {
      const int v = active.front();
      active.pop_front();
      const int begin = reverse_start[v], end = reverse_start[v+1];
      work += end-begin+1;
      if(work > budget) break;

      int best = infinity, id = -1;
      for(int j = begin; j < end; ++j) {
        const int e = reverse[j], u = input[e].first;
        if(depth[u] < best) {
          best = depth[u];
          id = e;
          if(best == 0) break;
        }
      }
      if(id < 0) continue;
      const int u = input[id].first, previous = left_match[u];
      if(previous >= 0) {
        const int w = input[previous].second;
        active.push_back(w);
        right_owner[w] = -1;
      } else {
        ++matched;
      }
      match(id);
      ++depth[u];
    }
  }

public:
  struct VertexSet { std::vector<bool> left, right; };

  explicit BipartiteMatching(int left, int right) : left_match(left,-1), right_owner(right,-1) {
    assert(left >= 0 && right >= 0);
  }

  int left_size() const { return int(left_match.size()); }
  int right_size() const { return int(right_owner.size()); }
  int edge_count() const { return int(input.size()); }

  void reserve_edges(int m) {
    assert(m >= 0);
    input.reserve(m);
  }

  int add_edge(int u, int v) {
    assert(0 <= u && u < left_size() && 0 <= v && v < right_size());
    assert(input.size() < std::numeric_limits<int>::max());
    const int id = edge_count();
    input.emplace_back(u,v);
    maximum = false;
    start.clear();
    return id;
  }

  std::pair<int,int> get_edge(int id) const {
    assert(0 <= id && id < edge_count());
    return input[id];
  }

  const std::vector<std::pair<int,int>>& edges() const { return input; }
  const std::vector<int>& left_match_edges() const { return left_match; }
  int right_match_edge(int v) const {
    const int u = right_partner(v);
    return u < 0 ? -1 : left_match[u];
  }

  int left_partner(int u) const {
    assert(0 <= u && u < left_size());
    return left_match[u] < 0 ? -1 : input[left_match[u]].second;
  }

  int right_partner(int v) const {
    assert(0 <= v && v < right_size());
    return right_owner[v];
  }

  std::vector<int> matching_edges() const {
    std::vector<int> answer;
    answer.reserve(matched);
    for(int id : left_match) if(id >= 0) answer.push_back(id);
    return answer;
  }

  int solve() { return solve_impl(true); }

private:
  int solve_impl(bool allow_contraction) {
    if(maximum) return matched;
    build();
    int bound = std::min(left_size(),right_size());
    if(input.empty()) {
      maximum = true;
      return 0;
    }
    {
      std::vector<int> reverse_start, reverse;
      if(matched == 0 && bound >= 128) bound = prepare_matching(reverse_start,reverse,allow_contraction);
      if(maximum) return matched;
      for(int u = 0; u < left_size() && matched < bound; ++u) if(left_match[u] < 0) {
        for(int i = start[u]; i < start[u+1]; ++i) if(right_owner[arcs[i].right] < 0) {
          match(arcs[i].id);
          ++matched;
          break;
        }
      }
      if(bound-matched >= 8 && edge_count() >= 512) warm_start(bound,reverse_start,reverse);
    }
    if(matched == bound || matched == edge_count()) {
      maximum = true;
      return matched;
    }

    std::vector<int> depth(left_size()), current(left_size()), queue, path;
    queue.reserve(left_size());
    while(matched < bound) {
      std::fill(depth.begin(),depth.end(),-1);
      queue.clear();
      for(int u = 0; u < left_size(); ++u) if(left_match[u] < 0 && start[u] != start[u+1]) {
        depth[u] = 0;
        queue.push_back(u);
      }
      int limit = left_size();
      for(int i = 0; i < int(queue.size()); ++i) {
        const int u = queue[i];
        if(depth[u] >= limit) break;
        for(int j = start[u]; j < start[u+1]; ++j) {
          const int owner = right_owner[arcs[j].right];
          if(owner < 0) {
            limit = depth[u];
            break;
          }
          const int next = owner;
          if(depth[next] != -1) continue;
          depth[next] = depth[u]+1;
          queue.push_back(next);
        }
      }
      if(limit == left_size()) break;

      std::copy_n(start.begin(),left_size(),current.begin());
      for(int root : queue) {
        if(depth[root] > 0) break;
        if(depth[root] < 0 || left_match[root] >= 0) continue;
        path.clear();
        int u = root;
        while(true) {
          int& i = current[u];
          bool advanced = false, done = false;
          while(i < start[u+1]) {
            const auto [v,id] = arcs[i++];
            const int owner = right_owner[v];
            if(owner < 0) {
              if(depth[u] != limit) continue;
              path.push_back(id);
              for(int e : path) {
                match(e);
                depth[input[e].first] = -1;
              }
              ++matched;
              done = true;
              break;
            }
            const int next = owner;
            if(depth[u] == limit || depth[next] != depth[u]+1) continue;
            path.push_back(id);
            u = next;
            advanced = true;
            break;
          }
          if(done) break;
          if(advanced) continue;
          depth[u] = -1;
          if(path.empty()) break;
          u = input[path.back()].first;
          path.pop_back();
        }
        if(matched == bound) break;
      }
    }
    maximum = true;
    return matched;
  }

public:
  VertexSet minimum_vertex_cover() const {
    assert(maximum);
    VertexSet answer{std::vector<bool>(left_size(),true),std::vector<bool>(right_size(),false)};
    std::vector<int> work;
    for(int u = 0; u < left_size(); ++u) if(left_match[u] < 0) {
      answer.left[u] = false;
      work.push_back(u);
    }
    for(int i = 0; i < int(work.size()); ++i) {
      const int u = work[i];
      for(int j = start[u]; j < start[u+1]; ++j) {
        const int v = arcs[j].right;
        if(answer.right[v]) continue;
        answer.right[v] = true;
        const int w = right_partner(v);
        assert(w >= 0);
        if(answer.left[w]) {
          answer.left[w] = false;
          work.push_back(w);
        }
      }
    }
    return answer;
  }

  VertexSet maximum_independent_set() const {
    auto answer = minimum_vertex_cover();
    answer.left.flip();
    answer.right.flip();
    return answer;
  }

  std::optional<std::vector<int>> minimum_edge_cover() const {
    assert(maximum);
    auto answer = matching_edges();
    std::vector<int> extra(right_size(),-1);
    for(int u = 0; u < left_size(); ++u) {
      if(start[u] == start[u+1]) return std::nullopt;
      if(left_match[u] < 0) answer.push_back(arcs[start[u]].id);
    }
    for(int id = 0; id < edge_count(); ++id) {
      const int v = input[id].second;
      if(right_owner[v] < 0) extra[v] = id;
    }
    for(int v = 0; v < right_size(); ++v) if(right_owner[v] < 0) {
      if(extra[v] < 0) return std::nullopt;
      answer.push_back(extra[v]);
    }
    return answer;
  }
};

} // namespace soryuusi
