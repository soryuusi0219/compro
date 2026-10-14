#include "library/utility/fastio.hpp"
#include <iostream>
#include <string>
#include "library/math/stern_brocot.hpp"
void pf(soryuusi::SB::Fraction x){soryuusi::cout<<x.first<<' '<<x.second<<'\n';}
int main(){int q;soryuusi::cin>>q;while(q--){std::string t;soryuusi::cin>>t;if(t=="DECODE_PATH"){int n;soryuusi::cin>>n;soryuusi::SB::Path p;for(int i=0;i<n;++i){char c;long long k;soryuusi::cin>>c>>k;if(i==0&&c=='L')p.push_back(0);p.push_back(k);}pf(soryuusi::SB::decode_path(p));}else if(t=="ENCODE_PATH"){long long a,b;soryuusi::cin>>a>>b;auto p=soryuusi::SB::encode_path({a,b});soryuusi::cout<<p.size()-(!p.empty()&&!p.front());bool right=false;for(auto k:p){if(k)soryuusi::cout<<(right?" L ":" R ")<<k;right=!right;}soryuusi::cout<<'\n';}else if(t=="LCA"){long long a,b,c,d;soryuusi::cin>>a>>b>>c>>d;pf(soryuusi::SB::lca({a,b},{c,d}));}else if(t=="ANCESTOR"){long long k,a,b;soryuusi::cin>>k>>a>>b;auto x=soryuusi::SB::ancestor(k,{a,b});if(x.first)pf(x);else soryuusi::cout<<-1<<'\n';}else{long long a,b;soryuusi::cin>>a>>b;auto [l,r]=soryuusi::SB::range({a,b});soryuusi::cout<<l.first<<' '<<l.second<<' '<<r.first<<' '<<r.second<<'\n';}}}
