// https://github.com/soryuusi0219/compro/blob/main/library/graph/graph_general.hpp
#pragma once
#include <algorithm>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "static_graph.hpp"

namespace soryuusi {

using namespace std;

namespace internal {

template<class I>
int tree_edge_to(const I& edge){return static_cast<int>(edge);}
template<class I,class T>
int tree_edge_to(const pair<I,T>& edge){return static_cast<int>(edge.first);}
template<class T>
int tree_edge_to(const static_graph_edge<T>& edge){return edge.to;}

template<class I,class G>
vector<I> tree_path(const G& graph, int u, int v){
  using Iterator = decltype(graph[v].begin());
  struct Frame{int v; Iterator next, end;};
  vector<Frame> stack;
  auto enter = [&](int x){
    const auto& adjacent = graph[x];
    stack.push_back({x,adjacent.begin(),adjacent.end()});
  };
  enter(v);
  while(!stack.empty()){
    auto& frame = stack.back();
    if(frame.v == u){
      vector<I> result;
      result.reserve(stack.size());
      for(auto it = stack.rbegin(); it != stack.rend(); ++it) result.push_back(it->v);
      return result;
    }
    if(frame.next == frame.end){
      stack.pop_back();
      continue;
    }
    const int to = tree_edge_to(*frame.next++);
    if(stack.size() == 1 || to != stack[stack.size()-2].v) enter(to);
  }
  return {};
}

template<class T,class G,class Weight>
tuple<int,int,T> tree_diameter(const G& graph, Weight weight){
  struct Visit{int v, parent; T distance;};
  vector<Visit> stack;
  auto farthest = [&](int root){
    pair<T,int> best{T(0),root};
    stack.push_back({root,-1,T(0)});
    while(!stack.empty()){
      auto [v,p,d] = stack.back();
      stack.pop_back();
      if(best < pair<T,int>{d,v}) best = {d,v};
      for(const auto& edge : graph[v]){
        const int to = tree_edge_to(edge);
        if(to != p) stack.push_back({to,v,d+weight(edge)});
      }
    }
    return best;
  };
  int u = farthest(0).second;
  auto [distance,v] = farthest(u);
  if(u > v) swap(u,v);
  return {u,v,distance};
}

template<class G>
pair<int,int> tree_centroids(const G& graph){
  const int n = graph.size();
  using Iterator = decltype(graph[0].begin());
  struct Frame{int v, size; Iterator next, end;};
  vector<Frame> stack;
  auto enter = [&](int x){
    const auto& adjacent = graph[x];
    stack.push_back({x,1,adjacent.begin(),adjacent.end()});
  };
  if(!n) return {-1,-1};
  enter(0);
  while(true){
    auto& [v,size,next,end] = stack.back();
    const int p = stack.size() == 1 ? -1 : stack[stack.size()-2].v;
    if(next != end){
      const int to = tree_edge_to(*next++);
      if(to != p) enter(to);
      continue;
    }
    if(size >= n-size){
      if(size == n-size) return {min(v,p),max(v,p)};
      return {v,v};
    }
    const int subtree = size;
    stack.pop_back();
    stack.back().size += subtree;
  }
}

}  // namespace internal

template<class INT>
vector<INT> get_path(const vector<vector<INT>>& G, int u, int v){return internal::tree_path<INT>(G,u,v);}
template<class T>
vector<int> get_path(const static_graph<T>& G, int u, int v){return internal::tree_path<int>(G,u,v);}
template<class T,class INT>
vector<INT> get_path(const vector<vector<pair<INT,T>>>& G, int u, int v){return internal::tree_path<INT>(G,u,v);}

template<class INT>
tuple<int,int,int> diameter(const vector<vector<INT>>& G){return internal::tree_diameter<int>(G,[](const auto&){return 1;});}
inline tuple<int,int,int> diameter(const static_graph<>& G){return internal::tree_diameter<int>(G,[](const auto&){return 1;});}
template<class T,class INT>
tuple<int,int,T> diameter(const vector<vector<pair<INT,T>>>& G){return internal::tree_diameter<T>(G,[](const auto& edge) -> const T& {return edge.second;});}
template<class T> requires (!is_void_v<T>)
tuple<int,int,T> diameter(const static_graph<T>& G){return internal::tree_diameter<T>(G,[](const auto& edge) -> const T& {return edge.cost;});}

template<class INT>
pair<int,int> tree_get_centroids(const vector<vector<INT>>& G){return internal::tree_centroids(G);}
template<class T>
pair<int,int> tree_get_centroids(const static_graph<T>& G){return internal::tree_centroids(G);}
template<class T,class INT>
pair<int,int> tree_get_centroids(const vector<vector<pair<INT,T>>>& G){return internal::tree_centroids(G);}

}  // namespace soryuusi
