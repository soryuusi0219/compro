#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "library/tree/auxiliary_tree.hpp"
#include "library/tree/euler_tour.hpp"
#include "library/tree/lazy_euler_tour.hpp"
#include "library/tree/rerooting.hpp"
#include "library/tree/simple_euler_tour.hpp"

namespace {

long long sum(long long x, long long y) { return x+y; }
long long zero() { return 0; }
long long identity_mapping(int, long long value) { return value; }

struct SumNode {
  long long sum;
  int size;
  bool operator==(const SumNode&) const = default;
};

SumNode node_sum(SumNode x, SumNode y) {
  return {x.sum+y.sum,x.size+y.size};
}
SumNode empty_node() { return {0,0}; }
SumNode add_to_node(long long value, SumNode node) {
  node.sum += value*node.size;
  return node;
}
long long add_actions(long long x, long long y) { return x+y; }
long long no_action() { return 0; }

template<class Tree>
void add_edges(Tree& tree) {
  tree.add_edge(0,1);
  tree.add_edge(0,2);
  tree.add_edge(1,3);
  tree.add_edge(1,4);
  tree.add_edge(2,5);
  tree.add_edge(5,6);
}

}  // namespace

int main() {
  soryuusi::SimpleEularTour simple(7);
  add_edges(simple);
  simple.build();
  assert(simple.lca(3,4) == 1);
  assert(simple.lca(3,6) == 0);
  assert(simple.dist(4,6) == 5);

  std::vector<long long> values = {1,2,3,4,5,6,7};
  soryuusi::EularTour<long long,sum,zero> euler(values);
  add_edges(euler);
  euler.build();
  assert(euler.lca(4,6) == 0);
  assert(euler.dist(3,4) == 2);
  assert(euler.prod(1) == 2+4+5);
  assert(euler.prod(2) == 3+6+7);
  euler.set(4,50);
  assert(euler.prod(1) == 2+4+50);

  std::vector<SumNode> nodes;
  for(long long value : values) nodes.push_back({value,1});
  using LazyEuler = soryuusi::lazy_EularTour<
      SumNode,node_sum,empty_node,long long,add_to_node,add_actions,no_action>;
  LazyEuler lazy_euler(nodes);
  add_edges(lazy_euler);
  lazy_euler.build();
  lazy_euler.apply(1,10);
  assert(lazy_euler.prod(1) == SumNode({2+4+5+30,3}));
  assert(lazy_euler.get(3) == SumNode({14,1}));
  assert(lazy_euler.lca(3,6) == 0);

  soryuusi::Auxiliary_Tree auxiliary(7);
  add_edges(auxiliary);
  auxiliary.set_up();
  assert(auxiliary.lca(3,4) == 1);
  assert(auxiliary.lca(3,6) == 0);
  assert(auxiliary.up(6,2) == 2);
  const auto [compressed,index,root] = auxiliary.auxiliary_tree({3,4,6});
  assert(std::set<long long>(index.begin(),index.end()) ==
         std::set<long long>({0,1,3,4,6}));
  assert(index[root] == 0);
  std::size_t edge_count = 0;
  for(const auto& adjacent : compressed) edge_count += adjacent.size();
  assert(edge_count == 4);
  const auto [weighted_compressed,weighted_index,weighted_root] =
      auxiliary.auxiliary_tree_dist({3,4,6});
  assert(weighted_index == index && weighted_root == root);
  std::size_t weighted_edge_count = 0;
  for(int u = 0; u < static_cast<int>(weighted_compressed.size()); ++u) {
    for(const auto& [v,distance] : weighted_compressed[u]) {
      assert(distance == auxiliary.depth[weighted_index[v]]-
                         auxiliary.depth[weighted_index[u]]);
      ++weighted_edge_count;
    }
  }
  assert(weighted_edge_count == edge_count);

  soryuusi::Rerooting<long long,sum,zero,int,identity_mapping> rerooting(7,values);
  rerooting.add_edge(0,1,0);
  rerooting.add_edge(0,2,0);
  rerooting.add_edge(1,3,0);
  rerooting.add_edge(1,4,0);
  rerooting.add_edge(2,5,0);
  rerooting.add_edge(5,6,0);
  rerooting.build();
  const long long total = std::accumulate(values.begin(),values.end(),0LL);
  for(long long answer : rerooting.answers()) assert(answer == total);

  std::cout << "recursive_tree: all tests passed\n";
}
