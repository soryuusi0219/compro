#include "library/utility/fastio.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include "library/math/modint.hpp"
using mint=soryuusi::modint998244353;
int main(){int n;soryuusi::cin>>n;std::vector<long long>a(n);for(auto&x:a)soryuusi::cin>>x;std::unordered_map<long long,int>last;std::vector<int>prev(n);for(int i=0;i<n;++i){auto it=last.find(a[i]);if(it!=last.end())prev[i]=it->second+1;last[a[i]]=i;}std::vector<mint>dp(n+1),sum(n+1);dp[0]=1;for(int i=0;i<n;++i){sum[i+1]=sum[i]+dp[i];dp[i+1]=sum[i+1]-sum[prev[i]];}soryuusi::cout<<(sum[n]+dp[n]-1).val()<<'\n';}
