#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "library/graph/dijkstra_01.hpp"
#include "library/graph/dijkstra_1.hpp"
#include "library/graph/directed_mst.hpp"
#include "library/graph/dynamic_connectivity_offline.hpp"
#include "library/tree/centroid_decomposition.hpp"

int main(){
  soryuusi::Dijkstra1 bfs(5,6);
  bfs.add_edge(0,1,true);
  bfs.add_edge(1,2,true);
  bfs.add_edge(0,3,true);
  bfs.add_edge(3,4,true);
  assert(bfs.distance(0) == std::vector<int>({0,1,2,1,2}));
  assert(bfs.path(2) == std::vector<int>({0,1,2}));

  soryuusi::Dijkstra01 bfs01(5,6);
  bfs01.add_edge(0,1,1,true);
  bfs01.add_edge(1,2,0,true);
  bfs01.add_edge(0,3,0,true);
  bfs01.add_edge(3,4,1,true);
  bfs01.add_edge(4,2,0,true);
  assert(bfs01.distance(0) == std::vector<int>({0,1,1,0,1}));

  soryuusi::CentroidDecomposition centroid(7);
  centroid.addEdge(0,1);
  centroid.addEdge(0,2);
  centroid.addEdge(1,3);
  centroid.addEdge(1,4);
  centroid.addEdge(2,5);
  centroid.addEdge(2,6);
  centroid.build();
  auto order = centroid.dfs_order;
  std::sort(order.begin(),order.end());
  std::vector<int> expected(7);
  std::iota(expected.begin(),expected.end(),0);
  assert(order == expected && centroid.dfs_order[0] == 0);

  const int path_size = 200'000;
  soryuusi::CentroidDecomposition path(path_size);
  for(int v = 1; v < path_size; ++v) path.addEdge(v-1,v);
  path.build();
  assert((int)path.dfs_order.size() == path_size && path.dfs_order[0] == path_size/2);

  soryuusi::dynamic_connectivity_offline connectivity(4,6);
  connectivity.add_edge(0,0,1);
  connectivity.add_edge(1,1,2);
  connectivity.erase_edge(2,0,1);
  connectivity.add_edge(3,2,3);
  connectivity.add_edge(4,0,3);
  connectivity.erase_edge(5,1,2);
  connectivity.build();
  std::vector<bool> answer;
  connectivity.run([&](int i){
    if(i == 0) answer.push_back(connectivity.uf.same(0,1));
    if(i == 1) answer.push_back(connectivity.uf.same(0,2));
    if(i == 2) answer.push_back(connectivity.uf.same(0,2));
    if(i == 3) answer.push_back(connectivity.uf.same(1,3));
    if(i == 4) answer.push_back(connectivity.uf.same(0,1));
    if(i == 5) answer.push_back(connectivity.uf.same(0,1));
  });
  assert(answer == std::vector<bool>({true,true,false,true,true,false}));

  soryuusi::DirectedMST<long long> directed(4);
  directed.add_edge(0,1,8);
  int root_edge = directed.add_edge(0,1,4);
  directed.add_edge(2,1,-2);
  int edge12 = directed.add_edge(1,2,-3);
  directed.add_edge(3,2,0);
  int edge23 = directed.add_edge(2,3,1);
  directed.add_edge(2,2,-20);
  auto [cost,incoming] = directed.solve(0);
  assert(cost == 2 && incoming == std::vector<int>({-1,root_edge,edge12,edge23}));
  assert(directed.solve(0).second == incoming);
  assert(directed.solve(1).second.empty());

  std::cout << "csr_graph: all tests passed\n";
}
