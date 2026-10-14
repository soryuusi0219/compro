// https://github.com/soryuusi0219/compro/blob/main/library/math/linear_sieve.hpp
#pragma once

#include <vector>

namespace soryuusi {

struct Linear_Sieve{
  std::vector<int> spf;
  std::vector<int> primes;

  Linear_Sieve() {}
  Linear_Sieve(int n) : spf(n){
    primes.reserve(n >> 4);
    for(int i = 2; i < n; ++i) {
      if(!spf[i]) {
        spf[i] = i;
        primes.push_back(i);
      }
      for(int p : primes){
        if(p > spf[i]) break;
        if(i*p >= n) break;
        spf[i*p] = p;
      }
    }
  }

  //smallest prime factor
  inline int operator[](int n) const {return spf[n];}
  inline int smallest_prime_factor(int n) const {return spf[n];}
  inline bool is_prime(int n) const {return spf[n] == n;}
  inline const std::vector<int>& spf_table() const {return spf;}
  inline const std::vector<int>& prime_table() const {return primes;}
};

}  // namespace soryuusi
