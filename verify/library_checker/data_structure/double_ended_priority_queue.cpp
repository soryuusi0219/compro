#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/interval_heap.hpp"

int e() {return 0;}
int op(int x, int y) {return x+y;}

int main(){
  int n,q; soryuusi::cin >> n >> q;
  std::vector<int> s(n);
  for(auto& x : s) soryuusi::cin >> x;
  soryuusi::Interval_Heap<int, std::greater<int>> que(s);
  while(q--){
    int t;
    soryuusi::cin >> t;
    if(t == 0) {
      int x;
      soryuusi::cin >> x;
      que.push(x);
    }else if(t == 1) {
      soryuusi::cout << que.get_max() << "\n";
      que.pop_max();
    }else {
      soryuusi::cout << que.get_min() << "\n";
      que.pop_min();
    }
  }
}
