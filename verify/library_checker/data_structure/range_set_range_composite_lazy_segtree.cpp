#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>

#include "library/math/modint.hpp"

#include "library/data_structure/lazy_segtree.hpp"

using mint = soryuusi::modint998244353;
using affine = std::pair<mint, mint>;
affine identity() { return {1, 0}; }
affine compose(affine left, affine right) {
  return {left.first * right.first, right.first * left.second + right.second};
}
affine pow(affine f, int n){
  affine g = identity();
  while(n){
    if(n&1) g = compose(g,f);
    f = compose(f,f);
    n >>= 1;
  }
  return g;
}
struct S{affine f; int len;};
S op(S x, S y){return {compose(x.f,y.f), x.len + y.len};}
S e(){return {identity(),0};}
struct F{affine f; bool flag;};
S mapping(F x, S y){
  return x.flag ? S{pow(x.f,y.len), y.len} : y;
}
F composition(F x, F y){return x.flag ? x : y;}
F id(){return {identity(),false};}


int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<S> functions(n);
  for (auto& [f, len] : functions) {
    int av, bv;
    soryuusi::cin >> av >> bv;
    f = affine{av, bv};
    len = 1;
  }
  soryuusi::LazySegtree<S, op, e, F, mapping, composition, id> seg(functions);
  while (q--) {
    int type, left, right;
    soryuusi::cin >> type >> left >> right;
    if (type == 0) {
      int a, b;
      soryuusi::cin >> a >> b;
      seg.apply(left, right, F{affine{a, b}, true});
    } else {
      int x;
      soryuusi::cin >> x;
      auto [a, b] = seg.prod(left, right).f;
      soryuusi::cout << (a * x + b).val() << '\n';
    }
  }
}
