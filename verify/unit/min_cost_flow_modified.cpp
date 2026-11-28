#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "library/graph/min_cost_flow_modified.hpp"

namespace {

std::pair<long long,long long> forward_cost(long long cost, long long flow) {
  return {cost,flow+1};
}

std::pair<long long,long long> backward_cost(long long cost, long long flow) {
  return {cost,flow-1};
}

long long max_cost() { return 1LL << 60; }
long long zero_cost() { return 0; }

using MinCostFlow = soryuusi::mcf_graph_modified<
    long long,long long,long long,forward_cost,backward_cost,max_cost,zero_cost>;

using Pair = std::pair<long long,long long>;

Pair convex_right(int kind, long long x) {
  if(kind == 1) return {0,2};
  if(x < 2) return {-3,2};
  if(x < 4) return {1,4};
  return {4,5};
}

Pair convex_left(int kind, long long x) {
  if(kind == 1) return {0,0};
  if(x <= 2) return {-3,0};
  if(x <= 4) return {1,2};
  return {4,4};
}

void convex_cases() {
  using Convex = soryuusi::mcf_graph_modified<long long,long long,int,convex_right,convex_left,max_cost,zero_cost>;
  Convex graph(2);
  graph.add_edge(0,1,5,0);
  graph.add_edge(0,1,2,1);
  assert(graph.slope(0,1,3) == (std::vector<Pair>{{0,0},{2,-6},{3,-6}}));
  assert(graph.slope(0,1) == (std::vector<Pair>{{0,0},{1,0},{3,2},{4,6}}));
  assert(graph.slope(1,0,4) == (std::vector<Pair>{{0,0},{1,-4},{3,-6},{4,-6}}));
  graph.reset();
  assert(graph.flow(0,1) == Pair(7,0));
  graph.reset();
  graph.add_edge(1,1,5,0);
  bool detected = false;
  try {
    graph.flow(0,1);
  } catch(const std::domain_error&) {
    detected = true;
  }
  assert(detected);
}

}  // namespace

int main() {
  MinCostFlow graph(4);
  const int edge01 = graph.add_edge(0,1,1,0);
  const int edge02 = graph.add_edge(0,2,1,1);
  const int edge12 = graph.add_edge(1,2,1,0);
  const int edge13 = graph.add_edge(1,3,1,2);
  const int edge23 = graph.add_edge(2,3,1,0);
  const int loop = graph.add_edge(1,1,3,10);

  assert(graph.flow(0,3,1) == Pair(1,0));
  assert(graph.flow(0,3,1) == Pair(1,3));
  assert(graph.get_edge(edge01).flow == 1);
  assert(graph.get_edge(edge02).flow == 1);
  assert(graph.get_edge(edge12).flow == 0);
  assert(graph.get_edge(edge13).flow == 1);
  assert(graph.get_edge(edge23).flow == 1);
  assert(graph.get_edge(loop).flow == 0);
  assert(graph.edges().size() == 6);

  graph.reset();
  assert(graph.flow(0,3,2) == Pair(2,3));
  MinCostFlow wide(2);
  wide.add_edge(0,1,std::numeric_limits<long long>::max(),2);
  assert(wide.flow(0,1,3) == Pair(3,6));
  convex_cases();

  std::cout << "min_cost_flow_modified: all tests passed\n";
}
