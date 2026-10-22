#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "library/data_structure/Segtree_dual.hpp"
#include "library/data_structure/bit_2d.hpp"
#include "library/data_structure/bit_sparse.hpp"
#include "library/data_structure/bit_vector.hpp"
#include "library/data_structure/increment_decrement_ordered_multi_set.hpp"
#include "library/data_structure/merge_sort_tree_segtree.hpp"
#include "library/data_structure/mo_rollback.hpp"
#include "library/data_structure/persistent_dsu_with_data.hpp"
#include "library/data_structure/persistent_dynamic_segtree.hpp"
#include "library/data_structure/segtree_2d.hpp"
#include "library/data_structure/segtree_2d_sparse.hpp"
#include "library/data_structure/segtree_sparse.hpp"

namespace {

long long add(long long a, long long b) { return a+b; }
long long zero() { return 0; }
long long mapping(long long f, long long x) { return f+x; }
long long composition(long long f, long long g) { return f+g; }

void test_dual_segtree() {
  std::vector<long long> expected{1,2,3,4,5};
  soryuusi::Segtree_dual<long long,long long,mapping,composition,zero> seg(expected);
  seg.apply(1,4,7);
  for(int i = 1; i < 4; ++i) expected[i] += 7;
  seg.all_apply(-2);
  for(auto& x : expected) x -= 2;
  seg.set(2,100);
  expected[2] = 100;
  for(int i = 0; i < 5; ++i) assert(seg.get(i) == expected[i]);
}

void test_bits() {
  std::vector<std::vector<long long>> a{{1,2,3},{4,5,6}};
  soryuusi::BIT2D<long long> bit2(a);
  assert(bit2.sum(0,2,0,3) == 21);
  bit2.add(1,1,10);
  assert(bit2.sum(1,2,1,3) == 21);

  soryuusi::BIT_Sparse<long long,long long> sparse(5);
  for(long long x : {-5,2,9,20}) sparse.add_point(x);
  sparse.build();
  sparse.add(-5,3);
  sparse.add(9,7);
  assert(sparse.sum(-10,10) == 10);
  assert(sparse.sum(2,9) == 0);

  std::vector<unsigned long long> basis;
  assert(soryuusi::f(basis,3ULL));
  assert(soryuusi::f(basis,5ULL));
  assert(!soryuusi::f(basis,6ULL));
}

void test_ordered_multiset() {
  soryuusi::IncrementDecrementOrderedMultiSet values(6,6,2);
  std::vector<int> a(6,2);
  values.increment(3); ++a[3];
  values.increment(1); ++a[1];
  values.decrement(3); --a[3];
  values.decrement(0); --a[0];
  for(int x = 0; x <= 6; ++x) {
    assert(values.count_lt(x) == std::count_if(a.begin(),a.end(),[&](int y){return y < x;}));
  }
  std::multiset<int> ordered(a.begin(),a.end());
  for(int k = 0; k < 6; ++k) assert(a[values.kth(k)] == *std::next(ordered.begin(),k));
}

void test_merge_sort_tree() {
  std::vector<int> key{4,1,3,2,5};
  std::vector<long long> value{10,20,30,40,50};
  soryuusi::MergeSortTree_Segtree<int,long long,add,zero> seg(key,value);
  assert(seg.prod(1,4) == 90);
  assert(seg.prod_interval(0,5,2,5) == 80);
  seg.set(2,100);
  value[2] = 100;
  assert(seg.get(2) == 100);
  assert(seg.prod_interval(1,5,1,4) == 160);
}

void test_mo_rollback() {
  std::vector<int> a{3,1,4,1,5,9,2};
  std::vector<std::pair<int,int>> queries{{0,3},{1,6},{2,2},{0,7},{4,7}};
  soryuusi::Mo_Rollback mo(a.size(),queries.size());
  for(auto [l,r] : queries) mo.add_query(l,r);
  int sum = 0;
  std::vector<int> history, answer(queries.size());
  auto add_index = [&](int i){sum += a[i];};
  auto del_index = [&](int i){sum -= a[i];};
  auto reset = [&]{sum = 0; history.clear();};
  auto snapshot = [&]{history.push_back(sum);};
  auto rollback = [&]{sum = history.back(); history.pop_back();};
  auto solve = [&](int i){answer[i] = sum;};
  mo.run(add_index,del_index,reset,snapshot,rollback,solve);
  for(int i = 0; i < (int)queries.size(); ++i) {
    auto [l,r] = queries[i];
    assert(answer[i] == std::accumulate(a.begin()+l,a.begin()+r,0));
  }
}

void test_persistent_dsu() {
  soryuusi::Persistent_dsu<long long,add> dsu;
  auto root0 = dsu.init(4,1);
  auto root1 = dsu.merge(0,1,root0).second;
  auto root2 = dsu.merge(1,2,root1).second;
  assert(!dsu.same(0,1,root0));
  assert(dsu.same(0,1,root1) && !dsu.same(0,2,root1));
  assert(dsu.same(0,2,root2));
  assert(dsu.size(0,root2) == 3 && dsu.get(0,root2) == 3);
  auto root3 = dsu.set(1,10,root2);
  assert(dsu.get(2,root3) == 10 && dsu.get(2,root2) == 3);
}

void test_persistent_dynamic_segtree() {
  soryuusi::Dynamic_Segtree<long long,add,zero> seg(16);
  auto root0 = seg.instance();
  auto root1 = seg.set(root0,3,5);
  auto root2 = seg.set(root1,10,7);
  auto root3 = seg.set(root2,3,2);
  assert(seg.prod(root0,0,16) == 0);
  assert(seg.prod(root1,0,16) == 5);
  assert(seg.prod(root2,0,16) == 12 && seg.all_prod(root2) == 12);
  assert(seg.get(root3,3) == 2 && seg.prod(root3,3,11) == 9);
  assert(seg.max_right(root2,0,[](long long x){return x <= 5;}) == 10);
  assert(seg.min_left(root2,16,[](long long x){return x <= 7;}) == 4);
}

void test_segment_trees() {
  std::vector<std::vector<long long>> a{{1,2,3},{4,5,6},{7,8,9}};
  soryuusi::Segtree2D<long long,add,zero> seg2(a);
  assert(seg2.prod(0,3,0,3) == 45);
  assert(seg2.prod(1,3,1,3) == 28);
  seg2.set(1,1,20);
  assert(seg2.get(1,1) == 20 && seg2.all_prod() == 60);

  soryuusi::Segtree_Sparse<long long,add,zero,long long> sparse(4);
  for(long long x : {-10,2,8,30}) sparse.add_point(x);
  sparse.build();
  sparse.set(-10,4); sparse.set(2,5); sparse.set(30,7);
  assert(sparse.prod(-20,9) == 9 && sparse.all_prod() == 16);

  soryuusi::Segtree2D_Sparse<long long,add,zero,long long,long long> sparse2(4);
  std::vector<std::pair<long long,long long>> points{{-2,3},{1,4},{5,-1},{5,7}};
  for(auto [x,y] : points) sparse2.add_point(x,y);
  sparse2.build();
  sparse2.add(-2,3,2); sparse2.add(1,4,5); sparse2.add(5,-1,7); sparse2.add(5,7,11);
  assert(sparse2.prod(-3,6,-2,5) == 14);
  assert(sparse2.all_prod() == 25);
}

}  // namespace

int main() {
  test_dual_segtree();
  test_bits();
  test_ordered_multiset();
  test_merge_sort_tree();
  test_mo_rollback();
  test_persistent_dsu();
  test_persistent_dynamic_segtree();
  test_segment_trees();
  std::cout << "data_structure_misc: all tests passed\n";
}
