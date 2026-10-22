#include "library/utility/fastio.hpp"

#include "library/data_structure/persistent_dynamic_segtree.hpp"

int op(int x, int y) {return x+y;}
int identity() {return 0;}

int main() {
  constexpr int max_value = 1000000001;
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Dynamic_Segtree<int,op,identity> seg(max_value);
  auto root = seg.instance();
  while(n--) {
    int x;
    soryuusi::cin >> x;
    root = seg.set(root,x,1);
  }
  while(q--) {
    int type, x;
    soryuusi::cin >> type >> x;
    if(type == 0) root = seg.set(root,x,1);
    else if(type == 1) root = seg.set(root,x,0);
    else if(type == 2) {
      const int answer = seg.max_right(root,0,[&](int count) {return count < x;});
      soryuusi::cout << (answer == max_value ? -1 : answer) << '\n';
    }else if(type == 3) {
      soryuusi::cout << seg.prod(root,0,x+1) << '\n';
    }else if(type == 4) {
      soryuusi::cout << seg.min_left(root,x+1,[](int count) {return count == 0;})-1 << '\n';
    }else{
      const int answer = seg.max_right(root,x,[](int count) {return count == 0;});
      soryuusi::cout << (answer == max_value ? -1 : answer) << '\n';
    }
  }
}
