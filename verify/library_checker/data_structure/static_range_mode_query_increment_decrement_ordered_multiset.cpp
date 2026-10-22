#include "library/utility/fastio.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "library/data_structure/increment_decrement_ordered_multi_set.hpp"
#include "library/data_structure/mo.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> a(n), key;
  for(auto& x : a) soryuusi::cin >> x;
  key = a;
  std::sort(key.begin(),key.end());
  key.erase(std::unique(key.begin(),key.end()),key.end());
  for(auto& x : a) x = std::lower_bound(key.begin(),key.end(),x)-key.begin();
  soryuusi::Mo mo(n,q);
  for(int i = 0; i < q; ++i) {
    int l, r;
    soryuusi::cin >> l >> r;
    mo.add_query(l,r);
  }
  const int m = key.size();
  soryuusi::IncrementDecrementOrderedMultiSet ordered(m,n+1);
  std::vector<int> count(m);
  std::vector<std::pair<int,int>> answer(q);
  auto add = [&](int i) {ordered.increment(a[i]); ++count[a[i]];};
  auto del = [&](int i) {ordered.decrement(a[i]); --count[a[i]];};
  auto solve = [&](int i) {
    const int id = ordered.kth(m-1);
    answer[i] = {key[id],count[id]};
  };
  mo.run(add,add,del,del,solve);
  for(auto [value,count_] : answer) soryuusi::cout << value << ' ' << count_ << '\n';
}
