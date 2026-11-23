// https://github.com/soryuusi0219/compro/blob/main/library/string/manacher.hpp
#pragma once
#include <string>
#include <vector>

namespace soryuusi {

using namespace std;

namespace internal {

template<class Sequence>
vector<int> manacher_lengths(const Sequence& s){
  int n = s.size();
  if(n == 0) return {};
  vector<int> res(n*2-1,0);
  for(int i = 0, j = 0; i < n*2-1;){
    while(i >= j && i+j < n*2 && s[(i+j)>>1] == s[(i-j)>>1]) j += 2;
    res[i] = j-1;
    int k = 1;
    while(i >= k && k+res[i-k] < j-1) res[i+k] = res[i-k], ++k;
    i += k, j -= k;
  }
  return res;
}

}  // namespace internal

inline vector<int> Manacher(const string& s){return internal::manacher_lengths(s);}

template<typename T>
vector<int> Manacher(const vector<T>& s){return internal::manacher_lengths(s);}

}  // namespace soryuusi
