#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/tree/01_on_tree.hpp"
int main(){int n;soryuusi::cin>>n;std::vector<long long>p(n,-1),c(n),d(n);for(int i=1;i<n;++i)soryuusi::cin>>p[i];for(auto&x:c)soryuusi::cin>>x;for(auto&x:d)soryuusi::cin>>x;auto[ans,v]=soryuusi::solve01onTree(n,p,c,d);soryuusi::cout<<ans<<'\n';for(int i=0;i<n;++i)soryuusi::cout<<v[i]<<" \n"[i+1==n];}
