#include "library/utility/fastio.hpp"
#include <iostream>
#include "library/math/enumerate_quotients.hpp"
int main(){long long n;soryuusi::cin>>n;soryuusi::EnumerateQuotients<long long>q(n);soryuusi::cout<<q.size()<<'\n';for(int i=0;i<q.size();++i)soryuusi::cout<<q[i]<<" \n"[i+1==q.size()];}
