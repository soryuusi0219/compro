// https://github.com/soryuusi0219/compro/blob/main/library/string/lyndon_factorization.hpp
#pragma once
#include <string>
#include <vector>

namespace soryuusi {

using namespace std;

vector<int> LyndonFactorization(const string& s){
  int n = s.size();
  vector<int> res;
  for(int i = 0, j = 1; i < n;){
    int k = i;
    while(k+j < n && s[k] == s[k+j]) k++;
    if(k+j == n || s[k] > s[k+j]){
      while(i <= k) {
        res.push_back(i);
        i += j;
      }
      j = 1;
    }else j += k-i+1;
  }
  res.push_back(n);
  return res;
}

}  // namespace soryuusi
