#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/lazy_link_cut.hpp"

struct Node {
  long long sum;
  int size;
};

Node op(Node x, Node y) { return {x.sum+y.sum,x.size+y.size}; }
Node identity() { return {0,0}; }
Node mapping(long long x, Node node) {
  return {node.sum+x*node.size,node.size};
}
long long composition(long long x, long long y) { return x+y; }
long long action_identity() { return 0; }

int main() {
  using Tree = soryuusi::lazy_linkcuttree<
      Node,op,identity,long long,mapping,composition,action_identity>;
  int n, q;
  soryuusi::cin >> n >> q;
  Tree tree(n);
  for(int i = 0; i < n; ++i) {
    long long value;
    soryuusi::cin >> value;
    tree.set(i,{value,1});
  }
  for(int i = 1; i < n; ++i) {
    int u, v;
    soryuusi::cin >> u >> v;
    tree.link(u,v);
  }
  while(q--) {
    int type;
    soryuusi::cin >> type;
    if(type == 0) {
      int u, v, w, x;
      soryuusi::cin >> u >> v >> w >> x;
      tree.cut(u,v);
      tree.link(w,x);
    } else if(type == 1) {
      int p;
      long long x;
      soryuusi::cin >> p >> x;
      const auto value = tree.get(p);
      tree.set(p,{value.sum+x,1});
    } else {
      int u, v;
      soryuusi::cin >> u >> v;
      soryuusi::cout << tree.prod(u,v).sum << '\n';
    }
  }
}
