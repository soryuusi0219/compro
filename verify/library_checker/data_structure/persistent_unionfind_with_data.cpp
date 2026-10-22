#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/persistent_dsu_with_data.hpp"

int op(int x, int y) {return x+y;}

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Persistent_dsu<int,op> dsu;
  using Root = decltype(dsu)::np;
  std::vector<Root> roots(q+1);
  roots[0] = dsu.init(n,1);
  for(int i = 0; i < q; ++i) {
    int type, k, u, v;
    soryuusi::cin >> type >> k >> u >> v;
    Root root = roots[k+1];
    if(type == 0) roots[i+1] = dsu.merge(u,v,root).second;
    else{
      soryuusi::cout << dsu.same(u,v,root) << '\n';
      roots[i+1] = root;
    }
  }
}
