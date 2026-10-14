// https://github.com/soryuusi0219/compro/blob/main/library/string/z_algorithm.hpp
#pragma once
#include <vector>

namespace soryuusi {

using namespace std;

template<class V>
vector<int> z_algo(V s){
  int n = s.size();
  vector<int> res(n);
  res[0] = n;
  for(int i = 1, j = 0; i < n; ++i){
    while(i+j < n && s[j] == s[i+j]) ++j;
    if((res[i] = j)) for(int k = 1; res[k] < --j;) res[++i] = res[k++];
  }
  return res;
}

}  // namespace soryuusi
