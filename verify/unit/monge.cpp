#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "library/utility/monotone_minima_dp.hpp"
#include "library/utility/smawk_dp.hpp"

int main() {
  for(int h : {0,1,2,3,7,16,31,69}) for(int w : {1,2,3,7,16,31,73}){
    auto value = [](int i,int j){return std::max(0,std::abs(i-j)-4);};
    auto negated = [&](int i,int j){return -value(i,j);};
    auto minima = soryuusi::monotoneminima<int>(h,w,value);
    auto linear = soryuusi::SMAWK<int>(h,w,value);
    auto maxima = soryuusi::SMAWK<int>(h,w,negated,std::greater<int>());
    assert(minima == linear);
    for(int i = 0; i < h; ++i){
      int best = 0;
      for(int j = 1; j < w; ++j) if(value(i,j) < value(i,best)) best = j;
      assert((minima[i] == std::pair{best,value(i,best)}));
      assert((maxima[i] == std::pair{best,-value(i,best)}));
    }
  }
  for(int n : {0,1,2,50,129}){
    auto cost = [](int i,int j){return (j-i)*(j-i)-3;};
    auto result_monotoneminima = soryuusi::monotoneminima_dp<int>(n,cost);
    auto result_smawk = soryuusi::SMAWK_dp<int>(n,cost);
    std::vector<int> expected(n+1);
    for(int r = 0; r <= n; ++r){
      expected[r] = cost(0,r);
      for(int l = 0; l < r; ++l) expected[r] = std::min(expected[r],expected[l]+cost(l,r));
    }
    assert(result_monotoneminima == expected);
    assert(result_smawk == expected);
  }
  std::cout << "monge: all tests passed\n";
}
