// https://github.com/soryuusi0219/compro/blob/main/library/math/modint_factorials.hpp
#pragma once
#include <cassert>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
class modint_factorials{
  private:
  vector<T> invs;
  vector<T> facts;
  vector<T> inv_facts;
  int max_calculated = -1;

  public:

  modint_factorials() = default;
  modint_factorials(int n) {precompute(n);}

  void precompute(int n) {
    assert(n >= 0);
    const auto mod = T::mod();
    if(n < 1) n = 1;
    assert(n < mod);
    if(n <= max_calculated) return;
    invs.resize(n+1);
    facts.resize(n+1);
    inv_facts.resize(n+1);
    if(max_calculated == -1) invs[0] = 0, invs[1] = 1, facts[0] = 1, facts[1] = 1, inv_facts[0] = 1, inv_facts[1] = 1, max_calculated = 1;
    for(int i = max_calculated+1; i <= n; i++){
      invs[i] = -invs[mod%i] * T(mod/i);
      facts[i] = facts[i-1] * i;
      inv_facts[i] = inv_facts[i-1] * invs[i];
    }
    max_calculated = n;
  }

  T mint_inv(int x) {
    assert(x != 0);
    if(x < 0) return -mint_inv(-x);
    if(x > max_calculated) precompute(x);
    return invs[x];
  }
  T mint_factorial(int x) {
    assert(x >= 0);
    if(x > max_calculated) precompute(x);
    return facts[x];
  }
  T mint_inv_factorial(int x) {
    assert(x >= 0);
    if(x > max_calculated) precompute(x);
    return inv_facts[x];
  }

  inline T inv(int x) {return mint_inv(x);}
  inline T fact(int x) {return mint_factorial(x);}
  inline T ifact(int x) {return mint_inv_factorial(x);}

  // n * (n-1) * ... * (n-k+1)
  template<bool admit_negative = false>
  T nPk(int n, int k) {
    if(n >= 0 && k > n) return T(0);
    if constexpr (admit_negative){
      if(k < 0) return T(0);
      if(n < 0) return ((k & 1) ? -nPk<false>(-n+k-1,k) : nPk<false>(-n+k-1,k));
    }else{
      assert(n >= 0 && k >= 0);
    }
    if(n > max_calculated) precompute(n);
    return facts[n] * inv_facts[n-k];
  }
  template<bool admit_negative = false>
  T nPk_inv(int n, int k) {
    assert(k >= 0);
    if constexpr (admit_negative){
      if(n < 0) return ((k & 1) ? -nPk_inv<false>(-n+k-1,k) : nPk_inv<false>(-n+k-1,k));
    }else{
      assert(n >= 0);
    }
    assert(k <= n);
    if(n > max_calculated) precompute(n);
    return inv_facts[n] * facts[n-k];
  }
  // [X^k](1+X)^n
  template<bool admit_negative = false>
  T nCk(int n, int k) {
    if constexpr (admit_negative){
      if(k < 0) return T(0);
      if(n < 0) return ((k & 1) ? -nCk<false>(-n+k-1,k) : nCk<false>(-n+k-1,k));
      if(n >= 0 && k > n) return T(0);
    }else{
      assert(k >= 0 && k <= n);
    }
    if(n > max_calculated) precompute(n);
    return facts[n] * inv_facts[k] * inv_facts[n-k];
  }
  template<bool admit_negative = false>
  T nCk_inv(int n, int k) {
    assert(k >= 0);
    if constexpr (admit_negative){
      if(n < 0) return ((k & 1) ? -nCk_inv<false>(-n+k-1,k) : nCk_inv<false>(-n+k-1,k));
    }else{
      assert(n >= 0);
    }
    assert(k <= n);
    if(n > max_calculated) precompute(n);
    return inv_facts[n] * facts[k] * facts[n-k];
  }
  // Catalan number
  // C_n = 1/(n+1) nCk(2n,n)
  T Catalan(int n) {
    assert(n >= 0);
    if(n*2 > max_calculated) precompute(n*2);
    return facts[n*2] * inv_facts[n] * inv_facts[n+1];
  }
  // [X^n]((1-sqrt(1-4X))/2X)^k
  T Catalan_convolution(int n, int k) {
    assert(n >= 0 && k >= 0);
    if(k == 0) return n ? T(0) : T(1);
    if(n == 0) return T(1);
    if(n*2+k-1 > max_calculated) precompute(n*2+k-1);
    return facts[n*2+k-1] * inv_facts[n+k] * inv_facts[n] * T(k);
  }
  // path from (0,0) to (n,m) with y <= x+k
  T Path_lower(int n, int m, int k) {
    assert(n >= 0 && m >= 0);
    if(k < 0 || m > n+k) return T(0);
    if(k >= m) return nCk(n+m,n);
    return nCk(n+m,n) - nCk(n+m,n+k+1);
  }
  // path from (0,0) to (n,m) with x+l <= y <= x+k
  T Path_between(int n, int m, int l, int k) {
    assert(n >= 0 && m >= 0);
    if(l > 0 || k < 0 || m > n+k || m < n+l) return T(0);
    if(l == k) return n || m ? T(0) : T(1); // l = k = 0
    int p = k - l + 2;
    T res = 0;
    int r1 = m % p;
    int r2 = (m-l+1) % p;
    for(int x = r1; x <= n+m; x += p) res += nCk(n+m, x);
    for(int x = r2; x <= n+m; x += p) res -= nCk(n+m, x);
    return res;
  }
  // Narayana number
  // N(n,k) = 1/n nCr(n,k) nCr(n,k-1)
  // parenthesis sequence of length 2n with k peaks
  T Narayana(int n, int k) {
    assert(k >= 0 && n >= k);
    if(n == 0) return T(1);
    if(k == 0) return T(0);
    if(n > max_calculated) precompute(n);
    return facts[n] * facts[n-1] * inv_facts[k] * inv_facts[k-1] * inv_facts[n-k] * inv_facts[n-k+1];
  }
};

}  // namespace soryuusi

// modint_factorials<mint> mf(1'000'000);
