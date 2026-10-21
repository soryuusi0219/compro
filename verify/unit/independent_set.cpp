#include <cassert>
#include <iostream>
#include <random>
#include <vector>

#include "library/graph/maximal_independent_set.hpp"
#include "library/graph/maximal_weighted_independent_set.hpp"

namespace {

bool independent(const std::vector<std::vector<long long>>& graph,
                 const std::vector<long long>& vertices) {
  std::vector<bool> selected(graph.size());
  for(long long vertex : vertices) {
    if(vertex < 0 || vertex >= static_cast<long long>(graph.size()) ||
       selected[vertex]) return false;
    selected[vertex] = true;
  }
  for(long long vertex : vertices) {
    for(long long next : graph[vertex]) if(selected[next]) return false;
  }
  return true;
}

std::pair<int,long long> brute_force(
    const std::vector<std::vector<long long>>& graph,
    const std::vector<int>& weight) {
  const int n = static_cast<int>(graph.size());
  int best_size = 0;
  long long best_weight = 0;
  for(int mask = 0; mask < (1 << n); ++mask) {
    bool valid = true;
    for(int u = 0; u < n; ++u) if(mask >> u & 1) {
      for(long long v : graph[u]) if(u < v && (mask >> v & 1)) valid = false;
    }
    if(!valid) continue;
    int size = __builtin_popcount(static_cast<unsigned>(mask));
    long long sum = 0;
    for(int i = 0; i < n; ++i) if(mask >> i & 1) sum += weight[i];
    best_size = std::max(best_size,size);
    best_weight = std::max(best_weight,sum);
  }
  return {best_size,best_weight};
}

}  // namespace

int main() {
  std::mt19937 rng(20261021);
  for(int n = 1; n <= 16; ++n) {
    for(int trial = 0; trial < 30; ++trial) {
      std::vector<std::vector<long long>> graph(n);
      for(int u = 0; u < n; ++u) for(int v = u+1; v < n; ++v) {
        if(rng()%4 == 0) graph[u].push_back(v), graph[v].push_back(u);
      }
      std::vector<int> weight(n);
      for(int& value : weight) value = static_cast<int>(rng()%20)+1;
      const auto [best_size,best_weight] = brute_force(graph,weight);

      auto graph_copy = graph;
      const auto vertices = soryuusi::maximal_independent_set(graph_copy);
      assert(independent(graph,vertices));
      assert(static_cast<int>(vertices.size()) == best_size);

      graph_copy = graph;
      const auto [result_weight,weighted_vertices] =
          soryuusi::maximal_weighted_independent_set(graph_copy,weight);
      assert(independent(graph,weighted_vertices));
      long long sum = 0;
      for(long long vertex : weighted_vertices) sum += weight[vertex];
      assert(sum == result_weight && sum == best_weight);
    }
  }

  std::vector<std::vector<long long>> clique(65);
  for(int u = 0; u < 65; ++u) for(int v = u+1; v < 65; ++v) {
    clique[u].push_back(v);
    clique[v].push_back(u);
  }
  auto clique_copy = clique;
  assert(soryuusi::maximal_independent_set(clique_copy).size() == 1);
  std::vector<int> weight(65);
  for(int i = 0; i < 65; ++i) weight[i] = i+1;
  clique_copy = clique;
  const auto [result_weight,vertices] =
      soryuusi::maximal_weighted_independent_set(clique_copy,weight);
  assert(result_weight == 65 && vertices == std::vector<long long>{64});

  std::cout << "independent_set: all tests passed\n";
}
