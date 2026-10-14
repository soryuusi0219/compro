#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;
using mint = modint998244353;

vector<mint> find_relation(vector<vector<mint>> F){
  int n = F.size();
  int m = 1 << 30;
  for(auto& f : F) m = min(m,(int)f.size());
  for(auto& f : F) f.resize(m);
  vector<int> col(n,-1), row(m,-1);
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      if(F[i][j] != 0 && row[j] == -1) {
        col[i] = j;
        row[j] = i;
        break;
      }
    }
    if(col[i] == -1){
      vector<mint> res(n,0);
      res[i] = 1;
      for(int j = 0; j < m; j++){
        if(F[i][j] != 0){
          res[row[j]] = - F[i][j] / F[row[j]][j];
        }
      }
      return res;
    }
    for(int ii = n-1; ii >= i; ii--) for(int j = 0; j < m; j++){
      if(j != col[i]) F[ii][j] -= F[ii][col[i]] * F[i][j] / F[i][col[i]];
    }
  }
  return {};
}