// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_min_plus_convex_arbitrary_monotone_minima.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <limits>
#include <vector>

namespace soryuusi {

using namespace std;

//c_k = min_{k=i+j}(a_i+b_j), a:convex, b:arbitrary
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conv_arb(const vector<T> a, const vector<T> b, const Compare &comp = Compare(), T INF = numeric_limits<T>::max()){
  int n = a.size();
  int m = b.size();
  int h = n+m-1;
  vector<T> c(n+m-1,INF);
  vector<int> id(n+m-1,0);
  int k = 1; while(k < h+1) k <<= 1;
  for(k >>= 1; k; k >>= 1){
    for(int i = k-1, j = max(0,k-n); j < m; ++j){
      if(i >= j && comp(a[i-j]+b[j],c[i])) c[i] = a[i-j]+b[j], id[i] = j;
      if(i + k < h && id[i+k] == j){
        if((i += (k << 1)) >= h) break;
        j = max(j-1,i-n);
      }
    }
  }
  return c;
}

}  // namespace soryuusi
