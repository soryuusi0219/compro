#include "library/utility/fastio.hpp"
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <atcoder/fenwicktree>
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<int>a(n);for(auto&x:a)soryuusi::cin>>x;std::vector<std::vector<std::pair<int,int>>>qs(n);for(int i=0,l,r;i<q;++i){soryuusi::cin>>l>>r;if(l<n)qs[l].push_back({r,i});}atcoder::fenwick_tree<int>bit(n);std::unordered_map<int,int>last;std::vector<int>ans(q);for(int i=n-1;i>=0;--i){auto it=last.find(a[i]);if(it!=last.end())bit.add(it->second,-1);last[a[i]]=i;bit.add(i,1);for(auto[r,id]:qs[i])ans[id]=bit.sum(i,r);}for(auto x:ans)soryuusi::cout<<x<<'\n';}
