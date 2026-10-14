// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_min_plus_convex_convex.hpp
#pragma once
#include <functional>
#include <vector>

namespace soryuusi {

using namespace std;

//c_k = min_{k=i+j}(a_i+b_j), a:convex, b:convex
template<class T, class Compare = less<T>>
vector<T> min_plus_convolution_conv_conv(const vector<T> a, const vector<T> b, const Compare &comp = Compare()){
  int n = a.size();
  int m = b.size();
  vector<T> c(n+m-1);
  c[0] = a[0] + b[0];
  for(int i = 1, j = 1; i < n || j < m;){
    if(j == m || (i < n && comp(a[i]-a[i-1],b[j]-b[j-1]))){
      c[i+j-1] = a[i] + b[j-1];
      ++i;
    }else{
      c[i+j-1] = a[i-1] + b[j];
      ++j;
    }
  }
  return c;
}

}  // namespace soryuusi
