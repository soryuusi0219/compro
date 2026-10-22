// https://github.com/soryuusi0219/compro/blob/main/library/math/number_of_increasing_sequence.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

#include "library/math/convolution.hpp"

#include "library/math/modint_factorials.hpp"

namespace soryuusi {

using namespace std;

template<class mint>
mint num_inc_seq_under(long long n, long long m, vector<long long> a){
  modint_factorials<mint> factorials;
  if(!n) return 1;
  vector<mint> v(n,0), w(m,0);
  v[0] = 1;
  for(int i = 0; i < n; i++){
    int k = (1<<__countr_zero(i+1));
    int x0 = i;
    int x1 = min(i+k,(int)n);
    int y0 = (k > i ? 0 : a[i-k]);
    int y1 = a[i];
    int x = x1-x0;
    int y = y1-y0;
    vector<mint> v0(x), w0(y), cv(x), cw(y);
    for(int j = 0; j < x; j++) v0[j] = factorials.mint_factorial(x-j-1) * v[x1-j-1];
    for(int j = 0; j < y; j++) w0[j] = factorials.mint_factorial(y-j-1) * w[y1-j-1];
    for(int j = 0; j < x; j++) cv[j] = factorials.mint_inv_factorial(j);
    for(int j = 0; j < y; j++) cw[j] = factorials.mint_inv_factorial(j);
    v0 = convolution(v0,cv);
    w0 = convolution(w0,cw);
    vector<mint> v1, w1;
    v1.insert(v1.begin(),v.begin()+x0,v.begin()+x1);
    w1.insert(w1.begin(),w.begin()+y0,w.begin()+y1);
    for(int j = 0; j < min(x,y); j++) v1[j] += w0[y-1-j] * factorials.mint_inv_factorial(j) * (j&1?-1:1);
    for(int j = 0; j < min(x,y); j++) w1[j] += v0[x-1-j] * factorials.mint_inv_factorial(j) * (j&1?-1:1);
    for(int j = 0; j < x; j++) cv[j] = factorials.template nCk<true>(y+j-1,j);
    for(int j = 0; j < y; j++) cw[j] = factorials.template nCk<true>(x+j-1,j);
    v1 = convolution(v1,cv);
    w1 = convolution(w1,cw);
    for(int j = 0; j < x; j++) v[j+x0] = v1[j];
    for(int j = 0; j < y; j++) w[j+y0] = w1[j];
  }
  mint ans = 0;
  for(int i = 0; i < m; i++) ans += w[i];
  return ans;
}

}  // namespace soryuusi
