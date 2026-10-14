// https://github.com/soryuusi0219/compro/blob/main/library/string/manacher.hpp
#pragma once
#include <string>
#include <vector>

namespace soryuusi {

using namespace std;

vector<int> Manacher(const string& s){
  int n = s.size();
  vector<int> res(n*2-1,0);
  for(int i = 0, j = 0; i < n*2-1;){
    while(i >= j && i+j < n*2 && s[(i+j)>>1] == s[(i-j)>>1]) j += 2;
    res[i] = j;
    int k = 1;
    while(i >= k && k+res[i-k] < j) res[i+k] = res[i-k], ++k;
    i += k, j -= k;
  }
  for(int i = 0; i < n*2-1; ++i) --res[i];
  return res;
}

template<typename T>
vector<int> Manacher(const vector<T>& s){
  int n = s.size();
  vector<int> res(n*2-1,0);
  for(int i = 0, j = 0; i < n*2-1;){
    while(i >= j && i+j < n*2 && s[(i+j)>>1] == s[(i-j)>>1]) j += 2;
    res[i] = j;
    int k = 1;
    while(i >= k && k+res[i-k] < j) res[i+k] = res[i-k], ++k;
    i += k, j -= k;
  }
  for(int i = 0; i < n*2-1; ++i) --res[i];
  return res;
}

}  // namespace soryuusi
