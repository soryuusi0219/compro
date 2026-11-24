#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "library/data_structure/bit_2d.hpp"
#include "library/data_structure/bit_sparse.hpp"
#include "library/data_structure/bit_vector.hpp"
#include "library/data_structure/increment_decrement_ordered_multi_set.hpp"
#include "library/data_structure/interval_heap.hpp"
#include "library/data_structure/merge_sort_tree_segtree.hpp"
#include "library/data_structure/mo.hpp"
#include "library/data_structure/mo_rollback.hpp"
#include "library/data_structure/persistent_dsu_with_data.hpp"
#include "library/data_structure/persistent_dynamic_segtree.hpp"
#include "library/data_structure/segtree_2d.hpp"
#include "library/data_structure/segtree_2d_sparse.hpp"
#include "library/data_structure/segtree_dual.hpp"
#include "library/data_structure/segtree_sparse.hpp"
#include "library/data_structure/static_range_inversions.hpp"
#include "library/data_structure/swag.hpp"
#include "library/data_structure/swag_deque.hpp"

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
  soryuusi::IncrementDecrementOrderedMultiSet ceiling(65,2,2);
  for(int round = 0; round < 200; ++round) {
    int i = round%65;
    ceiling.decrement(i);
    assert(ceiling.count_interval(1,2) == 1 && ceiling.kth(0) == i);
    ceiling.increment(i);
    assert(ceiling.count_lt(2) == 0 && ceiling.count_lt(3) == 65);
  }
  soryuusi::IncrementDecrementOrderedMultiSet empty(0,0);
  assert(empty.count_interval(-1,1) == 0);
}

std::string combine_text(const std::string& a, const std::string& b) {return a+b;}
std::string empty_text() {return {};}

void test_swag(){
  soryuusi::swag<std::string,combine_text,empty_text> queue;
  soryuusi::swagdq<std::string,combine_text,empty_text> deque;
  std::deque<char> reference;
  for(int i = 0; i < 65; ++i){
    std::string value(1,'a'+i%26);
    queue.push(value);
    deque.push_back(value);
    reference.push_back(value.front());
  }
  while(!reference.empty()){
    assert(queue.prod() == std::string(reference.rbegin(),reference.rend()));
    assert(deque.prod() == std::string(reference.begin(),reference.end()));
    queue.pop();
    deque.pop_front();
    reference.pop_front();
  }
  assert(queue.empty() && queue.prod().empty() && deque.empty() && deque.prod().empty());
  std::mt19937 rng(90726);
  for(int i = 0; i < 4000; ++i){
    char value = 'a'+rng()%26;
    if(reference.empty() || rng()%2){
      if(rng()%2){deque.push_front(std::string(1,value)); reference.push_front(value);}
      else {deque.push_back(std::string(1,value)); reference.push_back(value);}
    }else{
      if(rng()%2){deque.pop_front(); reference.pop_front();}
      else {deque.pop_back(); reference.pop_back();}
    }
    assert(deque.prod() == std::string(reference.begin(),reference.end()));
  }
}

template<class Compare> void test_interval_heap(Compare compare){
  std::mt19937 rng(50384);
  for(int n : {0,1,2,3,63,64,65}){
    std::vector<int> values(n);
    for(auto& x : values) x = int(rng()%13)-6;
    std::multiset<int,Compare> reference(values.begin(),values.end(),compare);
    soryuusi::Interval_Heap<int,Compare> heap(values.begin(),values.end(),compare);
    for(int i = 0; i < 2400; ++i){
      assert(heap.size() == static_cast<int>(reference.size()) && heap.empty() == reference.empty());
      if(reference.empty() || rng()%2){
        int x = int(rng()%13)-6;
        heap.push(x);
        reference.insert(x);
      }else{
        assert(heap.get_min() == *reference.begin() && heap.get_max() == *reference.rbegin());
        if(i%2){heap.pop_min(); reference.erase(reference.begin());}
        else {heap.pop_max(); reference.erase(std::prev(reference.end()));}
      }
    }
  }
}

