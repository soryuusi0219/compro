#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/data_structure/bit.hpp"
#include "library/data_structure/mo.hpp"
#include "library/utility/compress.hpp"
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<int>a(n);for(auto&x:a)soryuusi::cin>>x;auto [v,key]=soryuusi::compress(a);soryuusi::Mo mo(n,q);for(int i=0,l,r;i<q;++i){soryuusi::cin>>l>>r;mo.add_query(l,r);}soryuusi::BIT<long long> bit(key.size());long long cur=0,cnt=0;std::vector<long long>ans(q);auto al=[&](int i){cur+=bit.sum(v[i]);bit.add(v[i],1);++cnt;};auto dl=[&](int i){--cnt;bit.add(v[i],-1);cur-=bit.sum(v[i]);};auto ar=[&](int i){cur+=cnt-bit.sum(v[i]+1);bit.add(v[i],1);++cnt;};auto dr=[&](int i){--cnt;bit.add(v[i],-1);cur-=cnt-bit.sum(v[i]+1);};mo.run(al,ar,dl,dr,[&](int i){ans[i]=cur;});for(auto x:ans)soryuusi::cout<<x<<'\n';}
