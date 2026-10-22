#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/Segtree_dual.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::modint998244353;

struct Affine {
  mint b, c;
};

mint mapping(Affine f, mint x) {return f.b*x+f.c;}
Affine composition(Affine f, Affine g) {return {f.b*g.b,f.b*g.c+f.c};}
Affine identity() {return {1,0};}

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<mint> a(n);
  for(auto& x : a) soryuusi::cin >> x;
  soryuusi::Segtree_dual<mint,Affine,mapping,composition,identity> seg(a);
  while(q--) {
    int type;
    soryuusi::cin >> type;
    if(type == 0) {
      int l, r, b, c;
      soryuusi::cin >> l >> r >> b >> c;
      seg.apply(l,r,{b,c});
    }else{
      int i;
      soryuusi::cin >> i;
      soryuusi::cout << seg.get(i).val() << '\n';
    }
  }
}
