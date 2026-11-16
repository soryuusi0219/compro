#include "library/utility/fastio.hpp"
#include "library/data_structure/splay_explicit.hpp"

long long op(long long a,long long b){return a+b;}
long long e(){return 0;}
int main(){
  soryuusi::SplayExplicit<int,long long,op,e,false> arena;
  auto set=arena.new_set();
  int n,q;soryuusi::cin>>n>>q;
  while(n--){int x;soryuusi::cin>>x;set.insert(x,x);}
  while(q--){
    int type,x;soryuusi::cin>>type>>x;
    if(type==0)set.insert(x,x);
    else if(type==1)set.erase(x);
    else if(type==2)soryuusi::cout<<(x>0 && x<=set.size() ? set.kth(x-1).first : -1)<<'\n';
    else if(type==3)soryuusi::cout<<set.upper_bound(x)<<'\n';
    else if(type==4){int p=set.upper_bound(x);soryuusi::cout<<(p ? set.kth(p-1).first : -1)<<'\n';}
    else{int p=set.lower_bound(x);soryuusi::cout<<(p<set.size() ? set.kth(p).first : -1)<<'\n';}
  }
}
