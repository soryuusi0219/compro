// https://github.com/soryuusi0219/compro/blob/main/library/math/determinant_sparse.hpp
#pragma once
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
T determinant_sparse(int n, vector<tuple<int,int,T>> val){
  vector<vector<T>> mat(n,vector<T>(n,0));
  T ret = 1;
  vector<pair<int,int>> nonzerocnt(n);
  for(int i = 0; i < n; i++) nonzerocnt[i].second = i;
  for(auto [r,c,v] : val) {
    mat[r][c] = v;
    nonzerocnt[r].first++;
  }
  sort(nonzerocnt.begin(),nonzerocnt.end());
  vector<int> p(n); 
  for(int i0 = 0; i0 < n; i0++){
    int i = nonzerocnt[i0].second;
    int j = 0;
    while(j < n && mat[i][j] == 0) j++;
    if(j == n) return 0;
    p[i] = j;
    ret *= mat[i][j];
    T a = T(1)/mat[i][j];
    for(int k = n-1; k >= j; k--) if(mat[i][k] != 0){
      T b = mat[i][k] * a;
      for(int ii0 = i0 + 1; ii0 < n; ii0++){
        int ii = nonzerocnt[ii0].second;
        mat[ii][k] -= mat[ii][j] * b;
      }
    }
  }
  int cnt = 0;
  for(int i = 0; i < n; i++) for(int j = 0; j < i; j++) cnt += (p[i] < p[j]);
  if(cnt & 1) return -ret;
  return ret;
}

}  // namespace soryuusi
