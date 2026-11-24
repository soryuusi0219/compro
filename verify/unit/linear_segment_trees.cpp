#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "library/data_structure/bit.hpp"
#include "library/data_structure/bit_sparse.hpp"
#include "library/data_structure/lazy_segtree.hpp"
#include "library/data_structure/range_set_range_prod.hpp"
#include "library/data_structure/segtree.hpp"
#include "library/data_structure/segtree_beats_templates.hpp"
#include "library/data_structure/segtree_dual.hpp"
#include "library/data_structure/segtree_sparse.hpp"

namespace {

std::string join(const std::string& left, const std::string& right) { return left + right; }
std::string empty() { return {}; }
long long add(long long x, long long y) { return x+y; }
long long zero() { return 0; }

struct Permutation {
  std::array<char,3> to;
};
Permutation id() { return {{'a','b','c'}}; }
char permute(Permutation f, char ch) { return f.to[ch-'a']; }
std::string map_string(Permutation f, std::string s) {
  for(char& ch : s) ch = permute(f,ch);
  return s;
}
Permutation compose(Permutation f, Permutation g) {
  for(char& ch : g.to) ch = permute(f,ch);
  return g;
}

void test_fenwick_and_sparse() {
  soryuusi::BIT<int> empty_bit(std::vector<int>{});
  assert(empty_bit.sum(0) == 0 && empty_bit.lower_bound(0) == 0);

  const std::vector<int> a{0,2,0,7,1,0,4,0};
  soryuusi::BIT<int> bit(a);
  soryuusi::BIT_Sparse<int,long long> sparse;
  soryuusi::Segtree_Sparse<std::string,join,empty,long long> seg;
  constexpr long long step = 10000000000LL;
  for(int i : {7,3,5,0,6,4,2,1,1}) {
    sparse.add_point(i*step);
    seg.add_point(i*step);
  }
  sparse.build();
  seg.build();
  for(int i = 0; i < (int)a.size(); ++i) {
    sparse.add(i*step,a[i]);
    seg.set(i*step,std::string(1,char('a'+i)));
    int prefix = std::accumulate(a.begin(),a.begin()+i+1,0);
    assert(bit.sum(i+1) == prefix);
    assert(sparse.sum(i*step+1) == prefix);
  }
  for(int l = 0; l <= (int)a.size(); ++l) {
    for(int r = l; r <= (int)a.size(); ++r) {
      int sum = std::accumulate(a.begin()+l,a.begin()+r,0);
      assert(bit.sum(l,r) == sum);
      assert(sparse.sum(l*step,r*step) == sum);
      assert(seg.prod(l*step,r*step) == std::string("abcdefgh").substr(l,r-l));
    }
  }
  assert(bit.lower_bound(-1) == -1);
  for(int k = 0; k <= 15; ++k) {
    int p = 0, sum = 0;
    while(p < (int)a.size() && sum+a[p] <= k) sum += a[p++];
    assert(bit.lower_bound(k) == p);
  }
  seg.build();
  sparse.build();
  assert(seg.all_prod().empty() && sparse.sum(step*8) == 0);
}

void test_noncommutative_trees(int n) {
  using Seg = soryuusi::Segtree<std::string,join,empty>;
  using Lazy = soryuusi::LazySegtree<std::string,join,empty,Permutation,map_string,compose,id>;
  using Assign = soryuusi::RangeSetRangeProd<std::string,join,empty>;
  using Dual = soryuusi::Segtree_dual<char,Permutation,permute,compose,id>;

  Seg from_size(n);
  Lazy lazy_from_size(n);
  Assign assign_from_size(n);
  assert(from_size.all_prod().empty() && lazy_from_size.all_prod().empty() && assign_from_size.all_prod().empty());
  std::string expected(n,'a');
  std::vector<std::string> initial(n,"a");
  Seg seg(initial);
  Lazy lazy(initial);
  Assign assign(initial);
  Dual dual(n,'a');
  std::mt19937 rng(4260+n);
  constexpr Permutation actions[] = {{{'b','a','c'}},{{'a','c','b'}},{{'b','c','a'}}};

  auto check_search = [&](int l, int r) {
    const std::string target = expected.substr(l,r-l);
    auto prefix = [&](const std::string& x){return target.starts_with(x);};
    auto suffix = [&](const std::string& x){return target.ends_with(x);};
    assert(seg.max_right(l,prefix) == r && seg.min_left(r,suffix) == l);
    assert(lazy.max_right(l,prefix) == r && lazy.min_left(r,suffix) == l);
    assert(assign.max_right(l,prefix) == r && assign.min_left(r,suffix) == l);
  };
  auto set_range = [&](int l, int r, char ch) {
    assign.range_set(l,r,std::string(1,ch));
    for(int i = l; i < r; ++i) {
      lazy.set(i,std::string(1,ch));
      seg.set(i,std::string(1,ch));
      dual.set(i,ch);
      expected[i] = ch;
    }
  };

  set_range(0,n,'b');
  check_search(n/4,3*n/4);
  check_search(n,n);
  check_search(0,0);
  for(int t = 0; t < 450; ++t) {
    int l = rng()%(n+1), r = rng()%(n+1);
    if(l > r) std::swap(l,r);
    if(t%5 == 0) l = 0, r = n;
    const auto f = actions[rng()%3];
    if(t%3 == 0) {
      set_range(l,r,char('a'+rng()%3));
    } else {
      if(l == 0 && r == n) {
        lazy.all_apply(f);
        dual.all_apply(f);
      } else {
        if(r-l == 1) lazy.apply(l,f);
        else lazy.apply(l,r,f);
        dual.apply(l,r,f);
      }
      for(int i = l; i < r; ++i) {
        expected[i] = permute(f,expected[i]);
        seg.set(i,expected.substr(i,1));
        assign.set(i,expected.substr(i,1));
      }
    }
    const auto target = expected.substr(l,r-l);
    assert(seg.prod(l,r) == target && lazy.prod(l,r) == target && assign.prod(l,r) == target);
    assert(lazy.all_prod() == expected && assign.all_prod() == expected);
    if(l < n) assert(lazy.get(l) == expected.substr(l,1) && assign.get(l) == expected.substr(l,1) && dual.get(l) == expected[l]);
    check_search(l,r);
  }

  if(n > 0) {
    seg.reset(0);
    lazy.reset(0);
    assert(seg.get(0).empty() && lazy.get(0).empty());
  }
}

void test_beats(int n) {
  using Beats = soryuusi::SegtreeBeatsChminChmaxAdd<int,long long>;
  std::vector<int> a(n);
  for(int i = 0; i < n; ++i) a[i] = i%7;
  Beats seg(a);
  Beats from_size(n);
  assert(from_size.all_prod().len == 0);
  std::mt19937 rng(32466+n);
  for(int t = 0; t < 480; ++t) {
    int l = rng()%(n+1), r = rng()%(n+1);
    if(l > r) std::swap(l,r);
    if(t%5 == 0) l = 0, r = n;
    const int x = rng()%8;
    switch(t%4) {
      case 0:
        seg.chmin(l,r,x);
        for(int i = l; i < r; ++i) a[i] = std::min(a[i],x);
        break;
      case 1:
        seg.chmax(l,r,x);
        for(int i = l; i < r; ++i) a[i] = std::max(a[i],x);
        break;
      case 2:
        seg.add(l,r,x);
        for(int i = l; i < r; ++i) a[i] += x;
        break;
      case 3:
        if(l < r) {
          seg.set(l,Beats::value_type(x));
          a[l] = x;
        }
        break;
    }
    const auto sum = std::accumulate(a.begin()+l,a.begin()+r,0LL);
    auto res = seg.prod(l,r);
    assert(res.sum == sum && res.len == r-l && !res.fail);
    if(l < r) assert(res.mn1 == *std::min_element(a.begin()+l,a.begin()+r) && res.mx1 == *std::max_element(a.begin()+l,a.begin()+r));
    assert(seg.all_prod().sum == std::accumulate(a.begin(),a.end(),0LL));
    if(l < n) assert(seg.get(l).sum == a[l]);
    int left = r, right = l;
    long long prefix = 0, suffix = 0;
    while(right < n && prefix+a[right] <= sum) prefix += a[right++];
    while(left > 0 && a[left-1]+suffix <= sum) suffix += a[--left];
    auto fits = [&](Beats::value_type val){return val.sum <= sum;};
    assert(seg.max_right(l,fits) == right && seg.min_left(r,fits) == left);
  }
}

void test_range_set_doubling() {
  soryuusi::RangeSetRangeProd<long long,add,zero> seg(8);
  seg.range_set(0,8,1LL << 59);
  assert(seg.all_prod() == (1LL << 62));
  seg.range_set(3,4,0);
  assert(seg.prod(2,5) == (1LL << 60));
}

}  // namespace

int main() {
  test_fenwick_and_sparse();
  test_range_set_doubling();
  for(int n : {0,1,2,3,7,8,9,15,16,17,31,32,33}) {
    test_noncommutative_trees(n);
    test_beats(n);
  }
  std::cout << "linear_segment_trees: all tests passed\n";
}
