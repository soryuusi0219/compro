// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_min_plus_concave_arbitrary.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <functional>
#include <limits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

//c_k = min_{k=i+j}(a_i+b_j), a:concave, b:arbitrary
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conc_arb(const vector<T> A, const vector<T> B, const Compare &comp = Compare(), T INF = numeric_limits<T>::max()){
  int n = A.size();
  int m = B.size();
  int h = n+m-1;
  vector<T> c(h,INF);
  if(n == 1){
    for(int i = 0; i < m; ++i) c[i] = A[0] + B[i];
    return c;
  }
  if(m == 1){
    for(int i = 0; i < n; ++i) c[i] = A[i] + B[0];
    return c;
  }
  int k = bit_floor((unsigned)h) << 1;
  int mn = k >> (bit_width((unsigned)(n-1)));
  vector<int> seg(k,-1);
  for(int l = 0; l < m; ++l){
    int r = l + n;
    r &= -bit_floor((unsigned)(l^r));
    int s = bit_width((unsigned)(r-l-1));
    int i = (k + l) >> s;
    int f = l;
    while(s && ~f){
      int m = ((i << 1 | 1) << --s) - k - 1;
      if(m < l) i = i << 1 | 1;
      else if(~seg[i] && comp(B[seg[i]] + A[m-seg[i]],B[f] + A[m-f])) i = i << 1;
      else{
        swap(seg[i],f);
        i = i << 1 | 1; 
      }
    }
    if(~f && comp(B[f] + A[i-f-k], c[i-k])) c[i-k] = B[f] + A[i-f-k];
    for(int j = (l + k) >> 1; j >= mn; j >>= 1) if(~seg[j] && comp(B[seg[j]] + A[l-seg[j]], c[l])) c[l] = B[seg[j]] + A[l-seg[j]];
  }
  for(int i = m; i < h; ++i) for(int j = (i + k) >> 1; j >= mn; j >>= 1) if(~seg[j] && comp(B[seg[j]] + A[i-seg[j]], c[i])) c[i] = B[seg[j]] + A[i-seg[j]];
  fill(seg.begin(), seg.end(),-1);
  for(int r = h; r >= n;){
    int f = r - n;
    int l = r & -bit_floor((unsigned)(f^r));
    if(l != r){
      int s = bit_width((unsigned)(r-l-1));
      int i = (k + l) >> s;
      while(s && ~f){
        int m = ((i << 1 | 1) << --s) - k;
        if(m >= r) i = i << 1;
        else if(~seg[i] && comp(B[seg[i]] + A[m-seg[i]], B[f] + A[m-f])) i = i << 1 | 1;
        else{
          swap(seg[i],f);
          i = i << 1; 
        }
      }
      if(~f && comp(B[f] + A[i-f-k], c[i-k])) c[i-k] = B[f] + A[i-f-k];
    }
    --r;
    for(int j = (r + k) >> 1; j >= mn; j >>= 1) if(~seg[j] && comp(B[seg[j]] + A[r-seg[j]], c[r])) c[r] = B[seg[j]] + A[r-seg[j]];
  }
  for(int i = 0; i < n; ++i) for(int j = (i + k) >> 1; j >= mn; j >>= 1) if(~seg[j] && comp(B[seg[j]] + A[i-seg[j]], c[i])) c[i] = B[seg[j]] + A[i-seg[j]];
  return c;
}

}  // namespace soryuusi
