#include "library/utility/fastio.hpp"
#include "library/data_structure/splay_explicit.hpp"
#include <utility>
#include <vector>
#include "library/math/modint.hpp"
using mint=soryuusi::modint998244353;
using S=std::pair<mint,int>;
using F=std::pair<mint,mint>;
S op(S a,S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
S mapping(F f,S x){return {f.first*x.first+f.second*x.second,x.second};}
F composition(F f,F g){return {f.first*g.first,f.first*g.second+f.second};}
F id(){return {1,0};}
int main(){
  soryuusi::LazySplayExplicit<int,S,op,e,F,mapping,composition,id,false> arena;
  int n,q;soryuusi::cin>>n>>q;
  std::vector<std::pair<int,S>> values;
  for(int i=0;i<n;++i){int x;soryuusi::cin>>x;values.push_back({i,{x,1}});}
  auto set=arena.new_set(std::move(values));
  while(q--){int type,l,r;soryuusi::cin>>type>>l>>r;
    if(type==0){int b,c;soryuusi::cin>>b>>c;set.apply_key(l,r,{b,c});}
    else soryuusi::cout<<set.prod_key(l,r).first.val()<<'\n';
  }
}
