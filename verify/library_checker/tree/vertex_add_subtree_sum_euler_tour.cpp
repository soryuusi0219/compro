#include "library/utility/fastio.hpp"

#include <vector>

#include "library/tree/euler_tour.hpp"

long long op(long long x, long long y) { return x+y; }
long long identity() { return 0; }

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> value(n);
  for(auto& x : value) soryuusi::cin >> x;
  soryuusi::EularTour<long long,op,identity> tree(value);
  for(int v = 1; v < n; ++v) {
    int parent;
    soryuusi::cin >> parent;
    tree.add_edge(parent,v);
  }
  tree.build();
  while(q--) {
    int type, u;
    soryuusi::cin >> type >> u;
    if(type == 0) {
      long long x;
      soryuusi::cin >> x;
      tree.set(u,tree.get(u)+x);
    } else {
      soryuusi::cout << tree.prod(u) << '\n';
    }
  }
}
