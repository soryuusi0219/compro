#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<long long>s(n+1);for(int i=0;i<n;++i){long long x;soryuusi::cin>>x;s[i+1]=s[i]+x;}while(q--){int l,r;soryuusi::cin>>l>>r;soryuusi::cout<<s[r]-s[l]<<'\n';}}
