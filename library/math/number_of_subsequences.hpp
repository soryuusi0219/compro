// https://github.com/soryuusi0219/compro/blob/main/library/math/number_of_subsequences.hpp
#pragma once
#include <unordered_map>
#include <vector>

namespace soryuusi {

template<class Count, class Value>
Count number_of_subsequences(const std::vector<Value>& values){
  std::unordered_map<Value,Count> previous;
  Count total = 1;
  for(const auto& value : values){
    auto [it, inserted] = previous.try_emplace(value,total);
    const Count duplicates = inserted ? Count(0) : it->second;
    it->second = total;
    total += total-duplicates;
  }
  return total-Count(1);
}

}  // namespace soryuusi
