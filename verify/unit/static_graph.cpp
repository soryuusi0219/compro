#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "library/graph/static_graph.hpp"

int main(){
  soryuusi::static_graph graph(4,6);
  graph.add_edge(0,1,true);
  graph.add_edge(0,2,true);
  graph.add_edge(2,3,true);
  graph.build();
  std::vector<int> adjacent;
  for(int v : graph.vertices(0)) adjacent.push_back(v);
  assert(adjacent == std::vector<int>({1,2}));
  int edge_count = 0;
  for(auto e : graph[2]) edge_count += e.to == 0 || e.to == 3;
  assert(edge_count == 2);

  std::vector<std::vector<int>> adjacency0 = {{1,2},{2},{},{0}};
  soryuusi::static_graph from_adjacency(adjacency0);
  adjacent.clear();
  for(auto e : from_adjacency[0]) adjacent.push_back(e.to);
  assert(adjacent == adjacency0[0]);

  std::vector<std::vector<std::pair<short,long long>>> adjacency = {
      {{1,3},{2,5}},{{2,4}},{},{}};
  soryuusi::static_graph<long long> weighted(adjacency);
  long long sum = 0;
  for(auto e : weighted.edges(0)) sum += e.cost;
  assert(sum == 8);
  adjacent.clear();
  for(int v : weighted.vertices(0)) adjacent.push_back(v);
  assert(adjacent == std::vector<int>({1,2}));
  weighted.build();

  std::cout << "static_graph: all tests passed\n";
}
