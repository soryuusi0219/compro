#include "library/utility/fastio.hpp"

#include <vector>

#include "library/math/modint.hpp"
#include "library/tree/rerooting.hpp"

using mint = soryuusi::modint998244353;

struct Node {
  mint sum;
  int size;
};

struct Affine {
  mint b, c;
};

Node op(Node x, Node y) { return {x.sum+y.sum,x.size+y.size}; }
Node identity() { return {0,0}; }
Node mapping(Affine f, Node x) { return {f.b*x.sum+f.c*x.size,x.size}; }

int main() {
  int n;
  soryuusi::cin >> n;
  std::vector<Node> value(n);
  for(auto& x : value) {
    int a;
    soryuusi::cin >> a;
    x = {a,1};
  }
  soryuusi::Rerooting<Node,op,identity,Affine,mapping> tree(n,value);
  for(int i = 1; i < n; ++i) {
    int u, v, b, c;
    soryuusi::cin >> u >> v >> b >> c;
    tree.add_edge(u,v,{b,c});
  }
  tree.build();
  const auto answer = tree.answers();
  for(int i = 0; i < n; ++i) {
    soryuusi::cout << answer[i].sum.val() << " \n"[i + 1 == n];
  }
}
