#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "library/data_structure/binary_trie.hpp"
#include "library/data_structure/dynamic_lazy_segtree.hpp"
#include "library/data_structure/dynamic_segtree.hpp"
#include "library/data_structure/dynamic_segtree_2d.hpp"
#include "library/data_structure/hash_table.hpp"
#include "library/data_structure/internal/node_pool.hpp"
#include "library/data_structure/li_chao_tree_dynamic.hpp"
#include "library/data_structure/li_chao_tree_static.hpp"
#include "library/data_structure/point_set_range_count.hpp"

namespace {
using LL = long long;
LL plus_value(LL a, LL b) { return a+b; }
LL zero() { return 0; }
std::string join(const std::string& a, const std::string& b) { return a+b; }
std::string empty_string() { return {}; }

struct alignas(64) Tracked {
  inline static int alive = 0;
  int value;
  explicit Tracked(int v) : value(v) {
    if(v < 0) throw std::runtime_error("constructor failed");
    ++alive;
  }
  ~Tracked() { --alive; }
};

void test_pool() {
  using Pool = soryuusi::internal::NodePool<Tracked>;
  {
    Pool pool;
    auto* first = pool.create(19);
    auto root = pool.root(first);
    assert(reinterpret_cast<std::uintptr_t>(first)%alignof(Tracked) == 0);
    for(int i = 1; i < 8; ++i) pool.create(i);
    try {
      pool.create(-1);
      assert(false);
    } catch(const std::runtime_error&) {}
    assert(pool.size() == 8 && Tracked::alive == 8);
    for(int i = 8; i < 5200; ++i) pool.create(i);
    assert(pool.get(root) == first && first->value == 19);
    Pool moved(std::move(pool));
    assert(pool.size() == 0 && moved.get(root) == first);
    pool.create(5);
    Pool assigned;
    assigned.create(12);
    assigned = std::move(moved);
    assert(moved.size() == 0 && assigned.size() == 5200 && Tracked::alive == 5201);
    assert(assigned.get(root) == first && pool.size() == 1);
  }
  assert(Tracked::alive == 0);
}

void test_dynamic_segtree() {
  soryuusi::Dynamic_Segtree<LL,plus_value,zero> huge(INT_MAX);
  huge.set(INT_MAX-1,8);
  huge.set(INT_MAX-23,3);
  assert(huge.get(INT_MAX-1) == 8 && huge.prod(INT_MAX-23,INT_MAX) == 11);

  constexpr int n = 65;
  soryuusi::Dynamic_Segtree<LL,plus_value,zero> tree(n);
  soryuusi::Dynamic_Segtree<std::string,join,empty_string> noncomm(n);
  std::vector<int> a(n);
  std::vector<std::string> text(n);
  std::mt19937 rng(517979);
  for(int j = 0; j < 700; ++j) {
    int i = rng()%n, l = rng()%(n+1), r = rng()%(n+1);
    if(l > r) std::swap(l,r);
    a[i] = rng()%17;
    text[i] = std::to_string(i)+",";
    tree.set(i,a[i]);
    noncomm.set(i,text[i]);
    const auto allocated = tree.allocated_nodes();
    assert(tree.prod(l,r) == std::accumulate(a.begin()+l,a.begin()+r,0LL));
    const auto selected = std::accumulate(text.begin()+l,text.begin()+r,std::string{});
    assert(noncomm.prod(l,r) == selected);
    auto prefix = [&](const std::string& s){ return selected.starts_with(s); };
    auto suffix = [&](const std::string& s){ return selected.ends_with(s); };
    int start = l, end = r;
    while(start > 0 && text[start-1].empty()) --start;
    while(end < n && text[end].empty()) ++end;
    assert(noncomm.max_right(l,prefix) == end);
    assert(noncomm.min_left(r,suffix) == start);
    int limit = rng()%150;
    auto fits = [&](LL s){ return s <= limit; };
    int right = l, left = r, sum = 0;
    while(right < n && sum+a[right] <= limit) sum += a[right++];
    sum = 0;
    while(left > 0 && sum+a[left-1] <= limit) sum += a[--left];
    assert(tree.max_right(l,fits) == right && tree.min_left(r,fits) == left);
    assert(tree.allocated_nodes() == allocated);
  }
}

void test_2d() {
  constexpr int n = INT_MAX;
  const std::vector<int> coords{0,1,59,n/2-2,n/2,n-12,n-2,n-1};
  int k = coords.size();
  std::vector table(k,std::vector<LL>(k));
  soryuusi::Dynamic_Segtree2D<LL,plus_value,zero> a(n,n);
  std::mt19937 rng(875);
  for(int t = 0; t < 1200; ++t) {
    int x = rng()%k, y = rng()%k, v = int(rng()%103)-50;
    if(t%2) {
      a.set(coords[x],coords[y],v);
      table[x][y] = v;
    } else {
      a.add(coords[x],coords[y],v);
      table[x][y] += v;
    }
    assert(a.get(coords[x],coords[y]) == table[x][y]);
    auto count = a.allocated_nodes();
    int l = rng()%k, r = rng()%k, d = rng()%k, u = rng()%k;
    if(l > r) std::swap(l,r);
    if(d > u) std::swap(d,u);
    LL expected = 0;
    for(int i = l; i < r; ++i) for(int j = d; j < u; ++j) expected += table[i][j];
    assert(a.prod(coords[l],coords[r],coords[d],coords[u]) == expected);
    assert(a.allocated_nodes() == count);
  }
  auto clone = a.clone();
  LL total = clone.all_prod();
  a.set(n-1,n-1,a.get(n-1,n-1)+12);
  assert(clone.all_prod() == total && a.all_prod() == total+12);
}

struct Sum { LL value; int size; };
struct Affine { LL a,b; };
Sum add_sums(Sum a, Sum b) { return {a.value+b.value,a.size+b.size}; }
Sum empty() { return {0,0}; }
Sum mapping(Affine f, Sum a) { return {f.a*a.value+f.b*a.size,a.size}; }
Affine compose(Affine f, Affine g) { return {f.a*g.a,f.a*g.b+f.b}; }
Affine identity() { return {1,0}; }

void test_dynamic_lazy() {
  using Tree = soryuusi::Dynamic_LazySegtree<Sum,add_sums,empty,Affine,mapping,compose,identity>;
  auto init = [](int l,int r){ return Sum{2LL*(r-l),r-l}; };
  Tree huge(INT_MAX,init);
  huge.apply(0,INT_MAX,{1,3});
  huge.set(INT_MAX-1,{7,1});
  auto count = huge.allocated_nodes();
  huge.apply(INT_MAX-7,INT_MAX-7,{8,5});
  assert(huge.allocated_nodes() == count && huge.prod(INT_MAX-7,INT_MAX-7).value == 0);
  assert(huge.prod(INT_MAX-3,INT_MAX).value == 17);
  assert(huge.allocated_nodes() == count);

  constexpr int n = 101;
  Tree ds(n,init);
  std::vector<LL> model(n,2);
  std::mt19937 rng(90920);
  for(int t = 0; t < 1000; ++t) {
    int l = rng()%(n+1), r = rng()%(n+1), i = rng()%n, v = int(rng()%43)-21;
    if(l > r) std::swap(l,r);
    if(t%13 == 0) l = 0, r = n;
    if(t%3 == 0){
      ds.set(i,{v,1});
      model[i] = v;
    } else {
      Affine f{int(rng()%3)-1,v};
      ds.apply(l,r,f);
      for(int j = l; j < r; ++j) model[j] = f.a*model[j]+f.b;
    }
    int left = rng()%(n+1), right = rng()%(n+1);
    if(left > right) std::swap(left,right);
    auto size = ds.allocated_nodes();
    const Tree& read = ds;
    assert(read.get(i).value == model[i]);
    assert(read.prod(left,right).value == std::accumulate(model.begin()+left,model.begin()+right,0LL));
    assert(ds.allocated_nodes() == size);
  }
}

void test_trie() {
  using Trie = soryuusi::Binary_Trie<unsigned,10>;
  Trie a, b;
  std::set<unsigned> x, y;
  std::mt19937 rng(2804);
  for(int t = 0; t < 1000; ++t) {
    auto& ds = t%2 ? a : b;
    auto& values = t%2 ? x : y;
    unsigned v = rng()%512;
    if(t%5 < 3){
      ds.insert(v);
      ds.insert(v);
      values.insert(v);
    } else if(t%5 == 3) {
      ds.erase(v);
      values.erase(v);
    } else {
      ds.operate_xor(v);
      std::set<unsigned> next;
      for(unsigned item : values) next.insert(item^v);
      values.swap(next);
    }
    auto size = ds.allocated_nodes();
    if(!values.empty()) assert(ds.min_element() == *values.begin() && ds.max_element() == *values.rbegin());
    assert(ds.allocated_nodes() == size && ds.size() == int(values.size()));
  }
  a.merge(std::move(b));
  x.insert(y.begin(),y.end());
  int k = 0;
  for(unsigned v : x) assert(a.kth(k++) == v);
  assert(b.size() == 0 && !x.empty());
  Trie empty;
  empty.merge(std::move(a));
  assert(a.size() == 0 && empty.size() == int(x.size()));
  b.insert(37);
  a.insert(81);
  assert(b.min_element() == 37 && a.min_element() == 81);
}

void test_li_chao() {
  using Line = soryuusi::Line<LL>;
  constexpr LL inf = 1LL<<60;
  using Dynamic = soryuusi::DynamicLiChaoTree<LL,-48,49,1,inf>;
  Dynamic d;
  std::vector<LL> xs;
  for(int i = -48; i < 49; ++i) if(i%3) xs.push_back(i);
  std::reverse(xs.begin(),xs.end());
  xs.push_back(4);
  soryuusi::StaticLiChaoTree<LL,inf> s(xs);
  soryuusi::StaticLiChaoTree<LL,inf> empty_x(std::vector<LL>{});
  empty_x.add_Line({1,2});
  assert(empty_x.get_min(0) == inf);
  std::vector<LL> best(97,inf);
  std::mt19937 rng(75486);
  for(int t = 0; t < 300; ++t) {
    int l = int(rng()%97)-48, r = int(rng()%98)-48;
    if(l > r) std::swap(l,r);
    Line f{int(rng()%63)-31,int(rng()%203)-101};
    if(t%13 == 0){
      s.add_Line(f);
      d.add_Line(f);
      l = -48;
      r = 49;
    } else {
      s.add_Segment(f,l,r);
      d.add_Segment(f,l,r);
    }
    for(int i = l; i < r; ++i) best[i+48] = std::min(best[i+48],f.get(i));
    auto nodes = d.allocated_nodes();
    for(int i = -48; i < 49; ++i) assert(d.get_min(i) == best[i+48]);
    for(LL i : xs) assert(s.get_min(i) == best[i+48]);
    assert(d.allocated_nodes() == nodes);
  }
  constexpr LL lo = std::numeric_limits<LL>::min(), hi = std::numeric_limits<LL>::max();
  soryuusi::DynamicLiChaoTree<LL,lo,hi,1,inf> huge;
  huge.add_Line({0,10});
  huge.add_Segment({0,7},hi-8,hi);
  huge.add_Segment({0,-3},lo,lo+4);
  assert(huge.get_min(lo) == -3 && huge.get_min(0) == 10 && huge.get_min(hi-1) == 7);
}

std::uint64_t constant_hash(int) { return 0; }

void test_count() {
  using Count = soryuusi::PointSetRangeCount<int,constant_hash>;
  Count empty(9,0);
  assert(empty.count(0,9,8) == 0);
  Count limited(10,2);
  limited.set(3,5);
  limited.set(7,6);
  try {
    limited.set(3,7);
    assert(false);
  } catch(const std::length_error&) {}
  assert(limited.count(0,10,5) == 1 && limited.count(0,10,6) == 1 && limited.count(0,10,7) == 0);

  constexpr int n = 41, m = 17;
  Count ds(n,m);
  std::vector<int> model(n,-1);
  std::mt19937 rng(48151);
  for(int t = 0; t < 1500; ++t) {
    int i = rng()%n, x = rng()%m;
    ds.set(i,x);
    auto count = ds.allocated_nodes();
    ds.set(i,x);
    model[i] = x;
    int l = rng()%(n+1), r = rng()%(n+1), key = rng()%(m+2);
    if(l > r) std::swap(l,r);
    assert(ds.count(l,r,key) == std::count(model.begin()+l,model.begin()+r,key));
    assert(ds.allocated_nodes() == count);
  }
}

}  // namespace

int main() {
  test_pool();
  test_dynamic_segtree();
  test_2d();
  test_dynamic_lazy();
  test_trie();
  test_li_chao();
  test_count();
  std::cout << "sparse_structures: all tests passed\n";
}
