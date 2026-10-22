#include "library/utility/fastio.hpp"

#include <algorithm>
#include <vector>

#include "library/data_structure/mo_rollback.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> a(n), values;
  for(auto& x : a) soryuusi::cin >> x;
  values = a;
  std::sort(values.begin(),values.end());
  values.erase(std::unique(values.begin(),values.end()),values.end());
  for(auto& x : a) x = std::lower_bound(values.begin(),values.end(),x)-values.begin();
  soryuusi::Mo_Rollback mo(n,q);
  for(int i = 0; i < q; ++i) {
    int l, r;
    soryuusi::cin >> l >> r;
    mo.add_query(l,r);
  }
  std::vector<int> count(values.size()), answer(q);
  int distinct = 0;
  auto add = [&](int i) {if(count[a[i]]++ == 0) ++distinct;};
  auto del = [&](int i) {if(--count[a[i]] == 0) --distinct;};
  auto reset = [&] {std::fill(count.begin(),count.end(),0); distinct = 0;};
  auto snapshot = [] {};
  auto rollback = [] {};
  auto solve = [&](int i) {answer[i] = distinct;};
  mo.run(add,del,reset,snapshot,rollback,solve);
  for(int x : answer) soryuusi::cout << x << '\n';
}
