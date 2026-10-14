// https://github.com/soryuusi0219/compro/blob/main/library/utility/compress.hpp
#pragma once
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
pair<vector<long long>, vector<T>> compress(const vector<T>& v){
  vector<long long> index;
  vector<T> key;
  vector<T> vc = v;
  sort(vc.begin(), vc.end());
  key.push_back(vc[0]);
  for(int i = 0; i < (int)vc.size(); i++){
    if(vc[i] != key.back()) key.push_back(vc[i]); 
  }
  for(int i = 0; i < (int)v.size(); i++){
    index.push_back(distance(key.begin(), lower_bound(key.begin(),key.end(),v[i])));
  }
  return {index,key};
}

}  // namespace soryuusi
