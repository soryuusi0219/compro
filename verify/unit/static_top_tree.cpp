#include <cassert>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include "library/tree/static_top_tree.hpp"
#include "library/tree/static_top_tree_rerooting.hpp"

namespace {

long long add(long long a, long long b) { return a+b; }
long long zero() { return 0; }

}  // namespace

int main() {
  std::vector<long long> value{1,2,3,4,5,6,7};
  std::vector<std::pair<int,int>> edges{{0,1},{0,2},{1,3},{1,4},{2,5},{5,6}};

  soryuusi::StaticTopTree<long long,add,add> top_tree(value);
  for(auto [u,v] : edges) top_tree.add_edge(u,v);
  top_tree.build(0);
  assert(top_tree.get() == 28);
  top_tree.set(5,20);
  assert(top_tree.get() == 42);

  soryuusi::StaticTopTreeRerooting<long long,long long,add,add,add,add,zero,zero>
      rerooting(value,value);
  for(auto [u,v] : edges) rerooting.add_edge(u,v);
  rerooting.build(0);
  for(int root = 0; root < 7; ++root) assert(rerooting.get(root) == 28);
  rerooting.set(5,20,20);
  for(int root = 0; root < 7; ++root) assert(rerooting.get(root) == 42);

  std::cout << "static_top_tree: all tests passed\n";
}
