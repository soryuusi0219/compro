#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <atcoder/segtree>
#include "library/data_structure/point_set_range_count.hpp"
using S=std::pair<int,int>;
S op(S x,S y){if(x.first==y.first)return{x.first,x.second+y.second};if(x.second>y.second)return{x.first,x.second-y.second};return{y.first,y.second-x.second};}
S e(){return{-1,0};}
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<int>a(n);std::vector<S>v(n);for(int i=0;i<n;++i){soryuusi::cin>>a[i];v[i]={a[i],1};}atcoder::segtree<S,op,e>seg(v);soryuusi::PointSetRangeCount<int,soryuusi::hash_int<int>>counter(a,n+q);while(q--){int t,x,y;soryuusi::cin>>t>>x>>y;if(t==0){seg.set(x,{y,1});counter.set(x,y);}else{int candidate=seg.prod(x,y).first;soryuusi::cout<<(counter.count(x,y,candidate)*2>y-x?candidate:-1)<<'\n';}}}
