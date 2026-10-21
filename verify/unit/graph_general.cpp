#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

#include "library/graph/graph_general.hpp"

namespace {

template<class Graph>
void add_edge(Graph& graph, int u, int v) {
  graph[u].push_back(v);
  graph[v].push_back(u);
}

template<class T>
void add_edge(std::vector<std::vector<std::pair<int,T>>>& graph,
              int u, int v, T weight) {
  graph[u].emplace_back(v,weight);
  graph[v].emplace_back(u,weight);
}

}  // namespace

int main() {
  std::vector<std::vector<int>> graph(7);
  add_edge(graph,0,1);
  add_edge(graph,0,2);
  add_edge(graph,1,3);
  add_edge(graph,1,4);
  add_edge(graph,2,5);
  add_edge(graph,5,6);

  assert((soryuusi::get_path(graph,3,6) == std::vector<int>{3,1,0,2,5,6}));
  const auto [u,v,distance] = soryuusi::diameter(graph);
  assert(distance == 5);
  assert((std::set<int>{u,v} == std::set<int>({3,6}) ||
          std::set<int>{u,v} == std::set<int>({4,6})));
  assert((soryuusi::tree_get_centroids(graph) == std::pair<int,int>(0,0)));

  std::vector<std::vector<std::pair<int,long long>>> weighted(7);
  add_edge(weighted,0,1,2LL);
  add_edge(weighted,0,2,3LL);
  add_edge(weighted,1,3,5LL);
  add_edge(weighted,1,4,7LL);
  add_edge(weighted,2,5,11LL);
  add_edge(weighted,5,6,13LL);
  assert((soryuusi::get_path(weighted,4,6) ==
          std::vector<int>{4,1,0,2,5,6}));
  const auto [wu,wv,weighted_distance] = soryuusi::diameter(weighted);
  assert((std::set<int>{wu,wv} == std::set<int>({4,6})));
  assert(weighted_distance == 36);
  assert((soryuusi::tree_get_centroids(weighted) == std::pair<int,int>(0,0)));

  soryuusi::static_graph static_graph(7,12);
  static_graph.add_edge(0,1,true);
  static_graph.add_edge(0,2,true);
  static_graph.add_edge(1,3,true);
  static_graph.add_edge(1,4,true);
  static_graph.add_edge(2,5,true);
  static_graph.add_edge(5,6,true);
  static_graph.build();
  assert(soryuusi::get_path(static_graph,3,6) == soryuusi::get_path(graph,3,6));
  assert(soryuusi::diameter(static_graph) == soryuusi::diameter(graph));
  assert(soryuusi::tree_get_centroids(static_graph) == soryuusi::tree_get_centroids(graph));

  soryuusi::static_graph<long long> static_weighted(weighted);
  assert(soryuusi::get_path(static_weighted,4,6) == soryuusi::get_path(weighted,4,6));
  assert(soryuusi::diameter(static_weighted) == soryuusi::diameter(weighted));
  assert(soryuusi::tree_get_centroids(static_weighted) == soryuusi::tree_get_centroids(weighted));

  std::vector<std::vector<int>> even_path(6);
  for(int i = 0; i + 1 < 6; ++i) add_edge(even_path,i,i+1);
  assert((soryuusi::tree_get_centroids(even_path) == std::pair<int,int>(2,3)));

  std::cout << "graph_general: all tests passed\n";
}
