// https://github.com/soryuusi0219/compro/blob/main/library/graph/max_flow.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <vector>

namespace soryuusi {

template <class Cap> struct FlowEdge {
  int from, to;
  Cap cap, flow;
};

template <class Cap = long long> class MaxFlow {
  static_assert(std::numeric_limits<Cap>::is_integer && !std::is_same_v<Cap,bool>);

  struct Arc {
    int to;
    Cap residual;
  };
  struct Frame {
    int vertex;
    Cap remaining;
  };

  int n;
  std::vector<Arc> arcs;
  mutable std::vector<int> start, order;
  std::vector<int> level, current, queue;
  std::vector<Frame> stack;

  void build_adjacency() const {
    if(!start.empty()) return;
    order.resize(arcs.size());
    start.assign(std::size_t(n)+1,0);
    // Each arc has a paired reverse, so counting destinations gives the outgoing degrees.
    for(const auto& arc : arcs) ++start[arc.to+1];
    for(int v = 0; v < n; ++v) start[v+1] += start[v];
    for(int i = 0; i < int(arcs.size()); ++i) order[start[arcs[i^1].to]++] = i;
    std::move_backward(start.begin(),start.end()-1,start.end());
    start[0] = 0;
  }

  bool build_levels(int s, int t) {
    std::fill(level.begin(),level.end(),-1);
    level[t] = 0;
    queue.clear();
    queue.push_back(t);
    for(int i = 0; i < int(queue.size()); ++i) {
      const int v = queue[i];
      for(int j = start[v]; j < start[v+1]; ++j) {
        const int e = order[j];
        const int u = arcs[e].to;
        if(level[u] != -1 || arcs[e^1].residual == 0) continue;
        level[u] = level[v]+1;
        // All smaller distances are already known, which suffices for shortest s-t paths.
        if(u == s) return true;
        queue.push_back(u);
      }
    }
    return false;
  }

  Cap blocking_flow(int s, int t, Cap limit) {
    stack.clear();
    if(std::size_t(level[s])+1 > stack.capacity()) stack.reserve(level[s]+1);
    stack.push_back({s,limit});
    while(true) {
      auto& frame = stack.back();
      const int v = frame.vertex;
      if(v == t) {
        frame.remaining = 0;
      } else if(frame.remaining > 0) {
        int& j = current[v];
        while(j < start[v+1]) {
          const auto& arc = arcs[order[j]];
          if(arc.residual > 0 && level[arc.to] == level[v]-1) break;
          ++j;
        }
        if(j < start[v+1]) {
          const auto& arc = arcs[order[j]];
          stack.push_back({arc.to,std::min(frame.remaining,arc.residual)});
          continue;
        }
        level[v] = -1;
      }

      const Cap remaining = frame.remaining;
      stack.pop_back();
      if(stack.empty()) return limit-remaining;
      auto& parent = stack.back();
      int& j = current[parent.vertex];
      const int e = order[j];
      const Cap sent = std::min(parent.remaining,arcs[e].residual)-remaining;
      arcs[e].residual -= sent;
      arcs[e^1].residual += sent;
      parent.remaining -= sent;
      // The child exhausted its admissible arcs or this arc saturated.
      if(parent.remaining > 0) ++j;
    }
  }

public:
  using Edge = FlowEdge<Cap>;

  explicit MaxFlow(int n = 0) : n(n) { assert(n >= 0); }

  void reserve_edges(int m) {
    assert(0 <= m && m <= std::numeric_limits<int>::max()/2);
    arcs.reserve(2*m);
  }

  int add_edge(int from, int to, Cap cap) {
    assert(0 <= from && from < n && 0 <= to && to < n);
    assert(cap >= 0 && arcs.size()/2 < std::numeric_limits<int>::max()/2);
    const int i = int(arcs.size());
    arcs.resize(i+2);
    arcs[i] = {to,cap};
    arcs[i+1] = {from,0};
    start.clear();
    return i/2;
  }

  Edge get_edge(int id) const {
    assert(0 <= id && id < int(arcs.size()/2));
    const auto& arc = arcs[2*id];
    const auto& rev = arcs[2*id+1];
    return {rev.to,arc.to,Cap(arc.residual+rev.residual),rev.residual};
  }

  int size() const { return n; }

  template <class Emit> void for_each_residual_arc(Emit&& emit) const {
    for(int i = 0; i < int(arcs.size()); i += 2) {
      const auto& forward = arcs[i];
      const auto& reverse = arcs[i+1];
      if(forward.residual > 0) emit(reverse.to,forward.to,forward.residual);
      if(reverse.residual > 0) emit(forward.to,reverse.to,reverse.residual);
    }
  }

  std::vector<Edge> edges() const {
    std::vector<Edge> result;
    result.reserve(arcs.size()/2);
    for(int i = 0; i < int(arcs.size()/2); ++i) result.push_back(get_edge(i));
    return result;
  }

  void change_edge(int id, Cap cap, Cap flow) {
    assert(0 <= id && id < int(arcs.size()/2));
    assert(0 <= flow && flow <= cap);
    arcs[2*id].residual = cap-flow;
    arcs[2*id+1].residual = flow;
  }

  void reset() {
    for(int i = 0; i < int(arcs.size()); i += 2) {
      arcs[i].residual += arcs[i+1].residual;
      arcs[i+1].residual = 0;
    }
  }

  Cap flow(int s, int t, Cap limit = std::numeric_limits<Cap>::max()) {
    assert(0 <= s && s < n && 0 <= t && t < n && s != t && limit >= 0);
    if(limit == 0 || arcs.empty()) return 0;
    build_adjacency();
    level.resize(n);
    current.resize(n);
    queue.reserve(n);
    Cap result = 0;
    while(result < limit && build_levels(s,t)) {
      std::copy_n(start.begin(),n,current.begin());
      result += blocking_flow(s,t,limit-result);
    }
    return result;
  }

  Cap flow_push_relabel(int s, int t) {
    assert(0 <= s && s < n && 0 <= t && t < n && s != t);
    if(arcs.empty()) return 0;
    if(n > std::numeric_limits<int>::max()/2) return flow(s,t);
    build_adjacency();

    std::vector<Cap> excess(n,0);
    std::vector<int> height(n,0), at_height(n,0), first(2*n,-1), next(n);
    level.resize(n);
    current.resize(n);
    queue.reserve(n);
    int highest = -1;
    auto activate = [&](int v) {
      int h = height[v];
      next[v] = first[h];
      first[h] = v;
      highest = std::max(highest,h);
    };
    auto rebuild_buckets = [&]() {
      std::fill(first.begin(),first.end(),-1);
      highest = -1;
      for(int v = 0; v < n; ++v) if(v != s && v != t && excess[v] > 0) activate(v);
    };
    auto global_relabel = [&]() {
      std::fill(level.begin(),level.end(),-1);
      level[t] = 0;
      queue.clear();
      queue.push_back(t);
      for(int i = 0; i < int(queue.size()); ++i) {
        int v = queue[i];
        for(int j = start[v]; j < start[v+1]; ++j) {
          int e = order[j], u = arcs[e].to;
          if(u == s || level[u] >= 0 || arcs[e^1].residual == 0) continue;
          level[u] = level[v]+1;
          queue.push_back(u);
        }
      }
      height[s] = n;
      std::fill(at_height.begin(),at_height.end(),0);
      for(int v = 0; v < n; ++v) if(v != s) {
        // Keep labels monotone even when a vertex can only return excess to s.
        height[v] = level[v] >= 0 ? level[v] : std::max(height[v],n+1);
        if(height[v] < n) ++at_height[height[v]];
      }
      std::copy_n(start.begin(),n,current.begin());
    };
    global_relabel();

    Cap room = std::numeric_limits<Cap>::max();
    for(int j = start[s]; j < start[s+1]; ++j) {
      const auto& arc = arcs[order[j]];
      if(height[arc.to] >= n) continue;
      if(arc.residual > room) return flow(s,t);
      room -= arc.residual;
    }
    if(room == std::numeric_limits<Cap>::max()) return 0;
    for(int j = start[s]; j < start[s+1]; ++j) {
      const int e = order[j];
      auto& arc = arcs[e];
      if(height[arc.to] >= n || arc.residual == 0) continue;
      excess[arc.to] += arc.residual;
      arcs[e^1].residual += arc.residual;
      arc.residual = 0;
    }
    rebuild_buckets();

    const std::size_t relabel_threshold = 4*arcs.size()+n;
    std::size_t work = 0;
    while(highest >= 0) {
      if(first[highest] == -1) {
        --highest;
        continue;
      }
      const int v = first[highest];
      first[highest] = next[v];
      int& i = current[v];
      const int end = start[v+1];
      while(excess[v] > 0) {
        if(i < end) {
          ++work;
          int e = order[i];
          auto& arc = arcs[e];
          int u = arc.to;
          if(arc.residual == 0 || height[v] != height[u]+1) {
            ++i;
            continue;
          }
          const Cap sent = std::min(excess[v],arc.residual);
          arc.residual -= sent;
          arcs[e^1].residual += sent;
          excess[v] -= sent;
          if(u != s) {
            if(u != t && excess[u] == 0) activate(u);
            excess[u] += sent;
          }
          continue;
        }

        const int old = height[v];
        int h = 2*n-1;
        for(int j = start[v]; j < end; ++j) {
          const auto& arc = arcs[order[j]];
          if(arc.residual > 0) h = std::min(h,height[arc.to]+1);
        }
        assert(h > old);
        height[v] = h;
        i = start[v];
        work += end-i;
        if(h < n) ++at_height[h];
        if(old < n && --at_height[old] == 0) {
          // No vertex above the gap and below the source can reach the sink.
          for(int u = 0; u < n; ++u) if(u != s && old < height[u] && height[u] <= n) {
            if(height[u] < n) --at_height[height[u]];
            height[u] = n+1;
            current[u] = start[u];
          }
          rebuild_buckets();
          break;
        }
        if(work >= relabel_threshold) {
          global_relabel();
          rebuild_buckets();
          work = 0;
          break;
        }
      }
    }
    return excess[t];
  }

  std::vector<bool> min_cut(int s) const {
    assert(0 <= s && s < n);
    build_adjacency();
    std::vector<bool> reached(n,false);
    std::vector<int> work;
    work.push_back(s);
    reached[s] = true;
    for(int i = 0; i < int(work.size()); ++i) {
      const int v = work[i];
      for(int j = start[v]; j < start[v+1]; ++j) {
        const auto& arc = arcs[order[j]];
        if(arc.residual == 0 || reached[arc.to]) continue;
        reached[arc.to] = true;
        work.push_back(arc.to);
      }
    }
    return reached;
  }
};

} // namespace soryuusi
