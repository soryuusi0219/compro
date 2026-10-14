#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include "library/math/modint.hpp"
#include "library/data_structure/persistent_dynamic_lazy_segtree.hpp"
using mint=soryuusi::modint998244353;using S=std::pair<mint,mint>;S op(S x,S y){return {x.first+y.first,x.second+y.second};}S e(){return {0,0};}S mapping(S f,S x){return {f.first*x.first+f.second*x.second,x.second};}S composition(S f,S g){return {f.first*g.first,f.first*g.second+f.second};}S id(){return {1,0};}
int main(){int n,q;soryuusi::cin>>n>>q;soryuusi::Persistent_Dynamic_LazySegtree<S,op,e,S,mapping,composition,id>seg(n);using NP=decltype(seg)::NP;std::vector<NP>roots(q+1);for(int i=0,x;i<n;++i){soryuusi::cin>>x;roots[0]=seg.set(roots[0],i,{x,1});}for(int i=0;i<q;++i){int t,k;soryuusi::cin>>t>>k;++k;if(t==0){int l,r,b,c;soryuusi::cin>>l>>r>>b>>c;roots[i+1]=seg.apply(roots[k],l,r,{b,c});}else if(t==1){int s,l,r;soryuusi::cin>>s>>l>>r;++s;roots[i+1]=seg.copy(roots[s],roots[k],l,r);}else{int l,r;soryuusi::cin>>l>>r;soryuusi::cout<<seg.prod(roots[k],l,r).first.val()<<'\n';}}}
