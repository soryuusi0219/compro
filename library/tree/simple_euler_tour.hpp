// https://github.com/soryuusi0219/compro/blob/main/library/tree/simple_euler_tour.hpp
#pragma once
#include <algorithm>
#include <utility>
#include <vector>

#include "../graph/static_graph.hpp"
#include "../data_structure/static_rmq.hpp"

namespace soryuusi {

using namespace std;

namespace internal {

inline constexpr pair<int,int> euler_depth_inf(){return {1<<30,-1};}

template<class Enter, class Exit>
vector<pair<int,int>> build_euler_tour(static_graph<>& graph, int root, vector<int>& in, vector<int>& out, vector<int>& tour, Enter enter, Exit exit){
  graph.build();
  vector<pair<int,int>> depths;
  depths.reserve(tour.size());
  struct Frame {int v, next;};
  vector<Frame> stack;
  auto visit = [&](int v){
    tour[depths.size()] = v;
    depths.emplace_back(static_cast<int>(stack.size())-1,v);
  };
  auto push = [&](int v){
    in[v] = depths.size();
    stack.push_back({v,graph.start[v]});
    enter(v);
    visit(v);
  };
  push(root);
  while(!stack.empty()){
    auto& [v,next] = stack.back();
    if(next == graph.start[v+1]){
      out[v] = depths.size();
      exit(v);
      stack.pop_back();
      if(!stack.empty()) visit(stack.back().v);
    }else{
      const int to = graph.es[next++].to;
      if(stack.size() == 1 || to != stack[stack.size()-2].v) push(to);
    }
  }
  return depths;
}

}  // namespace internal

struct SimpleEularTour{
  static constexpr int inf = internal::euler_depth_inf().first;
  using StaticRMQ = soryuusi::StaticRMQ<pair<int,int>,false,64,internal::euler_depth_inf>;
  int N = 0;
  static_graph<> G;
  vector<int> invertex, outvertex, tourid;
  StaticRMQ depth;
  SimpleEularTour(int n) : N(n), G(N,2*N-2) {
    invertex.resize(N);
    outvertex.resize(N);
    tourid.resize(2*N-1);
  }
  void add_edge(int u, int v){
    G.add_edge(u,v,true);
  }
  void build(int root = 0){
    depth.build(internal::build_euler_tour(G,root,invertex,outvertex,tourid, [](int){},[](int){}));
  }
  inline int get_depth(int u) const {
    return depth.data[invertex[u]].first;
  }
  inline int lca(int u, int v) const {
    if(u == v) return u;
    if(invertex[u] > invertex[v]) swap(u,v);
    return depth.prod(invertex[u],invertex[v]+1).second;
  }
  inline int dist(int u, int v) const {
    int w = lca(u,v);
    return get_depth(u)+get_depth(v)-get_depth(w)*2;
  }
};

}  // namespace soryuusi
