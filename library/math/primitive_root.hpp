// https://github.com/soryuusi0219/compro/blob/main/library/math/primitive_root.hpp
#pragma once

#include "library/math/prime_factorization.hpp"


namespace soryuusi {

long long primitive_root(long long m) {
  if(m <= 4096LL) return internal::primitive_root_constexpr(static_cast<int>(m));
  auto divs = prime_factorization(m - 1);
  divs.erase(unique(divs.begin(),divs.end()),divs.end());
  for (int g = 2;; ++g) {
    bool ok = true;
    for (auto q : divs) {
      if (internal::pow_mod64_constexpr(g, (m - 1) / q, m) == 1) {
        ok = false;
        break;
      }
    }
    if (ok) return g;
  }
}

}  // namespace soryuusi
