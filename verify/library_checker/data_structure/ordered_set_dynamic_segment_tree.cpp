#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/dynamic_segtree.hpp"

int e() {return 0;}
int op(int x, int y) {return x+y;}

int main(){
  const int MX = 1 << 30;
  int n,q; soryuusi::cin >> n >> q;
  soryuusi::Dynamic_Segtree<int,op,e> seg(MX);
  while(n--){
    int x;
    soryuusi::cin >> x;
    seg.set(x,1);
  }
  while(q--){
    int t,x;
    soryuusi::cin >> t >> x;
    if(t == 0) seg.set(x,1);
    else if(t == 1) seg.set(x,0);
    else if(t == 2) soryuusi::cout << (MX - (MX - seg.max_right(0,[&x](int i) -> bool {return i < x;}) ? : MX+1)) << '\n';
    else if(t == 3) soryuusi::cout << seg.prod(0,x+1) << '\n';
    else if(t == 4) soryuusi::cout << (seg.min_left(x+1,[](int i) -> bool {return !i;}) - 1) << '\n';
    else if(t == 5) soryuusi::cout << (MX - (MX - seg.max_right(x,[](int i) -> bool {return !i;}) ? : MX+1)) << '\n';
  }
}
