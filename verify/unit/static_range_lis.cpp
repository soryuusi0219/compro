#include "library/data_structure/static_range_lis.hpp"
#include "library/data_structure/static_range_lis.hpp"

#include <cassert>
#include <functional>
#include <numeric>
#include <random>
#include <vector>

template<class T, class Compare = std::less<T>>
void check(const std::vector<T>& values) {
  soryuusi::StaticRangeLIS<T,Compare> lis(values);
  Compare comp;
  int n = values.size();
  for(int l = 0; l <= n; ++l) {
    std::vector<int> dp(n);
    int expected = 0;
    assert(lis.query(l,l) == 0);
    for(int r = l; r < n; ++r) {
      dp[r] = 1;
      for(int i = l; i < r; ++i) {
        if(!comp(values[r],values[i])) dp[r] = std::max(dp[r],dp[i]+1);
      }
      expected = std::max(expected,dp[r]);
      assert(lis.query(l,r+1) == expected);
    }
  }
}

struct Value {
  int key;
};
struct CompareValue {
  bool operator()(const Value& a, const Value& b) const {return a.key/2 < b.key/2;}
};

int main() {
  check<int>({});
  check<int>({5});
  check<int>({1,1});
  check<int>({3,1,4,1,5,9,2,6,5,3});
  check<Value,CompareValue>({{3},{2},{5},{0},{4}});
  for(int n : {2,3,7,16,31,63,64,65,127,128,129}) {
    std::vector<int> values(n);
    std::iota(values.begin(),values.end(),0);
    check(values);
    std::reverse(values.begin(),values.end());
    check(values);
    std::fill(values.begin(),values.end(),-7);
    check(values);
  }
  std::mt19937 rng(20261114);
  for(int testcase = 0; testcase < 250; ++testcase) {
    int n = rng()%50;
    std::vector<int> values(n);
    std::iota(values.begin(),values.end(),0);
    std::shuffle(values.begin(),values.end(),rng);
    check(values);
    check<int,std::greater<int>>(values);
    for(auto& x : values) x = int(rng()%11)-5;
    check(values);
    check<int,std::greater<int>>(values);
  }
  // Class template argument deduction remains available.
  soryuusi::StaticRangeLIS lis(std::vector<int>{2,1,3});
  assert(lis.query(0,3) == 2);
}
