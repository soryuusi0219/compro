// https://github.com/soryuusi0219/compro/blob/main/library/math/dirichlet_sum.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, int id = -1>
struct Dirichlet_Sum{
  static long long N, K, L;
  static vector<long long> eval_points;
  static vector<long long> primes, divisors;
  static void init(long long n, long long k){
    while(k * k < n) k++;
    N = n, K = k, L = (N-1)/K+1;
    eval_points.resize(L);
    for(int i = 0; i < L; i++) eval_points[i] = (N/(i+1));
    divisors.resize(K);
    for(long long i = 2; i <= K; i++){ 
      if(!divisors[i-1]) {
        primes.push_back(i);
        for(long long j = i*i; j <= K; j += i) if(!divisors[j-1]){
          long long j2 = j;
          while(!(j2 % i)) j2 /= i;
          divisors[j-1] = j2;
        }
      }
    }
  }
  vector<T> al, au;
  bool is_multiplicative = false;
  Dirichlet_Sum(){al.resize(K), au.resize(L);}
  Dirichlet_Sum(vector<T> _al, vector<T> _au, bool is_mul = false) : al(_al), au(_au), is_multiplicative(is_mul) {al.resize(K), au.resize(L);}
  Dirichlet_Sum(vector<T> _al, T (*f)(long long), bool is_mul = false) : al(_al), is_multiplicative(is_mul){
    al.resize(K), au.resize(L);
    for(int i = 0; i < L; i++) au[i] = f(eval_points[i]);
  }
  Dirichlet_Sum(T (*fl)(long long), T (*fu)(long long), bool is_mul = false) : is_multiplicative(is_mul){
    al.resize(K), au.resize(L);
    for(int i = 0; i < K; i++) al[i] = fl(i+1);
    for(int i = 0; i < L; i++) au[i] = fu(eval_points[i]);
  }
  Dirichlet_Sum operator+=(const Dirichlet_Sum& other){for(int i = 0; i < K; i++) al[i] += other.al[i]; for(int i = 0; i < L; i++) au[i] += other.au[i]; is_multiplicative = false; return (*this);}
  Dirichlet_Sum operator+(const Dirichlet_Sum& other) const {Dirichlet_Sum ret(*this); return (ret += other);}
  Dirichlet_Sum operator-=(const Dirichlet_Sum& other){for(int i = 0; i < K; i++) al[i] -= other.al[i]; for(int i = 0; i < L; i++) au[i] -= other.au[i]; is_multiplicative = false; return (*this);}
  Dirichlet_Sum operator-(const Dirichlet_Sum& other) const {Dirichlet_Sum ret(*this); return (ret -= other);}
  Dirichlet_Sum operator*=(const T x){for(T& v : al) v*= x; for(T& v : au) v*= x; is_multiplicative = false; return (*this);}
  Dirichlet_Sum operator*(const T x) const {Dirichlet_Sum ret(*this); return (ret *= x);}
  Dirichlet_Sum operator*(const Dirichlet_Sum& other) const {
    Dirichlet_Sum ret;
    if(is_multiplicative && other.is_multiplicative){
      ret.al[0] = 1;
      for(long long p : primes) for(long long i = 1; i <= K; i*=p) for(long long j = 1; i*j <= K; j*=p) if(i*j > 1) ret.al[i*j-1] += al[i-1]*other.al[j-1];
      for(long long i = 1; i <= K; i++) if(divisors[i-1] > 1) ret.al[i-1] = ret.al[divisors[i-1]-1]*ret.al[i/divisors[i-1]-1];
    }else if(is_multiplicative){
      copy(other.al.begin(), other.al.end(), ret.al.begin());
      for(long long p : primes) for(long long i = (K/p)*p; i >= p; i -= p) {
        long long n = i, q = 1;
        while(!(n%p)) {
          n /= p;
          q *= p;
          ret.al[i-1] += ret.al[n-1]*al[q-1];
        }
      }
    }else if(other.is_multiplicative){
      copy(al.begin(), al.end(), ret.al.begin());
      for(long long p : primes) for(long long i = (K/p)*p; i >= p; i -= p) {
        long long n = i, q = 1;
        while(!(n%p)) {
          n /= p;
          q *= p;
          ret.al[i-1] += ret.al[n-1]*other.al[q-1];
        }
      }
    }else{
      for(long long i = 1; i <= K; i++)for(long long j = 1; i*j <= K; j++) ret.al[i*j-1] += al[i-1]*other.al[j-1]; 
    }
    vector<T> suma(K+1,0), sumb(K+1,0);
    for(long long i = 0; i < K; i++) suma[i+1] += suma[i] + al[i];
    for(long long i = 0; i < K; i++) sumb[i+1] += sumb[i] + other.al[i];
    for(long long n = 1; n <= L; n++){
      T s = 0;
      for(long long i = 1; i*i*n <= N; i++) {
        ret.au[n-1] += al[i-1] * (i*n <= L ? other.au[i*n-1] : sumb[N/(i*n)]);
        s += al[i-1];
      }
      for(long long j = 1; j*j*n < N; j++) {
        ret.au[n-1] += other.al[j-1] * ((j*n <= L ? au[j*n-1] : suma[N/(j*n)]) - s);
      }
    }
    return ret;
  }
  Dirichlet_Sum operator*=(const Dirichlet_Sum& other){(*this) = (*this) * other; return (*this);}
  Dirichlet_Sum operator/(const Dirichlet_Sum& other) const {
    assert(other.al[0] != 0);
    T c = 1/other.al[0];
    Dirichlet_Sum ret;
    if(is_multiplicative && other.is_multiplicative){
      ret.al[0] = 1;
      for(long long p : primes) {
        for(long long i = 1; i <= K; i*=p) {
          if(i > 1) ret.al[i-1] = (al[i-1]-ret.al[i-1]) * c;
          for(long long j = p; i*j <= K; j*=p) ret.al[i*j-1] += ret.al[i-1]*other.al[j-1];
        }
      }
      for(long long i = 1; i <= K; i++) if(divisors[i-1] > 1) ret.al[i-1] = ret.al[divisors[i-1]-1]*ret.al[i/divisors[i-1]-1];
    }
    else if(other.is_multiplicative){
      copy(al.begin(), al.end(), ret.al.begin());
      for(long long p : primes) {
        vector<T> temp(1,T(1));
        for(long long i = p; i <= K; i*=p){
          temp.emplace_back(-other.al[i-1]);
          for(long long j = p, k = temp.size()-2; k > 0; j*=p, k--) temp.back() -= temp[k]*other.al[j-1];
        }
        for(long long i = (K/p)*p; i >= p; i -= p) {
          long long n = i, j = 0;
          while(!(n%p)) {
            n /= p;
            j++;
            ret.al[i-1] += ret.al[n-1]*temp[j];
          }
        }
      }
    }
    else{
      for(long long i = 1; i <= K; i++){
        ret.al[i-1] = (al[i-1]-ret.al[i-1]) * c;
        for(long long j = 2; i*j <= K; j++) {
          ret.al[i*j-1] += ret.al[i-1]*other.al[j-1];
        }
      }
    }
    vector<T> suma(K+1,0), sumb(K+1,0);
    for(long long i = 0; i < K; i++) suma[i+1] += suma[i] + other.al[i];
    for(long long i = 0; i < K; i++) sumb[i+1] += sumb[i] + ret.al[i];
    for(long long n = L; n >= 1; n--){
      ret.au[n-1] = au[n-1];
      T s = other.al[0];
      for(long long i = 2; i*i*n <= N; i++) {
        ret.au[n-1] -= other.al[i-1] * (i*n <= L ? ret.au[i*n-1] : sumb[N/(i*n)]);
        s += other.al[i-1];
      }
      for(long long j = 1; j*j*n < N; j++) {
        ret.au[n-1] -= ret.al[j-1] * ((j*n <= L ? other.au[j*n-1] : suma[N/(j*n)]) - s);
      }
      ret.au[n-1] *= c;
    }
    return ret;
  }
  Dirichlet_Sum operator/=(const Dirichlet_Sum& other){(*this) = (*this) / other; return (*this);}
  Dirichlet_Sum pow(long long k){
    if(k < N){
      auto ret = e();
      Dirichlet_Sum temp(*this);
      while(k){
        if(k & 1) ret *= temp;
        temp *= temp;
        k >>= 1;
      }
      return ret;
    }else{
      Dirichlet_Sum temp(*this);
      T a = temp.al[0];
      temp -= e() * a;
      T ai = T(1)/a;
      T x = a.pow(k);
      Dirichlet_Sum ret = e() * x;
      Dirichlet_Sum temp2 = e();
      long long N2 = N;
      int i = 0;
      while(N2){
        x *= ai * (k-i);
        i++;
        x /= i;
        temp2 *= temp;
        ret += temp2 * x;
        N2 >>= 1;
      }
      return ret;
    }
  }

