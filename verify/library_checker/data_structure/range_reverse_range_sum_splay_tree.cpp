#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/splay_tree.hpp"

long long op(long long x, long long y) { return x+y; }
long long identity() { return 0; }

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> value(n);
  for(auto& x : value) soryuusi::cin >> x;
  soryuusi::splay_tree<long long,op,identity> sequence(value);
  while(q--) {
    int type, l, r;
    soryuusi::cin >> type >> l >> r;
    if(type == 0) sequence.flip(l,r);
    else soryuusi::cout << sequence.prod(l,r) << '\n';
  }
}
