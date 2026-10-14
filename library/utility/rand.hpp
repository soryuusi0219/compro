// https://github.com/soryuusi0219/compro/blob/main/library/utility/rand.hpp
#pragma once

#include <random>
#include <type_traits>

namespace soryuusi {

inline std::mt19937 random_engine(std::random_device{}());

template <class Int>
Int random_integer(Int lower, Int upper) {
  static_assert(std::is_integral_v<Int>);
  return std::uniform_int_distribution<Int>(lower, upper)(random_engine);
}

}  // namespace soryuusi
