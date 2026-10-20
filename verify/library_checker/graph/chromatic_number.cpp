#include "library/utility/fastio.hpp"

#include "library/graph/chromatic_number.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  soryuusi::chromaticnumber graph(n);
  while(m--) {
    int u, v;
    soryuusi::cin >> u >> v;
    graph.add_edge(u,v);
  }
  soryuusi::cout << graph.color() << '\n';
}
