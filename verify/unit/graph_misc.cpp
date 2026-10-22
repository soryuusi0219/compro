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
}

}  // namespace

int main() {
  test_planar_graph();
  std::cout << "graph_misc: all tests passed\n";
}
