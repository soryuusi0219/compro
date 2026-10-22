#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>

#include "library/data_structure/bit.hpp"
#include "library/data_structure/hash_table.hpp"


int main(){
  int n,q;
  soryuusi::cin>>n>>q;
  std::vector<int>a(n);
  for(auto&x:a)soryuusi::cin>>x;
  std::vector<std::vector<std::pair<int,int>>>qs(n);
  for(int i=0,l,r;i<q;++i){
    soryuusi::cin>>l>>r;
    if(l<n)qs[l].push_back({r,i});
  }
  soryuusi::BIT<int>bit(n);
  soryuusi::HashTable<int,int,soryuusi::hash_int<int>> last(n);
  std::vector<int>ans(q);
  for(int i=n-1;i>=0;--i){
    auto x = last.get(a[i],-1);
    if(~x)bit.add(x,-1);
    last[a[i]]=i;
    bit.add(i,1);
    for(auto[r,id]:qs[i])ans[id]=bit.sum(i,r);
  }
  for(auto x:ans)soryuusi::cout<<x<<'\n';
}
