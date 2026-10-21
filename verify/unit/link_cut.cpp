#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "library/data_structure/lazy_link_cut.hpp"
#include "library/data_structure/link_cut.hpp"

namespace {

long long sum(long long x, long long y) { return x+y; }
long long zero() { return 0; }

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
void build_tree(Tree& tree) {
  tree.link(0,1);
  tree.link(1,2);
  tree.link(1,3);
  tree.link(3,4);
  tree.link(3,5);
}

}  // namespace

int main() {
  soryuusi::linkcuttree<long long,sum,zero> tree(6);
  for(int i = 0; i < 6; ++i) tree.set(i,i+1);
  build_tree(tree);
  assert(tree.prod(2,4) == 2+3+4+5);
  assert(tree.prod(0,5) == 1+2+4+6);
  assert((tree.get_path(2,4) == std::vector<int>{2,1,3,4}));
  tree.cut(1,3);
  tree.link(2,5);
  assert(tree.prod(0,4) == 1+2+3+6+4+5);
  assert((tree.get_path(0,4) == std::vector<int>{0,1,2,5,3,4}));

  using LazyTree = soryuusi::lazy_linkcuttree<
      SumNode,node_sum,empty_node,long long,add_to_node,add_actions,no_action>;
  LazyTree lazy_tree(6);
  for(int i = 0; i < 6; ++i) lazy_tree.set(i,{i+1,1});
  build_tree(lazy_tree);
  lazy_tree.apply(2,4,10);
  assert(lazy_tree.prod(2,4) == SumNode({2+3+4+5+40,4}));
  assert(lazy_tree.get(1) == SumNode({12,1}));
  assert((lazy_tree.get_path(5,0) == std::vector<int>{5,3,1,0}));
  lazy_tree.cut(1,3);
  lazy_tree.link(2,5);
  lazy_tree.apply(0,4,-2);
  assert(lazy_tree.prod(0,4).size == 6);
  assert((lazy_tree.get_path(0,4) == std::vector<int>{0,1,2,5,3,4}));

  std::cout << "link_cut: all tests passed\n";
}
