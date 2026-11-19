#include <utility>
#include <vector>

#include "library/data_structure/segtree_sparse.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

using Mint = soryuusi::modint998244353;
using Affine = std::pair<Mint,Mint>;
Affine op(Affine f, Affine g){return {g.first*f.first,g.first*f.second+g.second};}
Affine e(){return {1,0};}

int main(){
  int n,q;
  soryuusi::cin >> n >> q;
  struct Query {int type,l,r,x;};
  std::vector<Query> queries(q);
  soryuusi::Segtree_Sparse<Affine,op,e> tree(q);
  for(auto& [type,l,r,x] : queries){
    soryuusi::cin >> type >> l >> r >> x;
    if(type == 0) tree.add_point(l);
  }
  tree.build();
  for(auto [type,l,r,x] : queries){
    if(type == 0) tree.set(l,{r,x});
    else {
      auto [b,c] = tree.prod(l,r);
      soryuusi::cout << (b*x+c).val() << '\n';
    }
  }
}
