#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <source_location>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "library/data_structure/dsu.hpp"
#include "library/data_structure/dsu_undo.hpp"
#include "library/data_structure/dsu_undo_with_data.hpp"
#include "library/data_structure/dynamic_dsu.hpp"
#include "library/data_structure/hash_table.hpp"
#include "library/data_structure/range_parallel_unionfind.hpp"
#include "library/data_structure/weighted_dsu.hpp"

namespace {

void check(bool ok, std::source_location loc = std::source_location::current()) {
  if(ok) return;
  std::cerr << loc.file_name() << ':' << loc.line() << ": check failed\n";
  std::abort();
}

std::uint64_t constant_hash(int) { return 0; }
std::uint64_t high_hash(int x) { return static_cast<std::uint64_t>(x) << 48; }

void test_hash() {
  using Table = soryuusi::HashTable<int,int,constant_hash>;
  Table empty(0);
  check(!empty.contains(4) && empty.get(4,19) == 19);

  Table table(2);
  auto& ref = table[4];
  ref = 9;
  table[11] = 23;
  check(table.get(4,-1) == 9 && table.get(11,-1) == 23);
  table[8] = 17;
  table[9] = 21;
  check(!table.contains(25) && table.get(25,31) == 31);
  check(&ref == &table[4] && table[8] == 17);
  bool full = false;
  try {
    table[25] = 0;
  } catch(const std::length_error&) {
    full = true;
  }
  check(full && table.get(9,-1) == 21);

  soryuusi::HashTable<int,std::string,high_hash> biased(64);
  for(int i = 0; i < 96; ++i) biased[i] += std::to_string(i);
  for(int i = 95; i >= 0; --i) check(biased.get(i,"?") == std::to_string(i));
  check(biased.get(-1,"?") == "?");
}

void test_dynamic() {
  soryuusi::dynamic_dsu singleton(0);
  check(singleton.size(INT_MIN) == 1 && singleton.leader(INT_MIN) == INT_MIN);

  soryuusi::dynamic_dsu sparse(4);
  check(sparse.merge(-17,INT_MAX) == -17);
  check(sparse.merge(INT_MIN,22) == INT_MIN);
  check(sparse.merge(INT_MIN,INT_MAX) == INT_MIN);
  check(sparse.leader(INT_MAX) == INT_MIN && sparse.size(-17) == 4);
  check(sparse.same(22,-17));

  constexpr int n = 128;
  soryuusi::dynamic_dsu d(n);
  soryuusi::DSU dense(n);
  std::mt19937 rng(2727);
  auto key = [](int i) { return i*555555-30000000; };
  for(int i = 0; i < 3000; ++i) {
    int a = rng()%n, b = rng()%n;
    if(i%4 == 0) check(d.merge(key(a),key(b)) == key(dense.merge(a,b)));
    else if(i%4 == 1) check(d.same(key(a),key(b)) == dense.same(a,b));
    else if(i%4 == 2) check(d.leader(key(a)) == key(dense.leader(a)));
    else check(d.size(key(a)) == dense.size(a));
  }
}

using Perm = std::array<int,3>;
Perm compose(Perm a, Perm b) { return {a[b[0]],a[b[1]],a[b[2]]}; }
Perm identity() { return {0,1,2}; }
Perm inverse(Perm a) {
  Perm b;
  for(int i = 0; i < 3; ++i) b[a[i]] = i;
  return b;
}

void test_weighted() {
  constexpr std::array<Perm,8> p = {{{0,1,2},{1,0,2},{1,2,0},{0,2,1},{2,0,1},{2,1,0},{1,2,0},{0,2,1}}};
  auto delta = [&](int i,int j){ return compose(p[j],inverse(p[i])); };
  soryuusi::Weighted_dsu<Perm,compose,identity,inverse> d(p.size());
  for(int width = 1; width < (int)p.size(); width *= 2) {
    for(int i = 0; i < (int)p.size(); i += width*2) {
      check(d.merge(i+width,i,delta(i+width,i)) == i+2*width-1);
    }
  }
  // Depth three before any compression. This query must also work with NDEBUG.
  check(d.weight(7,0) == delta(7,0));
  for(int i = 0; i < (int)p.size(); ++i) {
    for(int j = 0; j < (int)p.size(); ++j) {
      check(d.weight(i,j) == delta(i,j));
      check(d.merge(i,j,delta(i,j)) == 7);
    }
  }
  check(d.merge(0,1,identity()) == -1 && d.size(0) == (int)p.size());

  soryuusi::Weighted_dsu_bool parity(3);
  parity.merge(0,1,true);
  parity.merge(2,1,false);
  check(parity.weight(0,2));
}

std::string concat(const std::string& a, const std::string& b) {
  return a+b;
}

void test_undo() {
  soryuusi::dsu_undo d(8);
  for(int width = 1; width < 8; width *= 2) {
    int state = d.get_state();
    for(int i = 0; i < 8; i += width*2) check(d.merge(i,i+width) == i);
    check(d.size(7) == width*2);
    check(d.merge(0,width) == -1);
    check(d.get_state() == state+8/(2*width));
  }
  check(d.groups() == std::vector<std::vector<long long>>{{0,1,2,3,4,5,6,7}});
  d.undo();
  check(d.leader(7) == 4);
  d.rollback(0);
  check(d.get_state() == 0 && d.size(7) == 1 && !d.same(0,7));

  soryuusi::dsu_undo_data<std::string,concat> data(std::vector<std::string>{"A","B","C","D"});
  check(data.merge(1,2) == 1);
  data.snapshot();
  check(data.merge(0,1) == 1);
  data.apply(2,"E");
  check(data.get(0) == "BCAE" && data.get_state() == 3);
  data.undo();
  check(data.get(2) == "BCA");
  check(data.groups() == std::vector<std::vector<long long>>{{0,1,2},{3}});
  data.rollback();
  check(data.get(0) == "A" && data.get(2) == "BC");
  data.rollback(0);
  check(data.get(1) == "B" && data.get(2) == "C");
}

void test_range_unions() {
  soryuusi::Range_Parallel_Unionfind empty(0);
  empty.merge(0,0,0);
  check(empty.groups().empty());

  for(int n : {1,2,7,8,9,31,32,33,128}) {
    soryuusi::Range_Parallel_Unionfind d(n);
    soryuusi::DSU expected(n);
    std::mt19937 rng(921+n);
    for(int t = 0; t < 300; ++t) {
      int l = rng()%(n+1), r = rng()%(n+1);
      int len = rng()%(n-std::max(l,r)+1);
      std::vector<std::tuple<int,int,int>> want, got;
      for(int i = 0; i < len; ++i) {
        int a = expected.leader(l+i), b = expected.leader(r+i);
        if(a != b) want.emplace_back(a,b,expected.merge(a,b));
      }
      if(t%3) {
        d.merge(l,r,len,[&](int a,int b,int root){ got.emplace_back(a,b,root); });
        check(got == want);
      } else {
        d.merge(l,r,len);
      }
      check(d.groups() == expected.groups());
    }
  }
}

}  // namespace

int main() {
  test_hash();
  test_dynamic();
  test_weighted();
  test_undo();
  test_range_unions();
  std::cout << "unionfind_hash: all tests passed\n";
}
