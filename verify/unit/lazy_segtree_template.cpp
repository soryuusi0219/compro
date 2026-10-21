// Run from the repository root; see lazy_segtree_document.txt.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include "library/data_structure/lazy_segtree_template.hpp"
#include "library/math/modint.hpp"

using namespace soryuusi;
using ll = long long;
std::mt19937 rng(20261015);

int random_int(int n) { return int(rng()%n); }

template<class Seg>
void constructors() {
  Seg empty;
  Seg zeros(5);
  Seg filled(3,2);
  Seg raw(std::vector<int>{0,1,0});
  std::vector<typename Seg::value_type> nodes;
  for(int i = 0; i < 3; ++i) nodes.push_back(raw.get(i));
  Seg aggregate(nodes);
  Seg copy = aggregate;
  Seg moved = std::move(copy);
  moved.set(0,raw.get(1));
  moved.set(0,0);
  assert(empty.max_right(0,[](auto) { return true; }) == 0);
  assert(empty.min_left(0,[](auto) { return true; }) == 0);
  (void)zeros;
  (void)filled;
}

template<bool Assign>
void sums(int n) {
  using Seg = std::conditional_t<Assign,LazySegtreeAssignSum<int,ll>,LazySegtreeAddSum<int,ll>>;
  std::vector<int> a(n);
  Seg seg(a);
  for(int step = 0; step < 1500; ++step) {
    int l = random_int(n+1), r = random_int(n+1);
    if(l > r) std::swap(l,r);
    int x = random_int(21)-10, kind = random_int(6);
    auto change = [&](int i) {
      if constexpr(Assign) a[i] = x;
      else a[i] += x;
    };
    if(kind == 0) {
      if constexpr(Assign) seg.assign(l,r,x);
      else seg.add(l,r,x);
      for(int i = l; i < r; ++i) change(i);
    } else if(kind == 1) {
      seg.all_apply(typename Seg::action_type(x));
      for(int i = 0; i < n; ++i) change(i);
    } else if(kind == 2 && n) {
      int p = random_int(n);
      seg.apply(p,typename Seg::action_type(x));
      change(p);
    } else if(kind == 3 && n) {
      int p = random_int(n);
      seg.set(p,x);
      a[p] = x;
    } else if(kind == 4) {
      // For assignment, nullopt must not overwrite a pending assignment.
      seg.apply(l,r,typename Seg::action_type{});
    }
    auto s = seg.prod(l,r);
    assert(s.sum == std::accumulate(a.begin()+l,a.begin()+r,0LL));
    assert(s.len == r-l);
    assert(seg.all_prod().sum == std::accumulate(a.begin(),a.end(),0LL));
    assert(seg.all_prod().len == n);
    if(n) {
      int p = random_int(n);
      assert(seg.get(p).sum == a[p]);
      assert(seg.get(p).len == 1);
    }
  }
}

template<bool IsMin, bool WithCount, bool Assign>
void extremes(int n) {
  using Add = std::conditional_t<IsMin,LazySegtreeAddMin<ll>,LazySegtreeAddMax<ll>>;
  using Set = std::conditional_t<IsMin,LazySegtreeAssignMin<ll>,LazySegtreeAssignMax<ll>>;
  using Count = std::conditional_t<IsMin,LazySegtreeAddMinCount<ll,ll>,LazySegtreeAddMaxCount<ll,ll>>;
  using Seg = std::conditional_t<WithCount,Count,std::conditional_t<Assign,Set,Add>>;
  const ll identity = IsMin ? std::numeric_limits<ll>::max()/2 : std::numeric_limits<ll>::lowest()/2;
  std::vector<ll> a(n);
  Seg seg(a);
  auto check = [&](int l, int r, auto s) {
    ll value = identity, count = 0;
    for(int i = l; i < r; ++i) {
      if(!count || (IsMin ? a[i] < value : value < a[i])) value = a[i], count = 1;
      else if(a[i] == value) ++count;
    }
    if constexpr(WithCount) {
      assert(s.first == value && s.second == count);
    } else {
      assert(s == value);
    }
  };
  for(int step = 0; step < 1500; ++step) {
    int l = random_int(n+1), r = random_int(n+1);
    if(l > r) std::swap(l,r);
    ll x = random_int(11)-5;
    int kind = random_int(5);
    if(kind <= 1) {
      if(kind == 1) l = 0, r = n;
      if constexpr(Assign) seg.assign(l,r,x);
      else seg.add(l,r,x);
      for(int i = l; i < r; ++i) {
        if constexpr(Assign) a[i] = x;
        else a[i] += x;
      }
    } else if(kind == 2 && n) {
      int p = random_int(n);
      seg.set(p,x);
      a[p] = x;
    } else if(kind == 3) {
      seg.apply(l,r,typename Seg::action_type{});
    }
    check(l,r,seg.prod(l,r));
    check(0,n,seg.all_prod());
    if(n) {
      int p = random_int(n);
      check(p,p+1,seg.get(p));
    }
  }
  if constexpr(WithCount) {
    // The count distinguishes the identity's value from an empty range.
    Seg edge(std::vector<ll>{identity,identity});
    edge.add(0,2,IsMin ? -1 : 1);
    a = {identity+(IsMin ? -1 : 1),identity+(IsMin ? -1 : 1)};
    check(0,2,edge.prod(0,2));
  }
}

