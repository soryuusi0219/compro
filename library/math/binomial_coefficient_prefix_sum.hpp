// https://github.com/soryuusi0219/compro/blob/main/library/math/binomial_coefficient_prefix_sum.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "modint_factorials.hpp"
#include "../data_structure/mo.hpp"

namespace soryuusi {

using namespace std;

template<class T>
vector<T> BinomialCoefficientPrfixSum(const vector<pair<int,int>>& v){
  if(v.empty()) return {};
  vector<T> res(v.size());
  int n = 0;
  for(auto [x,y] : v) n = max(n,x);
  const auto inverse = internal::factorial_and_inverse_factorials<T>(n).second;
  vector<T> factorial(n+1,T(1)), powers(n+1,T(1));
  for(int i = 1; i <= n; ++i){
    factorial[i] = factorial[i-1]*T(i);
    powers[i] = powers[i-1]+powers[i-1];
  }
  vector<int> pending;
  vector<bool> reflected(v.size());
  pending.reserve(v.size());
  for(int i = 0; i < (int)v.size(); ++i){
    auto [row,column] = v[i];
    assert(row >= 0);
    if(column < 0) continue;
    if(column >= row){res[i] = powers[row]; continue;}
    reflected[i] = (column > (row-1)/2);
    if(reflected[i]) column = row-1-column;
    if(column <= 8){
      T sum = 0;
      for(int j = 0; j <= column; ++j) sum += inverse[j]*inverse[row-j];
      sum *= factorial[row];
      res[i] = reflected[i] ? powers[row]-sum : sum;
    }else{
      pending.push_back(i);
    }
  }
  if(pending.empty()) return res;
  Mo mo(n+1,pending.size());
  for(int i : pending){
    const auto [row,column] = v[i];
    mo.add_query(reflected[i] ? row-1-column : column,row);
  }
  const T half = T(2).inv();
  T c = 1;
  int l = 0, r = 0;
  auto C = [&](int row, int column) {return column <= row ? factorial[row]*inverse[column]*inverse[row-column] : T(0);};
  auto AL = [&](int i) {c -= C(r,l); l = i;};
  auto DL = [&](int i) {l = i+1; c += C(r,l);};
  auto AR = [&](int i) {c += c - C(r,l); r = i+1;};
  auto DR = [&](int i) {r = i; c = (c + C(r,l)) * half;};
  auto SOL = [&](int i) {
    const int id = pending[i];
    res[id] = reflected[id] ? powers[r]-c : c;
  };
  mo.run(AL,AR,DL,DR,SOL);
  return res;
};

}  // namespace soryuusi