void test_xor_basis(){
  using T = std::int8_t;
  std::vector<T> basis;
  std::vector<unsigned> span{0};
  for(unsigned input : {128,225,97,16,64,63,255,32,8,1,7,2,4,5}){
    bool independent = std::find(span.begin(),span.end(),input) == span.end();
    assert(soryuusi::f(basis,static_cast<T>(input)) == independent);
    if(independent){
      int size = span.size();
      for(int i = 0; i < size; ++i) span.push_back(span[i]^input);
    }
    for(std::size_t i = 0; i < basis.size(); ++i){
      if(auto row = static_cast<std::uint8_t>(basis[i])){
        assert((row>>i) == 1);
        for(std::size_t j = 0; j < basis.size(); ++j) assert(i == j || !basis[j] || !((row>>j)&1));
      }
    }
  }
  for(auto value : span) assert(!soryuusi::f(basis,static_cast<T>(value)));
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
  constexpr int high = std::numeric_limits<int>::max();
  soryuusi::Mo_Rollback tail(high,4);
  tail.b = high-10;
  tail.add_query(high-3,high);
  long long sum_tail = 0, result = 0;
  tail.run([&](int i){sum_tail += i;},[&](int i){sum_tail -= i;},[&]{sum_tail = 0;},
           []{},[]{},[&](int i){assert(i == 0); result = sum_tail;});
  assert(result == 3LL*high-6 && sum_tail == 0);
}

void test_mo(){
  soryuusi::Mo mo(65,20);
  std::vector<std::pair<int,int>> ranges{{0,0},{65,65},{65-1,65},{0,65},{17,19},{7,32},{12,12},{2,4}};
  for(auto [l,r] : ranges) mo.add_query(l,r);
  long long sum = 0;
  std::vector<int> visited(ranges.size());
  auto add = [&](int i){sum += i;};
  auto del = [&](int i){sum -= i;};
  mo.run(add,add,del,del,[&](int id){
    auto [l,r] = ranges[id];
    assert(sum == (r-l)*(l+r-1)/2);
    ++visited[id];
  });
  assert(std::all_of(visited.begin(),visited.end(),[](int n){return n == 1;}));
}

struct InversionKey {int value;};
struct CompareInversionKey {
  bool operator()(const InversionKey& a, const InversionKey& b) const {return a.value/3 < b.value/3;}
};

void test_static_range_inversions(){
  std::mt19937 rng(90183);
  std::vector<InversionKey> values(65);
  for(auto& x : values) x.value = rng()%32;
  CompareInversionKey compare;
  for(int q : {-1,0,4,65,10000}){
    soryuusi::StaticRangeInversions<InversionKey,CompareInversionKey> tree(values,q);
    for(int l = 0; l <= (int)values.size(); ++l){
      assert(tree.query(l,l) == 0);
      int expected = 0;
      for(int r = l; r < (int)values.size(); ++r){
        for(int i = l; i < r; ++i) expected += compare(values[r],values[i]);
        assert(tree.query(l,r+1) == expected);
      }
    }
  }
  std::vector<int> decreasing(100000);
  std::iota(decreasing.rbegin(),decreasing.rend(),0);
  assert(soryuusi::StaticRangeInversions<int>(decreasing).query(5,100000) == 99995LL*99994/2);
  std::swap(decreasing[49999],decreasing[50000]);
  for(int q : {0,16,81}){
    soryuusi::StaticRangeInversions<int> tree(decreasing,q);
    for(auto [l,r] : std::vector<std::pair<int,int>>{{0,100000},{1,99999},{9,100000},{22500,90000},{22499,90001},{91001,99999}}){
      long long n = r-l;
      assert(tree.query(l,r) == n*(n-1)/2-(l <= 49999 && 50000 < r));
    }
  }
}

void test_persistent_dsu() {
  soryuusi::PersistentDSUWithData<long long,add> dsu;
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
  soryuusi::PersistentDynamicSegtree<long long,add,zero> seg(16);
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
  test_swag();
  test_interval_heap(std::less<int>{});
  test_interval_heap(std::greater<int>{});
  test_xor_basis();
  test_merge_sort_tree();
  test_mo_rollback();
  test_mo();
  test_static_range_inversions();
  test_persistent_dsu();
  test_persistent_dynamic_segtree();
  test_segment_trees();
  std::cout << "data_structure_misc: all tests passed\n";
}
