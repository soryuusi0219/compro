// https://github.com/soryuusi0219/compro/blob/main/library/math/binomial_coefficient_prefix_sum.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

#include "library/math/modint_factorials.hpp"
#include "library/data_structure/mo.hpp"

namespace soryuusi {

using namespace std;

template<class T>
vector<T> BinomialCoefficientPrfixSum(const vector<pair<int,int>>& v){
  if(!v.size()) return {};
  vector<T> res(v.size());
  int n = 0;
  for(auto [x,y] : v) n = max(n,x);
  Mo mo(n+1,v.size());
  modint_factorials<T> factorials(n);
  for(auto [r,l] : v) mo.add_query(l,r);
  T c = 1;
  int l = 0, r = 0;
  auto C = [&](int n, int k) {return 0 <= k && k <= n ? factorials.nCk(n,k) : T(0);};
  auto AL = [&](int i) {c -= C(r,l); l = i;};
  auto DL = [&](int i) {l = i+1; c += C(r,l);};
  auto AR = [&](int i) {c += c - C(r,l); r = i+1;};
  auto DR = [&](int i) {r = i; c = (c + C(r,l)) * factorials.mint_inv(2);};
  auto SOL = [&](int i) {res[i] = c;};
  mo.run(AL,AR,DL,DR,SOL);
  return res;
};

}  // namespace soryuusi
