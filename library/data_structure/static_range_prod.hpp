// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_range_prod.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, S (*op)(S, S), S (*e)()>
struct StaticRangeProd{
  static constexpr int b = 3;
  static constexpr int B = 1 << b;
  int n, m, h;
  vector<S> data;
  vector<S> left_prod;
  vector<S> right_prod;
  vector<vector<S>> sparse_table;
  StaticRangeProd(vector<S> v){
    data = v;
    n = data.size();
    if(n <= B) return;
    m = ((n-1) >> b) + 1;
    left_prod = vector<S>(n+1,e());
    right_prod = vector<S>(n+1,e());
    h = 31-__builtin_clz(m);
    sparse_table = vector<vector<S>>(h+1, vector<S>(m+1,e()));
    S temp = e();
    for(int i = 0; i < n; i++){
      left_prod[i] = temp;
      temp = op(temp, data[i]);
      if(!(~i&(B-1))) {
        sparse_table[0][i >> b] = temp;
        temp = e();
      }
    }
    left_prod[n] = temp;
    if(n&(B-1)){
      sparse_table[0][m-1] = temp;
      temp = e();
    }
    for(int i = (n&~(B-1))-1; i >= 0; i--){
      temp = op(data[i], temp);
      right_prod[i] = temp;
      if(!(i&(B-1))) {
        temp = e();
      }
    }
    sparse_table[0][m] = sparse_table[0][m-1];
    for(int h0 = 1; h0 <= h; h0++){
      int i = 1 << h0;
      for(int j = i; j <= m; j += i << 1){
        temp = e();
        for(int k = j-1; k >= j - i; k--){
          temp = op(sparse_table[0][k], temp);
          sparse_table[h0][k] = temp;
        }
        temp = e();
        for(int k = j; k < min(j + i,m+1); k++){
          sparse_table[h0][k] = temp;
          temp = op(temp, sparse_table[0][k]);
        }
      }
    }
    for(int i = 1; i <= m; i += 2) sparse_table[0][i] = e();
  }
  S naive_prod(int l, int r){
    S res = e();
    for(int i = l; i < r; i++) res = op(res,data[i]);
    return res;
  }
  S prod(int l, int r){
    if(l >= r) return e();
    if(r-l <= 8) return naive_prod(l,r);
    int l1 = (l >> b) + 1;
    int r1 = r >> b;
    if(l1 > r1) return naive_prod(l,r);
    if(l1 == r1) return op(right_prod[l], left_prod[r]);
    int mid = 31-__builtin_clz(l1^r1);
    return op(op(right_prod[l],op(sparse_table[mid][l1], sparse_table[mid][r1])), left_prod[r]);
  }
};

}  // namespace soryuusi
