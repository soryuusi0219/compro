#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include <atcoder/segtree>
#include "library/tree/heavy_light_decomposition.hpp"
using mint=soryuusi::modint998244353;using S=std::pair<mint,mint>;
S op(S f,S g){return {f.first*g.first,f.second*g.first+g.second};}S e(){return {1,0};}
S rev_op(S f,S g){return op(g,f);}
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<S>a(n);for(auto&[x,y]:a){int u,v;soryuusi::cin>>u>>v;x=u;y=v;}soryuusi::HLdecomposition h(n);for(int i=1,u,v;i<n;++i){soryuusi::cin>>u>>v;h.add_edge(u,v);}h.build(0);std::vector<S>b(n);for(int i=0;i<n;++i)b[h.inid(i)]=a[i];atcoder::segtree<S,op,e>forward(b);atcoder::segtree<S,rev_op,e>backward(b);while(q--){int t,x,y,z;soryuusi::cin>>t>>x>>y>>z;if(t){S left=e(),right=e();h.for_each(x,y,[&](int l,int r){left=op(left,backward.prod(l,r));},[&](int l,int r){right=op(forward.prod(l,r),right);});auto f=op(left,right);soryuusi::cout<<(f.first*z+f.second).val()<<'\n';}else{int i=h.inid(x);forward.set(i,{y,z});backward.set(i,{y,z});}}}
