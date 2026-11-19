#include <utility>
#include <vector>

#include "library/data_structure/segtree.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

using Mint = soryuusi::modint998244353;
using Affine = std::pair<Mint,Mint>;
Affine op(Affine f, Affine g){return {g.first*f.first,g.first*f.second+g.second};}
Affine e(){return {1,0};}

int main(){
  int n,q;
  soryuusi::cin >> n >> q;
  std::vector<Affine> a(n);
  for(auto& [b,c] : a) soryuusi::cin >> b >> c;
  soryuusi::Segtree<Affine,op,e> tree(a);
  while(q--){
    int type,l,r,x;
    soryuusi::cin >> type >> l >> r >> x;
    if(type == 0) tree.set(l,{r,x});
    else {
      auto [b,c] = tree.prod(l,r);
      soryuusi::cout << (b*x+c).val() << '\n';
    }
  }
}
