#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/bit.hpp"
#include "library/tree/heavy_light_decomposition.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> value(n);
  for(long long& x : value) soryuusi::cin >> x;
  soryuusi::HLdecomposition tree(n);
  for(int vertex = 1; vertex < n; ++vertex) {
    int parent;
    soryuusi::cin >> parent;
    tree.add_edge(parent,vertex);
  }
  tree.build(0);
  std::vector<long long> ordered(n);
  for(int vertex = 0; vertex < n; ++vertex) ordered[tree.inid(vertex)] = value[vertex];
  soryuusi::BIT<long long> bit(ordered);
  while(q--) {
    int type, u;
    soryuusi::cin >> type >> u;
    if(type == 0) {
      long long x;
      soryuusi::cin >> x;
      bit.add(tree.inid(u),x);
    } else {
      soryuusi::cout << bit.sum(tree.inid(u),tree.outid(u)) << '\n';
    }
  }
}
