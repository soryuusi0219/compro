#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include "library/math/modint.hpp"
#include "library/data_structure/dynamic_lazy_segtree.hpp"
using mint=soryuusi::modint998244353;using S=std::pair<mint,mint>;S op(S x,S y){return {x.first+y.first,x.second+y.second};}S e(){return {0,0};}S mapping(S f,S x){return {f.first*x.first+f.second*x.second,x.second};}S composition(S f,S g){return {f.first*g.first,f.first*g.second+f.second};}S id(){return {1,0};}
int main(){int n,q;soryuusi::cin>>n>>q;soryuusi::Dynamic_LazySegtree<S,op,e,S,mapping,composition,id>seg(n,[](int l,int r){return S{0,r-l};});while(q--){int t,l,r;soryuusi::cin>>t>>l>>r;if(t==0){int b,c;soryuusi::cin>>b>>c;seg.apply(l,r,{b,c});}else soryuusi::cout<<seg.prod(l,r).first.val()<<'\n';}}
