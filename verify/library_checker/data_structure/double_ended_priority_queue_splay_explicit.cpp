#include "library/utility/fastio.hpp"
#include "library/data_structure/splay_explicit.hpp"

long long op(long long a,long long b){return a+b;}
long long e(){return 0;}
int main(){
  soryuusi::SplayExplicit<int,long long,op,e> arena;
  auto set=arena.new_set();
  int n,q;soryuusi::cin>>n>>q;
  while(n--){int x;soryuusi::cin>>x;set.insert(x,x);}
  while(q--){
    int type;soryuusi::cin>>type;
    if(type==0){int x;soryuusi::cin>>x;set.insert(x,x);}
    else{int p=type==1 ? 0 : set.size()-1;soryuusi::cout<<set.kth(p).first<<'\n';set.erase_at(p);}
  }
}
