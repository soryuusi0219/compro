#include <cassert>
#include <iostream>
#include <utility>

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

}  // namespace

int main() {
  MinCostFlow graph(4);
  const int edge01 = graph.add_edge(0,1,1,0);
  const int edge02 = graph.add_edge(0,2,1,1);
  const int edge12 = graph.add_edge(1,2,1,0);
  const int edge13 = graph.add_edge(1,3,1,2);
  const int edge23 = graph.add_edge(2,3,1,0);
  const int loop = graph.add_edge(1,1,3,10);

  assert((graph.flow(0,3,2) == std::pair<long long,long long>(2,3)));
  assert(graph.get_edge(edge01).flow == 1);
  assert(graph.get_edge(edge02).flow == 1);
  assert(graph.get_edge(edge12).flow == 0);
  assert(graph.get_edge(edge13).flow == 1);
  assert(graph.get_edge(edge23).flow == 1);
  assert(graph.get_edge(loop).flow == 0);
  assert(graph.edges().size() == 6);

  std::cout << "min_cost_flow_modified: all tests passed\n";
}
