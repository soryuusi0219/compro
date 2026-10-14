// https://github.com/soryuusi0219/compro/blob/main/library/math/light/mint_factorial_light.hpp
#pragma once
#include <vector>

namespace soryuusi {

using namespace std;
using ll = long long;
#define rep(i, n) for(ll i=0;i<(ll)(n);i++)

template <class Mint>
inline vector<Mint> invs;
template <class Mint>
inline vector<Mint> fact;
template <class Mint>
inline vector<Mint> ifact;

template <class Mint>
void mf_init(ll n){
  invs<Mint>.resize(n+1);
  fact<Mint>.resize(n+1);
  ifact<Mint>.resize(n+1);
  fact<Mint>[0] = 1;
  rep(i,n) fact<Mint>[i+1] = fact<Mint>[i]*(i+1);
  ifact<Mint>[n] = fact<Mint>[n].inv();
  rep(i,n) ifact<Mint>[n-i-1] = ifact<Mint>[n-i]*(n-i);
  rep(i,n) invs<Mint>[i+1] = fact<Mint>[i] * ifact<Mint>[i+1];
}

template <class Mint>
Mint nPk(ll n, ll k){
  if(n < 0 || k < 0 || k > n) return 0;
  return fact<Mint>[n] * ifact<Mint>[n-k];
}

template <class Mint>
Mint nCk(ll n, ll k){
  if(n < 0 || k < 0 || k > n) return 0;
  return fact<Mint>[n] * ifact<Mint>[k] * ifact<Mint>[n-k];
}

}  // namespace soryuusi
