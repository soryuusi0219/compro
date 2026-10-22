#include "library/utility/fastio.hpp"

#include <algorithm>
#include <vector>

#include "library/data_structure/merge_sort_tree_segtree.hpp"

int op(int x, int y) {return x+y;}
int identity() {return 0;}

struct Query {
  int type, a, b, c;
};

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> a(n);
  std::vector<std::vector<int>> candidates(n);
  for(int i = 0; i < n; ++i) {
    soryuusi::cin >> a[i];
    candidates[i].push_back(a[i]);
  }
  std::vector<Query> queries(q);
  for(auto& query : queries) {
    soryuusi::cin >> query.type;
    if(query.type == 0) {
      soryuusi::cin >> query.a >> query.b;
      candidates[query.a].push_back(query.b);
    }else{
      soryuusi::cin >> query.a >> query.b >> query.c;
    }
  }
  std::vector<int> offset(n+1), keys;
  for(int i = 0; i < n; ++i) {
    auto& values = candidates[i];
    std::sort(values.begin(),values.end());
    values.erase(std::unique(values.begin(),values.end()),values.end());
    offset[i+1] = offset[i]+values.size();
    keys.insert(keys.end(),values.begin(),values.end());
  }
  std::vector<int> values(keys.size());
  auto index = [&](int i, int x) {
    return offset[i]+std::lower_bound(candidates[i].begin(),candidates[i].end(),x)-candidates[i].begin();
  };
  for(int i = 0; i < n; ++i) values[index(i,a[i])] = 1;
  soryuusi::MergeSortTree_Segtree<int,int,op,identity> seg(keys,values);
  for(const auto& query : queries) {
    if(query.type == 0) {
      seg.set(index(query.a,a[query.a]),0);
      a[query.a] = query.b;
      seg.set(index(query.a,a[query.a]),1);
    }else{
      soryuusi::cout << seg.prod_interval(offset[query.a],offset[query.b],query.c,query.c,true,true) << '\n';
    }
  }
}
