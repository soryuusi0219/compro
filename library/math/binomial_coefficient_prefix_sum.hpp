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
  auto AL = [&](int l, int r) {c -= factorials.nCk(r,l);};
  auto DL = [&](int l, int r) {c += factorials.nCk(r,l);};
  auto AR = [&](int l, int r) {c += c - factorials.nCk(r,l);};
  auto DR = [&](int l, int r) {c = (c + factorials.nCk(r,l)) * factorials.mint_inv(2);};
  auto SOL = [&](int i) {res[i] = c;};
  mo.run(AL,AR,DL,DR,SOL);
  return res;
};

}  // namespace soryuusi
