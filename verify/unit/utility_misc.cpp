#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "library/utility/64_ary_tree.hpp"
#include "library/utility/compress.hpp"
#include "library/utility/rand.hpp"
#include "library/utility/time.hpp"

namespace {

void test_bounded_set(){
  soryuusi::BitSetSet<int> empty;
  assert(empty.next(0) == 0 && empty.prev(0) == -1);
  std::mt19937 rng(9184);
  for(int n : {0,1,63,64,65,4095,4096,4097,262145}){
    soryuusi::BitSetSet<int> set(n);
    std::set<int> expected;
    if(n) set.insert(n-1), expected.insert(n-1);
    for(int i = 0; i < 2000; ++i){
      if(n){
        int x = rng()%n;
        if(i%3 == 0) set.insert(x), expected.insert(x);
        else if(i%3 == 1) set.erase(x), expected.erase(x);
        assert(set.contains(x) == expected.contains(x));
      }
      int x = static_cast<int>(rng()%(n+3))-1;
      auto it = expected.lower_bound(x);
      assert(set.next(x) == (it == expected.end() ? n : *it));
      assert(set.prev(x) == (it == expected.begin() ? -1 : *std::prev(it)));
    }
    if(n){
      set.erase(n-1);
      set.erase(n-1);
      assert(set.next(n-1) == n);
    }
  }
  soryuusi::BitSetSet<unsigned> unsigned_set(65);
  unsigned_set.insert(64);
  assert(unsigned_set.next(63) == 64 && unsigned_set.prev(66) == 64);
  assert(unsigned_set.prev(0) == std::numeric_limits<unsigned>::max());
}

template<class T>
void check_compression(const std::vector<T>& v){
  auto [index,key] = soryuusi::compress(v);
  auto sorted = v;
  std::sort(sorted.begin(),sorted.end());
  sorted.erase(std::unique(sorted.begin(),sorted.end()),sorted.end());
  assert(index.size() == v.size() && key == sorted);
  for(std::size_t i = 0; i < v.size(); ++i)
    assert(index[i] == std::lower_bound(key.begin(),key.end(),v[i])-key.begin());
}

void test_compression(){
  std::mt19937 rng(1561);
  check_compression(std::vector<int>{});
  check_compression(std::vector<std::string>{"az","z","az","aa"});
  for(int n : {4096,4097,17000}){
    std::vector<int> v(n);
    for(int& x : v) x = static_cast<int>(rng());
    v[0] = std::numeric_limits<int>::min();
    v[1] = std::numeric_limits<int>::max();
    check_compression(v);
    std::sort(v.begin(),v.end());
    check_compression(v);
    std::vector<unsigned int> small(n);
    for(auto& x : small) x = rng()%16;
    check_compression(small);
  }
}

}  // namespace

int main() {
  test_bounded_set();
  test_compression();
  soryuusi::random_engine.seed(123456789);
  for(int i = 0; i < 1000; ++i) {
    int x = soryuusi::random_integer(-4,7);
    assert(-4 <= x && x <= 7);
  }
  assert(soryuusi::random_integer(5,5) == 5);

  std::ostringstream output;
  auto* old = std::cout.rdbuf(output.rdbuf());
  soryuusi::tick();
  soryuusi::tick();
  std::cout.rdbuf(old);
  const std::string text = output.str();
  assert(std::count(text.begin(),text.end(),'\n') == 2);
  assert(text.find("ms") != std::string::npos);

  std::cout << "utility_misc: all tests passed\n";
}