template<class T>
void affine(int n) {
  std::vector<T> a(n);
  LazySegtreeAffineSum<T> seg(a);
  for(int step = 0; step < 1500; ++step) {
    int l = random_int(n+1), r = random_int(n+1);
    if(l > r) std::swap(l,r);
    T b = random_int(3)-1, c = random_int(11)-5;
    switch(random_int(7)) {
      case 0: seg.affine(l,r,b,c); break;
      case 1: b = 1; seg.add(l,r,c); break;
      case 2: c = 0; seg.multiply(l,r,b); break;
      case 3: b = 0; seg.assign(l,r,c); break;
      case 4: l = 0; r = n; seg.all_apply({b,c}); break;
      case 5:
        if(n) {
          int p = random_int(n);
          seg.set(p,c);
          a[p] = c;
        }
        b = 1; c = 0; break;
      default: seg.apply(l,r,{b,c}); break;
    }
    for(int i = l; i < r; ++i) a[i] = b*a[i]+c;
    auto s = seg.prod(l,r);
    assert(s.sum == std::accumulate(a.begin()+l,a.begin()+r,T(0)));
    assert(s.len == r-l);
    assert(seg.all_prod().sum == std::accumulate(a.begin(),a.end(),T(0)));
    if(n) {
      int p = random_int(n);
      assert(seg.get(p).sum == a[p]);
    }
  }
}

void xor_and_flip(int n) {
  std::vector<unsigned> a(n);
  std::vector<int> bits(n);
  LazySegtreeXorXor<unsigned> seg(a);
  LazySegtreeFlipInversion<> inv(bits);
  for(int step = 0; step < 1500; ++step) {
    int l = random_int(n+1), r = random_int(n+1);
    if(l > r) std::swap(l,r);
    unsigned x = rng();
    switch(random_int(5)) {
      case 0:
        l = 0; r = n;
        seg.all_apply(x); inv.all_apply(true); break;
      case 1:
        if(n) {
          int p = random_int(n);
          a[p] = x; bits[p] = int(x&1);
          seg.set(p,a[p]); inv.set(p,bits[p]);
        }
        x = 0; l = r = 0; break;
      default: seg.xor_apply(l,r,x); inv.flip(l,r); break;
    }
    for(int i = l; i < r; ++i) a[i] ^= x, bits[i] ^= 1;
    auto check = [&](int left, int right, auto sx, auto si) {
      unsigned value = 0;
      ll zero = 0, one = 0, inversions = 0;
      for(int i = left; i < right; ++i) {
        value ^= a[i];
        if(bits[i]) ++one;
        else ++zero, inversions += one;
      }
      assert(sx.value == value && sx.len == right-left);
      assert(si.zero == zero && si.one == one && si.inversion == inversions);
    };
    check(l,r,seg.prod(l,r),inv.prod(l,r));
    check(0,n,seg.all_prod(),inv.all_prod());
  }
}

void search(int n) {
  LazySegtreeAddSum<int,ll> seg(n);
  std::vector<int> a(n);
  for(int step = 0; step < 500; ++step) {
    int l = random_int(n+1), r = random_int(n+1);
    if(l > r) std::swap(l,r);
    seg.add(l,r,1);
    for(int i = l; i < r; ++i) ++a[i];
    ll limit = random_int(500);
    auto pred = [&](auto s) { return s.sum <= limit; };
    int right = l, left = r;
    ll s = 0;
    while(right < n && s+a[right] <= limit) s += a[right++];
    s = 0;
    while(left > 0 && s+a[left-1] <= limit) s += a[--left];
    assert(seg.max_right(l,pred) == right);
    assert(seg.min_left(r,pred) == left);
  }
}

