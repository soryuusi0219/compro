#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "library/graph/planar_graph.hpp"

namespace {

void test_planar_graph() {
  std::vector<std::pair<long long,long long>> points{{0,0},{4,0},{4,3},{0,3}};
  soryuusi::planer_graph<long long> graph(points);
  graph.add_edge(0,1);
  graph.add_edge(1,2);
  graph.add_edge(2,3);
  graph.add_edge(3,0);
  assert(graph.build() == 2);
  assert(graph.detect_domain({2,1}) >= 0);
  assert(graph.detect_domain({5,1}) == -1);
  auto areas = graph.get_areas();
  assert(std::find(areas.begin(),areas.end(),24) != areas.end());
  assert(std::find(areas.begin(),areas.end(),-24) != areas.end());

  soryuusi::planer_graph<long long> divided({{0,0},{2,0},{4,0},{0,2},{2,2},{4,2}});
  for(auto [u,v] : std::vector<std::pair<int,int>>{{0,1},{1,2},{2,5},{5,4},{4,3},{3,0},{1,4}}) divided.add_edge(u,v);
  assert(divided.build() == 3);
  assert(divided.get_areas() == std::vector<long long>({8,8,-16}));
  assert(divided.detect_domain({1,1}) >= 0 && divided.detect_domain({3,1}) >= 0);
  assert(divided.detect_domain({1,1}) != divided.detect_domain({3,1}));
}

}  // namespace

int main() {
  test_planar_graph();
  std::cout << "graph_misc: all tests passed\n";
}
