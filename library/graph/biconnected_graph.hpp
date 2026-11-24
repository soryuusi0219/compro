// https://github.com/soryuusi0219/compro/blob/main/library/graph/biconnected_graph.hpp
#pragma once
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

#include "static_graph.hpp"

namespace soryuusi {

using namespace std;

struct biconnected_graph{
  int n = 0;
  static_graph<> G;
  vector<int> h, nxt, ord, low, par;
  bool initialized = false;
  biconnected_graph() = default;
  biconnected_graph(int n_) : n(n_), G(n), ord(n,-1), low(n), par(n,-1) {}
  biconnected_graph(int n_, int m) : n(n_), G(n,2*m), ord(n,-1), low(n), par(n,-1) {}
  void add_edge(int u, int v){
    G.add_edge(u,v,true);
  }
  void low_link(){
    if(initialized) return;
    G.build();
    struct Frame{int vertex, next; bool skipped_parent;};
    vector<Frame> stack;
    order.reserve(n);
    auto enter = [&](int v, int p, int depth){
      par[v] = p;
      ord[v] = low[v] = depth;
      order.push_back(v);
      stack.push_back({v,G.start[v],false});
    };
    for(int root = 0; root < n; ++root) if(ord[root] == -1){
      enter(root,-1,0);
      while(!stack.empty()){
        auto& [v,next,skipped_parent] = stack.back();
        const int p = par[v];
        if(next != G.start[v+1]){
          const int to = G.es[next++].to;
          if(to == p && !skipped_parent){
            skipped_parent = true;
          }else if(ord[to] == -1){
            enter(to,v,ord[v]+1);
          }else low[v] = min(low[v],ord[to]);
        }else{
          if(p != -1) low[p] = min(low[p],low[v]);
          stack.pop_back();
        }
      }
    }
    h = par;
    nxt.assign(n,-1);
    for(int v = 0; v < n; ++v) if(par[v] != -1){
      nxt[v] = h[par[v]];
      h[par[v]] = v;
    }
    initialized = true;
  }
  bool is_bridge(int u, int v){
    low_link();
    if(par[u] == v) return parent_is_bridge(u);
    return par[v] == u && parent_is_bridge(v);
  }
  vector<int> two_edge_connected_component(int v){
    low_link();
    while(par[v] != -1 && !parent_is_bridge(v)) v = par[v];
    vector<int> component{v};
    for(int i = 0; i < (int)component.size(); ++i){
      const int u = component[i];
      for(int c = h[u]; c != par[u]; c = nxt[c]) if(!parent_is_bridge(c)) component.push_back(c);
    }
    return component;
  }
  vector<vector<int>> two_edge_connected_components(){
    low_link();
    auto [count,index] = edge_component_ids();
    return collect_components(count,index);
  }
  // (compressed forest, vertex to component, components)
  tuple<vector<vector<int>>,vector<int>,vector<vector<int>>> two_edge_connected_compressed(){
    low_link();
    auto [count,index] = edge_component_ids();
    vector<vector<int>> tree(count);
    for(int v : order) if(par[v] != -1 && index[v] != index[par[v]]){
      const int x = index[v], y = index[par[v]];
      tree[x].push_back(y);
      tree[y].push_back(x);
    }
    auto components = collect_components(count,index);
    return {move(tree),move(index),move(components)};
  }
  vector<vector<int>> two_vertex_connected_components(){
    low_link();
    auto [count,block] = vertex_block_ids();
    auto groups = collect_components(count,block);
    for(int v : order) if(par[v] != -1 && block[v] != block[par[v]]) groups[block[v]].push_back(par[v]);
    return groups;
  }
  pair<vector<vector<int>>,vector<int>> block_cut_tree(){
    low_link();
    auto [count,block] = vertex_block_ids();
    vector<vector<int>> tree(count);
    vector<int> index = block;
    auto connect = [&](int b, int a){
      tree[b].push_back(a);
      tree[a].push_back(b);
    };
    for(int v : order){
      const int p = par[v];
      if(p == -1 || block[v] == block[p]) continue;
      if(index[p] == block[p]){
        index[p] = tree.size();
        tree.emplace_back();
        connect(block[p],index[p]);
      }
      connect(block[v],index[p]);
    }
    return {move(tree),move(index)};
  }

private:
  vector<int> order;

  bool parent_is_bridge(int v) const{return low[v] > ord[par[v]];}

  pair<int,vector<int>> edge_component_ids() const {
    int count = 0;
    vector<int> ids(n);
    for(int v : order) ids[v] = par[v] == -1 || parent_is_bridge(v) ? count++ : ids[par[v]];
    return {count,move(ids)};
  }
  pair<int,vector<int>> vertex_block_ids() const {
    int count = 0;
    vector<int> ids(n);
    for(int v : order){
      const int p = par[v];
      // The root shares its block with one child, or forms a singleton.
      if(p == -1 || (low[v] >= ord[p] && (par[p] != -1 || v != h[p]))) ids[v] = count++;
      else ids[v] = ids[p];
    }
    return {count,move(ids)};
  }
  vector<vector<int>> collect_components(int count, const vector<int>& ids) const {
    vector<vector<int>> groups(count);
    for(int v : order) groups[ids[v]].push_back(v);
    return groups;
  }
};

}  // namespace soryuusi