int main() {
  constructors<LazySegtreeAddSum<int,ll>>();
  constructors<LazySegtreeAddMin<int>>();
  constructors<LazySegtreeAddMax<int>>();
  constructors<LazySegtreeAddMinCount<int>>();
  constructors<LazySegtreeAddMaxCount<int>>();
  constructors<LazySegtreeAssignSum<int,ll>>();
  constructors<LazySegtreeAssignMin<int>>();
  constructors<LazySegtreeAssignMax<int>>();
  constructors<LazySegtreeAffineSum<modint998244353>>();
  constructors<LazySegtreeXorXor<unsigned>>();
  // Its size-constructor's fill value must be binary.
  LazySegtreeFlipInversion<> binary(std::vector<bool>{true,false});
  assert(binary.all_prod().inversion == 1);
  LazySegtreeFlipInversion<> empty, zeros(5), ones(3,1);
  assert(empty.all_prod().zero == 0 && zeros.all_prod().zero == 5 && ones.all_prod().one == 3);
  LazySegtreeFlipInversion<> from_nodes(std::vector<LazySegtreeFlipInversion<>::value_type>{{0,1,0},{1,0,0}});
  assert(from_nodes.all_prod().inversion == 1);
  // Explicit aggregate lengths and weighted counts are preserved.
  LazySegtreeAddSum<int,ll> wide(std::vector<LazySegtreeAddSum<int,ll>::value_type>{{6000000000LL,3}});
  wide.add(0,1,1000000000);
  assert(wide.all_prod().sum == 9000000000LL);
  LazySegtreeAssignSum<int,ll> weighted(std::vector<LazySegtreeAssignSum<int,ll>::value_type>{{0,3}});
  weighted.assign(0,1,2000000000);
  assert(weighted.all_prod().sum == 6000000000LL);
  LazySegtreeAddMinCount<int,ll> counts(std::vector<std::pair<int,ll>>{{1,3},{1,5}});
  assert(counts.all_prod().second == 8);
  LazySegtreeAddMin<int,100> custom_add_min(std::vector<int>{3,1,4});
  LazySegtreeAddMax<int,-100> custom_add_max(std::vector<int>{3,1,4});
  custom_add_min.add(0,2,5);
  custom_add_max.add(1,3,-5);
  assert(custom_add_min.prod(0,3) == 4 && custom_add_min.prod(0,0) == 100);
  assert(custom_add_max.prod(0,3) == 3 && custom_add_max.prod(0,0) == -100);
  LazySegtreeAddMinCount<int,ll,100> custom_min_count(std::vector<int>{2,1,1});
  LazySegtreeAddMaxCount<int,ll,-100> custom_max_count(std::vector<int>{2,2,1});
  assert((custom_min_count.prod(0,3) == std::pair<int,ll>(1,2)));
  assert((custom_min_count.prod(0,0) == std::pair<int,ll>(100,0)));
  assert((custom_max_count.prod(0,3) == std::pair<int,ll>(2,2)));
  assert((custom_max_count.prod(0,0) == std::pair<int,ll>(-100,0)));
  LazySegtreeAssignMin<int,100> custom_assign_min(std::vector<int>{3,1,4});
  LazySegtreeAssignMax<int,-100> custom_assign_max(std::vector<int>{3,1,4});
  custom_assign_min.assign(0,2,8);
  custom_assign_max.assign(1,3,-8);
  assert(custom_assign_min.all_prod() == 4 && custom_assign_min.prod(0,0) == 100);
  assert(custom_assign_max.all_prod() == 3 && custom_assign_max.prod(0,0) == -100);
  for(int n : {0,1,2,3,5,8,17,31,64}) {
    sums<false>(n); sums<true>(n);
    extremes<true,false,false>(n); extremes<false,false,false>(n);
    extremes<true,true,false>(n); extremes<false,true,false>(n);
    extremes<true,false,true>(n); extremes<false,false,true>(n);
    affine<ll>(n); affine<modint998244353>(n);
    xor_and_flip(n); search(n);
  }
  std::cout << "lazy_segtree_template: all tests passed\n";
}
