#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
int main(){int n;long long mod;soryuusi::cin>>n>>mod;std::vector<long long>a(n);if(n>1)a[1]=1%mod;for(int i=2;i<n;++i)a[i]=(a[i-2]+a[i-1])*i%mod;for(int i=0;i<n;++i)soryuusi::cout<<a[i]<<" \n"[i+1==n];}
