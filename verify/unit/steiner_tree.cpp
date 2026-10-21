#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "library/tree/steiner_tree.hpp"

namespace {

using Graph = soryuusi::steiner_tree<long long,0,(1LL << 60)>;

void add_edges(Graph& graph) {
  graph.add_edge(0,1,1);
  graph.add_edge(1,2,1);
  graph.add_edge(1,3,2);
  graph.add_edge(3,4,1);
  graph.add_edge(0,4,10);
}

void check(const std::pair<long long,std::vector<std::tuple<int,int,long long>>>& result) {
  assert(result.first == 5);
  long long sum = 0;
  for(auto [u,v,w] : result.second) {
    assert(0 <= u && u < 5);
    assert(0 <= v && v < 5);
    sum += w;
  }
  assert(sum == result.first);
}

}  // namespace

int main() {
  const std::vector<int> terminals = {0,2,4};

  Graph graph(5);
  add_edges(graph);
  check(graph.solve(terminals));

  std::vector<std::vector<std::pair<int,long long>>> adjacency(5);
  auto add_edge = [&](int u, int v, long long w) {
    adjacency[u].push_back({v,w});
    adjacency[v].push_back({u,w});
  };
  add_edge(0,1,1);
  add_edge(1,2,1);
  add_edge(1,3,2);
  add_edge(3,4,1);
  add_edge(0,4,10);
  Graph from_adjacency(adjacency);
  check(from_adjacency.solve(terminals));

  std::cout << "steiner_tree: all tests passed\n";
}
