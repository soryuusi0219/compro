#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "library/data_structure/merge_sort_tree.hpp"
#include "library/data_structure/merge_sort_tree_bit.hpp"
#include "library/data_structure/merge_sort_tree_segtree.hpp"
#include "library/data_structure/merge_sort_tree_sum.hpp"
#include "library/data_structure/static_bit_vector.hpp"
#include "library/data_structure/wavelet.hpp"
#include "library/data_structure/wavelet_bit.hpp"
#include "library/data_structure/wavelet_segtree.hpp"
#include "library/data_structure/wavelet_sum.hpp"

namespace {

long long plus(long long a, long long b) { return a+b; }
long long zero() { return 0; }
std::string join(const std::string& a, const std::string& b) { return a+b; }
std::string empty() { return {}; }

void test_rank() {
  soryuusi::StaticBitVector rank;
  for(int n : {0,1,63,64,65,127,128,129}) {
    for(int mode = 0; mode < 3; ++mode) {
      rank.build(n);
      auto active = [&](int i){ return mode == 1 || (mode == 2 && i%5 != 0); };
      for(int i = 0; i < n; ++i) if(active(i)) rank.set(i);
      rank.init();
      int ones = 0;
      for(int i = 0; i <= n; ++i) {
        assert(rank.rank1(i) == ones && rank.rank0(i) == i-ones);
        if(i < n) {
          assert(rank.get(i) == active(i));
          ones += active(i);
        }
      }
    }
  }
}

void test_wavelets(int n, int pattern) {
  std::mt19937 rng(6924+n);
  constexpr int B = 8, limit = 1 << B;
  std::vector<int> keys(n);
  std::vector<long long> weights(n);
  for(int i = 0; i < n; ++i) {
    if(pattern == 0) keys[i] = 0;
    if(pattern == 1) keys[i] = limit-1;
    if(pattern == 2) keys[i] = 0b10100100 ^ (rng() & 0b01010001);
    if(pattern == 3) keys[i] = rng()%limit;
    weights[i] = int(rng()%39)-19;
  }
  soryuusi::Wavelet<int,B> a(keys);
  soryuusi::Wavelet_Sum<int,long long,B> b(keys,weights);
  soryuusi::Wavelet_BIT<int,long long,B> c(keys,weights);
  soryuusi::Wavelet_Segtree<int,long long,plus,zero,B> d(keys,weights);
  auto updated = weights;
  for(int t = 0; t < 180; ++t) {
    int l = rng()%(n+1), r = rng()%(n+1);
    if(l > r) std::swap(l,r);
    if(t%13 == 0) l = 0, r = n;
    const int x = t%5 == 0 ? limit : t%5 == 1 ? -1 : t%5 == 2 ? limit+65 : int(rng()%limit);
    const int low = t%7 == 0 ? -50 : std::min(x,int(rng()%limit));
    if(n > 0 && t%3 == 0) {
      int i = rng()%n, add = int(rng()%17)-8;
      c.add(i,add);
      updated[i] += add;
      d.set(i,updated[i]);
      assert(a.get(i) == keys[i] && b.get(i) == keys[i] && c.get(i) == keys[i]);
      assert((d.get(i) == std::pair{keys[i],updated[i]}));
    }
    int eq = 0, lt = 0;
    long long eq_fixed = 0, eq_live = 0, lt_fixed = 0, lt_live = 0, interval_fixed = 0, interval_live = 0, ge_live = 0;
    for(int i = l; i < r; ++i) {
      if(keys[i] == x) ++eq, eq_fixed += weights[i], eq_live += updated[i];
      if(keys[i] < x) ++lt, lt_fixed += weights[i], lt_live += updated[i];
      else ge_live += updated[i];
      if(low <= keys[i] && keys[i] < x) interval_fixed += weights[i], interval_live += updated[i];
    }
    assert(a.rank(l,r,x) == eq && b.rank(l,r,x) == eq && c.rank(l,r,x) == eq && d.rank(l,r,x) == eq);
    assert(a.count_lt(l,r,x) == lt && b.count_lt(l,r,x) == lt && c.count_lt(l,r,x) == lt && d.count_lt(l,r,x) == lt);
    assert(b.sum_with_val(l,r,x) == eq_fixed && c.sum_with_val(l,r,x) == eq_live && d.prod_with_val(l,r,x) == eq_live);
    assert(b.sum_lt(l,r,x) == lt_fixed && c.sum_lt(l,r,x) == lt_live && d.prod_lt(l,r,x) == lt_live);
    assert(b.sum_interval(l,r,low,x) == interval_fixed && c.sum_interval(l,r,low,x) == interval_live);
    assert(d.prod_interval(l,r,low,x) == interval_live && d.prod_geq(l,r,x) == ge_live);
    std::vector<int> sorted(keys.begin()+l,keys.begin()+r);
    std::sort(sorted.begin(),sorted.end());
    assert(a.next(l,r,x) == (lt == r-l ? -1 : sorted[lt]));
    assert(a.prev(l,r,x) == (lt == 0 ? -1 : sorted[lt-1]));
    if(l < r) {
      int k = rng()%(r-l);
      assert(a.range_kth(l,r,k) == sorted[k] && b.range_kth(l,r,k) == sorted[k]);
      assert(c.range_kth(l,r,k) == sorted[k] && d.range_kth(l,r,k) == sorted[k]);
    }
  }
}

void test_wide_unsigned() {
  using U = std::uint64_t;
  constexpr U high = U{1} << 63;
  std::vector<U> keys{high,0,high+7,~U{0}};
  soryuusi::Wavelet<U,64> wave(keys);
  assert(wave.range_kth(0,4,2) == high+7);
  assert(wave.rank(0,4,~U{0}) == 1 && wave.count_lt(0,4,~U{0}) == 3);
}

void test_merge_trees() {
  std::mt19937 rng(307519);
  soryuusi::MergeSortTree<int,std::greater<int>> a;
  soryuusi::MergeSortTree_Sum<int,long long,std::greater<int>> b;
  soryuusi::MergeSortTree_BIT<int,long long,std::greater<int>> c;
  soryuusi::MergeSortTree_Segtree<int,long long,plus,zero,std::greater<int>> d;
  assert(a.count_lt(0,0,0) == 0 && b.prod(0,0) == 0 && c.sum(0,0) == 0 && d.prod(0,0) == 0);
  for(int n : {65,0,1,129,63,64,66,257,33,513,1025}) {
    std::vector<int> keys(n);
    std::vector<long long> weights(n);
    for(int i = 0; i < n; ++i) {
      keys[i] = int(rng()%29)-14;
      weights[i] = int(rng()%35)-17;
    }
    auto values = weights;
    a.build(keys);
    b.build(keys,weights);
    c.build(keys,weights);
    d.build(keys,weights);
    assert(a.b == b.b && a.b == c.b && a.b == d.b);
    assert(a.key.size() == std::size_t(a.b)*n);
    for(int t = 0; t < 220; ++t) {
      if(n && t%3 == 0) {
        int i = rng()%n, add = int(rng()%17)-8;
        c.add(i,add);
        values[i] += add;
        d.set(i,values[i]);
        assert(c.get(i) == values[i] && d.get(i) == values[i]);
      }
      int l = rng()%(n+1), r = rng()%(n+1);
      if(l > r) std::swap(l,r);
      if(t%11 == 0) l = 0, r = n;
      if(t%11 == 1) r = n;
      if(t%11 == 2 && n >= 2) l = 1, r = n-1;
      int x = int(rng()%35)-17, y = x-5;
      bool include_first = t&1, include_last = t&2;
      int before = 0, equal = 0;
      long long fixed_before = 0, fixed_equal = 0, live_before = 0, live_equal = 0, interval = 0;
      for(int i = l; i < r; ++i) {
        if(keys[i] > x) ++before, fixed_before += weights[i], live_before += values[i];
        if(keys[i] == x) ++equal, fixed_equal += weights[i], live_equal += values[i];
        if((keys[i] < x || (include_first && keys[i] == x)) && (keys[i] > y || (include_last && keys[i] == y))) interval += values[i];
      }
      assert(a.count_lt(l,r,x) == before && b.count_lt(l,r,x) == before && c.count_lt(l,r,x) == before);
      assert(a.count_leq(l,r,x) == before+equal && b.count_leq(l,r,x) == before+equal && c.count_leq(l,r,x) == before+equal);
      assert(b.sum_lt(l,r,x) == fixed_before && b.sum_leq(l,r,x) == fixed_before+fixed_equal);
      assert(c.sum_lt(l,r,x) == live_before && c.sum_leq(l,r,x) == live_before+live_equal);
      assert(d.prod_interval(l,r,x,y,include_first,include_last) == interval);
      long long sum = std::accumulate(values.begin()+l,values.begin()+r,0LL);
      assert(c.sum(l,r) == sum && d.prod(l,r) == sum);
      assert(b.prod(l,r) == std::accumulate(weights.begin()+l,weights.begin()+r,0LL));
      if(l < r) {
        std::vector<int> sorted(keys.begin()+l,keys.begin()+r);
        std::sort(sorted.begin(),sorted.end(),std::greater<int>());
        int k = rng()%(r-l);
        assert(a.kth_smallest(l,r,k+1) == sorted[k]);
      }
    }
  }
}

void test_noncommutative_merge() {
  int n = 641;
  std::vector<std::string> keys(n), values(n);
  for(int i = 0; i < n; ++i) {
    keys[i] = std::string(1,char('a'+i%5));
    values[i] = std::to_string(i)+",";
  }
  soryuusi::MergeSortTree_Segtree<std::string,std::string,join,empty> ds(keys,values);
  for(int l : {0,1,2,31,64}) {
    for(int r : {67,96,128,129,577,640,641}) {
      std::string expect;
      auto visit = [&](int a,int b) {
        std::vector<int> order(b-a);
        std::iota(order.begin(),order.end(),a);
        std::sort(order.begin(),order.end(),[&](int i,int j){return keys[i] != keys[j] ? keys[i] < keys[j] : i > j;});
        for(int i : order) if("b" <= keys[i] && keys[i] < "e") expect += values[i];
      };
      for(int a = l, b = r, width = 1; a < b; width <<= 1) {
        if(a & width) visit(a,a+width), a += width;
        if(b & width) b -= width, visit(b,b+width);
      }
      assert(ds.prod_interval(l,r,"b","e") == expect);
    }
  }
}

}  // namespace

int main() {
  test_rank();
  for(int n : {0,1,63,64,65,129}) for(int pattern = 0; pattern < 4; ++pattern) test_wavelets(n,pattern);
  test_wide_unsigned();
  test_merge_trees();
  test_noncommutative_merge();
  std::cout << "static_rank_and_order_statistics: all tests passed\n";
}
