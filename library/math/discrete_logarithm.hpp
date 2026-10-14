// https://github.com/soryuusi0219/compro/blob/main/library/math/discrete_logarithm.hpp
#pragma once
#include <chrono>
#include <cmath>
#include <cstdint>
#include <vector>

#include "library/data_structure/hash_table.hpp"

namespace soryuusi {

using namespace std;

template<typename F, typename S, auto pow, auto inv, auto apply, uint64_t (*hash)(S)>
long long DiscreteLogarithm(F f, S s, S t, long long MX){
  if(s == t) return 0;
  HashTable<S,long long, hash> table(sqrtl(MX)+1);
  table[t] = 0;
  long long b = 1;
  F invf = inv(f);
  while(b * b < MX) table[t = apply(invf,t)] = b++;
  f = pow(f,b);
  for(int k = 0; k < MX; k += b, s = apply(f,s)) if(table.contains(s)) return table[s] + k;
  return -1;
}

template<typename F, typename S, auto pow, auto apply, uint64_t (*hash)(S)>
long long DiscreteLogarithm_Monoid(F f, S s, S t, long long MX){
  if(s == t) return 0;
  HashTable<S,long long, hash> table(sqrtl(MX)+1);
  table[t] = 0;
  long long b = 1;
  S pt = t;
  while(b * b < MX) table[pt = apply(f,pt)] = b++;
  int k = 1;
  S pres = s;
  F pf = pow(f,b);
  while(k * b <= MX) {
    if(table.contains(s = apply(pf,s))) {
      for(int j = 1; j <= b; ++j) if((pres = apply(f,pres)) == t) return (k-1)*b+j;
      break;
    }
    pres = s, ++k;
  }
  while(k * b <= MX) {
    if(table.contains(s = apply(pf,s))) {
      for(int j = 1; j <= b; ++j) if((pres = apply(f,pres)) == t) return (k-1)*b+j;
      break;
    }
    pres = s, ++k;
  }
  return -1;
}

}  // namespace soryuusi
