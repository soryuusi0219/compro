// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_min_plus_convex_arbitrary_smawk.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, class Compare = less<T>>
vector<pair<int,T>> SMAWK(int h, int w, const function<T(long long, long long)>& f, const Compare &comp = Compare()){
  vector<pair<int,T>> res(h);
  function<void(int,vector<int>&)> solve = [&](int k, const vector<int> col){
    int n = h >> k;
    if(!n) return;
    vector<int> c2;
    c2.reserve(n);
    int i = 0;
    for(int j : col){
      while(i > 0 && !comp(f((i<<k)-1, c2.back()), f((i<<k)-1, j))) {
        c2.pop_back();
        --i;
      }
      if(i < n) {
        c2.push_back(j);
        ++i;
      }
    }
    solve(k+1,c2);
    i = 1;
    res[(1<<k)-1] = {col[0],f((1<<k)-1, col[0])};
    for(int j : col){
      if(comp(f((i<<k)-1,j), res[(i<<k)-1].second)) res[(i<<k)-1] = {j,f((i<<k)-1,j)};
      while(i+2 <= n && res[((i+1)<<k)-1].first == j){
        i+=2;
        res[(i<<k)-1] = {j,f((i<<k)-1,j)};
      }
    }
  };
  vector<int> row(h), col(w);
  iota(row.begin(),row.end(),0);
  iota(col.begin(),col.end(),0);
  solve(0,col);
  return res;
}

//c_k = min_{k=i+j}(a_i+b_j), a:convex, b:arbitrary
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conv_arb(const vector<T> a, const vector<T> b, const Compare &comp = Compare()){
  int n = a.size();
  int m = b.size();
  function<T(long long,long long)> f = [&](long long i, long long j){
    if(i-j < 0 || i-j >= n) return numeric_limits<T>::max();
    return b[j]+a[i-j];
  };
  auto res0 = SMAWK(n+m-1,m,f,comp);
  vector<T> res(n+m-1);
  for(int i = 0; i < n+m-1; i++) res[i] = res0[i].second;
  return res;

}

}  // namespace soryuusi
