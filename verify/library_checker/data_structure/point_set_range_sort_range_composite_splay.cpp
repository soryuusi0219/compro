#include <utility>
#include <vector>
#include "library/data_structure/splay_sortable.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

using Mint = soryuusi::modint998244353;
using Affine = std::pair<Mint,Mint>;
Affine op(Affine f, Affine g){return {g.first*f.first,g.first*f.second+g.second};}
Affine e(){return {1,0};}

int main(){
  int n,q;
  soryuusi::cin >> n >> q;
  std::vector<std::pair<int,Affine>> entries(n);
  for(auto& [key,f] : entries){
    int a,b;
    soryuusi::cin >> key >> a >> b;
    f = {a,b};
  }
  soryuusi::SplaySortable<int,Affine,op,e> arena;
  auto seq = arena.new_sequence(entries);
  while(q--){
    int type,l,r;
    soryuusi::cin >> type >> l >> r;
    if(type == 0){
      int a,b;
      soryuusi::cin >> a >> b;
      seq.set(l,r,Affine{a,b});
    }else if(type == 1){
      int x;
      soryuusi::cin >> x;
      auto [a,b] = seq.prod(l,r);
      soryuusi::cout << (a*x+b).val() << '\n';
    }else{
      seq.sort(l,r,type == 3);
    }
  }
}