  inline static Dirichlet_Sum e(){return Dirichlet_Sum([](long long x) -> T{return (x==1 ? 1 : 0);}, [](long long x) -> T{return (x>=1 ? 1 : 0);}, true);}
  inline static Dirichlet_Sum zeta(){return Dirichlet_Sum([](long long x) -> T{return (x>=1 ? 1 : 0);}, [](long long x) -> T{return x;}, true);}
  static Dirichlet_Sum zetak(int k){
    static auto powll = [](long long a, long long b) -> long long{
      long long res=1;
      while(b){
        if(b&1)res=res*a;
        a=a*a;b>>=1;
      }
      return res;
    };
    static auto rootll = [](long long a, long long b) -> long long{
      if(a <= 1) return a;
      if(b >= 64) return 0;
      if(b == 1) return a;
      long long res = powl(a, 1.0l/b) - 1;
      while(powll(res+1,b) <= a) res++;
      return res;
    };
    if(k == 1) return zeta();
    if(!(N >> k)) return e();
    vector<T> vu(K,0), vl(L,0);
    for(int i = 1, j; (j = powll(i,k)) <= K; i++) vu[j-1] = 1;
    for(int i = 0; i < L; i++) vl[i] = rootll(eval_points[i],k);
    return Dirichlet_Sum(vu, vl, true);
  }
  static Dirichlet_Sum zetashift(int k){
    assert(1 <= k && k <= 5);
    if(k == 1) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx;}, [](long long x) -> T{T tx = x; return tx*(tx+1)/2;}, true);
    else if(k == 2) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx;}, [](long long x) -> T{T tx = x; return tx*(tx+1)*(tx*2+1)/6;}, true);
    else if(k == 3) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx*tx;}, [](long long x) -> T{T tx = x; return tx*tx*(tx+1)*(tx+1)/4;}, true);
    else if(k == 4) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx*tx*tx;}, [](long long x) -> T{T tx = x; return tx*(tx+1)*(tx*2+1)*(3*tx*(tx+1)+1)/30;}, true);
    else return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx*tx*tx*tx;}, [](long long x) -> T{T tx = x; return tx*tx*(tx+1)*(tx+1)*(2*tx*(tx+1)+1)*(2*tx*(tx+1)+1)/12;}, true);
  }
  inline T getN(){return au[0];}
  inline T getN() const {return au[0];}
  inline T getNdivi(int i){return au[i-1];}
  inline T getNdivi(int i) const {return au[i-1];}
  inline vector<T> getupper(){return au;}
  inline vector<T> getupper() const {return au;}
  inline pair<vector<T>,vector<T>> getAll(){return {al,au};}
  inline pair<vector<T>,vector<T>> getAll() const {return {al,au};}
};
template<class T, int id> long long Dirichlet_Sum<T,id>::N;
template<class T, int id> long long Dirichlet_Sum<T,id>::K;
template<class T, int id> long long Dirichlet_Sum<T,id>::L;
template<class T, int id> vector<long long> Dirichlet_Sum<T,id>::eval_points;
template<class T, int id> vector<long long> Dirichlet_Sum<T,id>::primes;
template<class T, int id> vector<long long> Dirichlet_Sum<T,id>::divisors;

}  // namespace soryuusi
