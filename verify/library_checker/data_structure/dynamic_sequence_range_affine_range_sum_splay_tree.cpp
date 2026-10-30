#include "library/utility/fastio.hpp"

#include <utility>
#include <vector>

#include "library/data_structure/splay_tree_lazy.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::modint998244353;

struct Node {
  mint sum;
  int size;
};

using Affine = std::pair<mint,mint>;

Node op(Node x, Node y) { return {x.sum+y.sum,x.size+y.size}; }
Node identity() { return {0,0}; }
Node mapping(Affine f, Node x) {
  return {f.first*x.sum+f.second*x.size,x.size};
}
Affine composition(Affine f, Affine g) {
  return {f.first*g.first,f.first*g.second+f.second};
}
Affine action_identity() { return {1,0}; }

int main() {
  using Sequence = soryuusi::splay_tree<
      Node,op,identity,Affine,mapping,composition,action_identity>;
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<Node> value(n);
  for(auto& x : value) {
    int a;
    soryuusi::cin >> a;
    x = {a,1};
  }
  Sequence sequence(value);
  while(q--) {
    int type;
    soryuusi::cin >> type;
    if(type == 0) {
      int i, x;
      soryuusi::cin >> i >> x;
      sequence.insert_at(i,{x,1});
    } else if(type == 1) {
      int i;
      soryuusi::cin >> i;
      sequence.erase_at(i);
    } else if(type == 2) {
      int l, r;
      soryuusi::cin >> l >> r;
      sequence.flip(l,r);
    } else if(type == 3) {
      int l, r, b, c;
      soryuusi::cin >> l >> r >> b >> c;
      sequence.apply(l,r,{b,c});
    } else {
      int l, r;
      soryuusi::cin >> l >> r;
      soryuusi::cout << sequence.prod(l,r).sum.val() << '\n';
    }
  }
}
