// https://github.com/soryuusi0219/compro/blob/main/library/string/lyndon_factorization.hpp
#pragma once
#include <string>
#include <vector>

namespace soryuusi {

using namespace std;

inline vector<int> LyndonFactorization(const string& s){
  int n = s.size();
  vector<int> res;
  for(int i = 0; i < n;){
    int j = i+1, k = i;
    while(j < n && s[k] <= s[j]){
      if(s[k] < s[j]) k = i;
      else ++k;
      ++j;
    }
    const int length = j-k;
    while(i <= k){
      res.push_back(i);
      i += length;
    }
  }
  res.push_back(n);
  return res;
}

}  // namespace soryuusi
