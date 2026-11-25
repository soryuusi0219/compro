// https://github.com/soryuusi0219/compro/blob/main/library/utility/compress.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

namespace internal {

template<class T>
vector<uint64_t> compress_sorted_records(const vector<T>& v){
  constexpr uint32_t flip = is_signed_v<T> ? uint32_t(1)<<31 : 0;
  constexpr int bits = 11, buckets = 1<<bits;
  array<array<uint32_t,buckets>,3> counts{};
  vector<uint64_t> records;
  records.reserve(v.size());
  for(size_t i = 0; i < v.size(); ++i){
    uint32_t value = uint32_t(v[i])^flip;
    records.push_back((uint64_t(value)<<32)|i);
    for(int j = 0; j < 3; ++j) ++counts[j][(value>>(j*bits))&(buckets-1)];
  }
  vector<uint64_t> scratch;
  for(int j = 0; j < 3; ++j){
    auto& count = counts[j];
    uint32_t prefix = 0;
    int nonempty = 0;
    for(auto& c : count){
      uint32_t size = c;
      nonempty += size != 0;
      c = prefix;
      prefix += size;
    }
    if(nonempty <= 1) continue;
    if(scratch.empty()) scratch.resize(v.size());
    int shift = 32+j*bits;
    for(uint64_t record : records) scratch[count[(record>>shift)&(buckets-1)]++] = record;
    records.swap(scratch);
  }
  return records;
}

}  // namespace internal

template<class T>
pair<vector<long long>, vector<T>> compress(const vector<T>& v){
  if(v.empty()) return {};
  vector<T> key;
  if(is_sorted(v.begin(),v.end())){
    vector<long long> index(v.size());
    key.push_back(v.front());
    for(size_t i = 1; i < v.size(); ++i){
      if(v[i] != key.back()) key.push_back(v[i]);
      index[i] = key.size()-1;
    }
    return {std::move(index),std::move(key)};
  }
  if constexpr (is_integral_v<T> && sizeof(T) <= 4){
    if(v.size() > 4096 && v.size() <= numeric_limits<uint32_t>::max()){
      auto records = internal::compress_sorted_records(v);
      vector<long long> index(v.size());
      uint32_t prev = 0;
      for(uint64_t record : records){
        uint32_t value = record>>32, pos = record;
        if(key.empty() || value != prev) key.push_back(v[pos]), prev = value;
        index[pos] = key.size()-1;
      }
      return {std::move(index),std::move(key)};
    }
  }
  key = v;
  sort(key.begin(),key.end());
  key.erase(unique(key.begin(),key.end(),[](const T& a,const T& b){return !(a != b);}),key.end());
  vector<long long> index(v.size());
  for(size_t i = 0; i < v.size(); ++i)
    index[i] = lower_bound(key.begin(),key.end(),v[i])-key.begin();
  return {std::move(index),std::move(key)};
}

}  // namespace soryuusi
