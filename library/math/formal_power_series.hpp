// https://github.com/soryuusi0219/compro/blob/main/library/math/formal_power_series.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "internal/internal_bit.hpp"
#include "convolution.hpp"
#include "internal/internal_fps_composition.hpp"
#include "internal/internal_math.hpp"
#include "internal/modint_traits.hpp"
#include "modint_factorials.hpp"
#include "convolution_mod.hpp"
#include "convolution_semi_online.hpp"
#include "convolution_online.hpp"
#include "ntt.hpp"

namespace soryuusi {

using namespace std;

template<class T, class Factorials = modint_factorials<T>>
struct FormalPowerSeries : vector<T>{
  inline static Factorials factorials{};
  using vector<T>::vector;
  using FPS = FormalPowerSeries;
  FormalPowerSeries(vector<T>&& v) : vector<T>(std::move(v)) {}
  template<class U> FormalPowerSeries(const vector<U>& v){
    this->resize(v.size());
    copy(v.begin(), v.end(), this->begin());
  }
  FormalPowerSeries(T x){this->assign(1,x);}
  friend FPS& operator+=(FPS& x, const FPS& y){
    if(y.size() > x.size()) x.resize(y.size());
    for(int i = 0; i < (int)y.size(); ++i) x[i]+=y[i];
    return x;
  }
  friend FPS& operator-=(FPS& x, const FPS& y){
    if(y.size() > x.size()) x.resize(y.size());
    for(int i = 0; i < (int)y.size(); ++i) x[i]-=y[i];
    return x;
  }
  friend FPS& operator+=(FPS& x, const T& y){
    if(x.empty()) return x = {y};
    x[0] += y;
    return x;
  }
  friend FPS& operator-=(FPS& x, const T& y){
    if(x.empty()) return x = {-y};
    x[0] -= y;
    return x;
  }
  friend FPS& operator*=(FPS& x, const T& y){
    for(auto& a : x) a *= y;
    return x;
  }
  friend FPS& operator/=(FPS& x, const T& y){return x *= y.inv();}
  friend FPS operator+(FPS x, const FPS& y) {x += y; return x;}
  friend FPS operator-(FPS x, const FPS& y) {x -= y; return x;}
  friend FPS operator+(FPS x, const T& y) {x += y; return x;}
  friend FPS operator-(FPS x, const T& y) {x -= y; return x;}
  friend FPS operator*(FPS x, const T& y) {x *= y; return x;}
  friend FPS operator/(FPS x, const T& y) {x /= y; return x;}
  FPS operator-() const {
    FPS res(*this);
    for(auto& x : res) x = -x;
    return res;
  }
  friend FPS operator*(const FPS& x, const FPS& y) {
    if constexpr (is_ntt_friendly_v<T>) {
      if(&x == &y) return soryuusi::convolution_square(x);
      return soryuusi::convolution(x,y);
    }else{
      return internal::fps_product(x,y);
    }
  }
  friend FPS& operator*=(FPS& x, const FPS& y) {
    if constexpr (is_ntt_friendly_v<T>) {
      if(&x == &y){
        vector<T> values;
        values.swap(x);
        return x = FPS(soryuusi::convolution_square(std::move(values)));
      }
      vector<T> lhs;
      lhs.swap(x);
      vector<T> rhs(y.begin(), y.end());
      x = FPS(soryuusi::convolution(std::move(lhs), std::move(rhs)));
      return x;
    } else {
      return x = x * y;
    }
  }
  friend FPS operator/(FPS x, const FPS& y) {x /= y; return x;}
  friend FPS& operator/=(FPS& x, const FPS& y) {int N = x.size(); return (x*=y.inv(N)).pred(N);}
  friend FPS operator%(const FPS& x, const FPS& y) {return x.div(y).second;}
  friend FPS& operator%=(FPS& x, const FPS& y) {return x = x % y;}
  FPS inv(int N = -1) const {
    if(N == -1) N = this->size();
    if(N == 0) return {};
    assert(!this->empty() && this->front() != T(0));
    FPS g = {this->front().inv()};
    g.reserve(N);
    for(int i = 1; i < min(32,N); ++i){
      T sum = 0;
      for(int j = 1; j <= min<int>(i,this->size()-1); ++j) sum += (*this)[j]*g[i-j];
      g.push_back(-sum*g[0]);
    }
    if constexpr (is_ntt_friendly_v<T>) {
      using NTT = NTTEngine<T>;
      while((int)g.size() < N && (int)g.size() <= (1 << (NTT::rank2-1))) {
        const int k = g.size();
        const int z = k << 1;
        FPS f_ntt(this->begin(), this->begin() + min<int>(this->size(), z));
        f_ntt.resize(z);
        FPS g_ntt(g);
        g_ntt.resize(z);
        NTT::ntt(f_ntt);
        NTT::ntt(g_ntt);
        for(int i = 0; i < z; ++i) f_ntt[i] *= g_ntt[i];
        NTT::intt_unnormalized(f_ntt);
        const T scale = -NTT::inverse_size(z);
        fill(f_ntt.begin(), f_ntt.begin() + k, T(0));
        for(int i = k; i < z; ++i) f_ntt[i] *= scale;
        NTT::ntt(f_ntt);
        for(int i = 0; i < z; ++i) f_ntt[i] *= g_ntt[i];
        NTT::intt_unnormalized(f_ntt);
        const int next = min(z, N);
        NTT::normalize(f_ntt.begin() + k, f_ntt.begin() + next, z);
        g.insert(g.end(), f_ntt.begin() + k, f_ntt.begin() + next);
      }
    }
    while((int)g.size() < N){
      const int k = g.size(), length = min(2*k,N), count = length-k;
      FPS error(internal::fps_middle_product<T>(pre(length),g));
      error >>= 1;  // Drop coefficient k-1, which is already known.
      error *= g.pre(count);
      for(int i = 0; i < count; ++i) g.push_back(-error[i]);
    }
    return g;
  }
  friend FPS& operator>>=(FPS& x, int k){
    if((int)x.size() < k) x.clear();
    else x.erase(x.begin(),x.begin()+k);
    return x;
  }
  friend FPS& operator<<=(FPS& x, int k){
    x.insert(x.begin(),k,T(0));
    return x;
  }
  friend FPS operator>>(FPS x, int k){x >>= k; return x;}
  friend FPS operator<<(FPS x, int k){x <<= k; return x;}
  T eval(T x) const {
    T ans = 0;
    for(auto it = this->rbegin(); it != this->rend(); ++it) (ans *= x) += *it;
    return ans;
  }
  FPS& shrink() {while(this->size() && this->back() == 0) this->pop_back(); return *this;}
  FPS& reversed(){reverse(this->begin(),this->end()); return *this;}
  FPS rev() const {FPS res(*this); res.reversed(); return res;}
  FPS& pred(int k){this->resize(k,0); return *this;}
  FPS pre(int k) const {
    assert(k >= 0);
    FPS res(this->begin(), this->begin() + min<int>(this->size(), k));
    res.resize(k);
    return res;
  }
  FPS dot(const FPS& other) const {
    FPS res(*this);
    if(res.size() < other.size()) res.resize(other.size());
    for(int i = 0; i < (int)other.size(); ++i) res[i] *= other[i];
    return res;
  }
  FPS& diffed() {
    if(this->empty()) return *this;
    for(int i = 0; i < (int)this->size() - 1; ++i) (*this)[i] = (*this)[i+1] * T(i+1);
    this->pop_back();
    return *this;
  }
  FPS& integraled(){
    this->push_back(0);
    for(int i = this->size()-2; i >= 0; --i) (*this)[i+1] = (*this)[i] * factorials.mint_inv(i+1);
    this->front() = 0;
    return *this;
  }
  FPS diff() const {FPS res(*this); res.diffed(); return res;}
  FPS integral() const {FPS res(*this); res.integraled(); return res;}
  FPS log(int N = -1) const {
    assert(this->empty() || this->front() == 1);
    if(N == -1) N = this->size();
    if(N <= 1) return FPS(N,0);
    FPS res(this->begin(), this->begin() + min((int)this->size(), N));
    res.diffed();
    res *= inv(N-1);
    res.pred(N-1).integraled();
    return res;
  }
  FPS exp(int N = -1) const {
    assert(this->empty() || this->front() == 0);
    if(N == -1) N = this->size();
    if(N == 0) return {};
    if(N == 1) return FPS{T(1)};
    if constexpr (requires {factorials.precompute(N-1);}) factorials.precompute(N-1);
    FPS g = {1};
    g.reserve(N);
    const int input_size = min<int>(N,this->size());
    FPS derivative(input_size > 0 ? input_size-1 : 0);
    for(int i = 1; i < input_size; ++i) derivative[i-1] = (*this)[i]*T(i);
    for(int i = 1; i < min(32,N); ++i){
      T sum = 0;
      for(int j = 0; j < min<int>(i,derivative.size()); ++j) sum += derivative[j]*g[i-1-j];
      g.push_back(sum*factorials.mint_inv(i));
    }
    if constexpr (is_ntt_friendly_v<T>){
      using NTT = NTTEngine<T>;
      if((int)g.size() < N && N <= (1 << NTT::rank2)){
        FPS reciprocal = g.inv();
        FPS a, b, correction, error;
        while((int)g.size() < N){
          const int k = g.size(), z = 2*k, length = min(z,N), count = length-k;
          const T scale = NTT::inverse_size(z), square_scale = scale*scale;
          a.assign(g.begin(),g.end()); a.resize(z);
          b.assign(reciprocal.begin(),reciprocal.end()); b.resize(z);
          NTT::ntt(a); NTT::ntt(b);

          error.assign(derivative.begin(),derivative.begin()+min<int>(length-1,derivative.size()));
          error.resize(z);
          NTT::ntt(error);
          for(int i = 0; i < z; ++i) error[i] *= a[i];
          NTT::intt_unnormalized(error);
          // The cyclic wrap is below degree k-1, where f' * g - g' vanishes.
          std::copy_n(error.begin()+k-1,count,error.begin());
          fill(error.begin()+count,error.end(),T(0));
          NTT::ntt(error);
          for(int i = 0; i < z; ++i) error[i] *= b[i];
          NTT::intt_unnormalized(error);
          for(int i = 0; i < count; ++i) error[i] *= square_scale*factorials.mint_inv(k+i);
          fill(error.begin()+count,error.end(),T(0));
          NTT::ntt(error);

          if(length < N){
            correction.resize(z);
            for(int i = 0; i < z; ++i) correction[i] = a[i]*b[i];
            NTT::intt_unnormalized(correction);
            for(int i = 0; i < k; ++i) correction[i] = correction[i+k]*scale;
            fill(correction.begin()+k,correction.end(),T(0));
            NTT::ntt(correction);
            for(int i = 0; i < z; ++i) correction[i] = (correction[i]+error[i])*b[i];
            NTT::intt_unnormalized(correction);
            for(int i = 0; i < k; ++i) reciprocal.push_back(-correction[i]*scale);
          }
          for(int i = 0; i < z; ++i) a[i] *= error[i];
          NTT::intt_unnormalized(a);
          NTT::normalize(a.begin(),a.begin()+count,z);
          g.insert(g.end(),a.begin(),a.begin()+count);
        }
        return g;
      }
    }
    while((int)g.size() < N){
      const int k = g.size(), length = min(2*k,N);
      FPS g2 = g*((pre(length)-g.log(length)) >> k);
      g.insert(g.end(),g2.begin(),g2.begin()+length-k);
    }
    g.pred(N);
    return g;
  }
  FPS pow(long long int k, int N = -1) const {
    if(N == -1) N = this->size();
    assert(k >= 0);
    if(N == 0) return {};
    if(k == 0){
      FPS res(N,0);
      if(N > 0) res[0] = 1;
      return res;
    }
    int c = 0, n = min((int)this->size(),N);
    while(c < n && (*this)[c] == 0) ++c;
    if(c == n || c > (N-1)/k) return FPS(N,0);
    FPS res(this->begin()+c, this->begin()+n);
    T a = res[0];
    FPS lg = (res/=a).log(N-c*k);
    FPS ex = (lg*=k).exp(N-c*k);
    ex *= a.pow(k);
    ex <<= (int)(c*k);
    return ex;
  }
  FPS sqrt(int N = -1) const {
    if(N == -1) N = this->size();
    if(N == 0) return {};
    assert(!this->empty() && this->front() == T(1));
    if(N == 1) return FPS{T(1)};
    FPS g = {1};
    g.reserve(N);
    const T half = T(2).inv();
    while((int)g.size() < N){
      const int k = g.size(), length = min(2*k,N), count = length-k;
      FPS square = g*g, error(count);
      for(int i = 0; i < count; ++i){
        if(k+i < (int)this->size()) error[i] = (*this)[k+i];
        if(k+i < (int)square.size()) error[i] -= square[k+i];
      }
      error /= g;
      for(T x : error) g.push_back(x*half);
    }
    return g;
  }
  FPS sqrt_general(int N = -1) const {
    if(N == -1) N = this->size();
    int c = 0, n = min((int)this->size(),N);
    while(c < n && (*this)[c] == 0) ++c;
    if(c == n) return FPS(N,0);
    if(c & 1) return {};
    FPS res(this->begin()+c, this->begin()+n);
    T b = modsqrt<T>(res[0]);
    if(b == -1) return {};
    FPS sq = (res/=res[0]).sqrt(N-c/2);
    sq *= b;
    sq <<= c/2;
    return sq;
  }
  pair<FPS,FPS> div(FPS g) const {
    FPS f(*this);
    f.shrink(), g.shrink();
    assert(!g.empty());
    if(f.size() < g.size()) return {{},std::move(f)};
    const int qsize = f.size()-g.size()+1, rsize = g.size()-1;
    if(min(qsize,rsize) <= 32){
      const T inverse = g.back().inv();
      FPS q(qsize);
      for(int i = qsize-1; i >= 0; --i){
        q[i] = f[i+rsize]*inverse;
        for(int j = 0; j < rsize; ++j) f[i+j] -= q[i]*g[j];
      }
      f.resize(rsize);
      return {std::move(q),std::move(f.shrink())};
    }
    FPS q(f.rbegin(),f.rbegin()+qsize);
    q /= FPS(g.rbegin(),g.rbegin()+min<int>(g.size(),qsize));
    q.reversed();
    f.resize(rsize);
    g.resize(rsize);
    FPS product = qsize > rsize ? g*FPS(q.begin(),q.begin()+rsize) : g*q;
    product.resize(rsize);
    f -= product;
    return {std::move(q),std::move(f.shrink())};
  }
  vector<T> mult_eval(const vector<T>& p) const {
    int n = p.size();
    if(n == 0) return {};
    vector<T> ans(n);
    constexpr int B = 6;
    if(min<int>(n,this->size()) < (1 << (B+1))){
      for(auto i = 0; i < n; ++i) ans[i] = eval(p[i]);
      return ans;
    }
    vector<FPS> g(2*n);
    vector<FPS> r(((n*2-1) >> B)+1);
    for(int i = 0; i < n; ++i) g[i+n] = {-p[i],1};
    for(int i = n-1; i; --i) g[i] = g[i<<1] * g[i<<1|1];
    r[1] = this->size() >= g[1].size() ? this->div(g[1]).second : *this;
    for(int i = 2; i <= ((n*2-1) >> B); ++i) r[i] = r[i>>1].div(g[i]).second;
    for(int i = 0; i < n; ++i) ans[i] = r[(i+n) >> B].eval(p[i]);
    return ans;
  }
  vector<T> to_newton(const vector<T>& p) const {
    assert(p.size() == this->size());
    int n = this->size();
    if(n == 0) return {};
    int N = 1;
    while(N < n) N <<= 1;
    vector<FPS> g(2*N), r(2*N);
    vector<T> ans(n);
    for(int i = 0; i < n; ++i) g[i+N] = {-p[i],1};
    for(int i = n; i < N; ++i) g[i+N] = {1};
    for(int i = N-1; i; --i) g[i] = g[i<<1] * g[i<<1|1];
    r[1] = *this;
    for(int i = 1; i < N; ++i){
      auto [q, rem] = r[i].div(g[i<<1]);
      r[i<<1] = std::move(rem);
      r[i<<1|1] = std::move(q);
    }
    for(int i = 0; i < n; ++i) ans[i] = r[i+N].empty() ? T(0) : r[i+N][0];
    return ans;
  }
  vector<T> geo_eval(T a, T r, int m){
    if(m == 0) return {};
    int n = this->size();
    if(n == 0) return vector<T>(m,0);
    if(r == T(1)) return vector<T>(m,eval(a));
    if(r == 0){
      vector<T> res(m, this->front());
      res[0] = eval(a);
      return res;
    }
    FPS x(n+m-1,1), y(n,1);
    T e = 1, f = 1;
    T ri = r.inv();
    for(int i = 0; i+1 < (int)x.size(); ++i) x[i+1] = e*x[i], e *= r;
    e = 1;
    for(int i = n-2; i >= 0; --i) y[i] = a*e*y[i+1], e *= ri;
    for(int i = 0; i < n; ++i) y[i] *= (*this)[n-i-1];
    auto z = internal::fps_middle_product<T>(std::move(x),std::move(y));
    vector<T> res(m);
    e = 1;
    for(int i = 0; i < m; ++i) res[i] = f*z[i], f *= e, e *= ri;
    return res;
  }
  static FPS interpolation(const vector<T>& x, const vector<T>& y){
    int n = x.size();
    if(n == 0) return {};
    vector<FPS> g0(n*2);
    for(int i = 0; i < n; ++i) g0[n+i] = {-x[i],1};
    for(int i = n-1; i; --i) g0[i] = g0[i<<1] * g0[i<<1|1];
    constexpr int B = 6;
    FPS Gd = g0[1].diff();
    vector<T> g(n);
    if(n < (1 << (B+1))) for(auto i = 0; i < n; ++i) g[i] = Gd.eval(x[i]);
    else{
      vector<FPS> r(((n*2-1) >> B)+1);
      r[1] = Gd;
      for(int i = 2; i <= ((n*2-1) >> B); ++i) r[i] = r[i>>1].div(g0[i]).second;
      for(int i = 0; i < n; ++i) g[i] = r[(i+n) >> B].eval(x[i]);
    }
    vector<FPS> q(n*2);
    for(int i = 0; i < n; ++i) q[n+i] = {y[i]/g[i]};
    for(int i = n-1; i; --i) q[i] = g0[i<<1] * q[i<<1|1] + q[i<<1] * g0[i<<1|1];
    return std::move(q[1]);
  }
  static FPS prod(vector<FPS> F){
    int n = F.size();
    if(!n) return{1};
    auto comp = [&F](int i, int j) -> bool {return F[i].size() > F[j].size();};
    priority_queue<int,vector<int>,decltype(comp)> q(comp);
    for(int i = 0; i < n; ++i) q.push(i);
    while(q.size() > 1){
      int i = q.top(); q.pop();
      int j = q.top(); q.pop();
      F[i] *= F[j];
      FPS().swap(F[j]);
      q.push(i);
    }
    return std::move(F[q.top()]);
  }
  static FPS geo_interpolation(const vector<T>& y, T a, T r){
    int n = y.size();
    if(!n) return {};
    if(n == 1) return {y[0]};
    FPS g(n+1,0);
    g[n] = 1;
    T rn = r.pow(n), ri = r.inv(), ai = a.inv(), e = 1;
    if(rn == 1) g[0] = -a.pow(n);
    else for(int i = n-1; i >= 0; --i) g[i] = g[i+1] * a * (e - rn) / (e*r-1), e *= r;
    vector<T> s = g.diff().geo_eval(a,r,n);
    g.pop_back();
    FPS c(n);
    for(int i = 0; i < n; ++i) c[i] = y[i] / s[i];
    FPS f(c.geo_eval(ri,ri,n+1));
    e = -ai;
    for(int i = 0; i <= n; ++i) f[i] *= e, e *= ai;
    FPS res = g*f;
    res.pred(n);
    return res;
  }
  static pair<FPS,FPS> quotient_sum(vector<pair<FPS,FPS>> QR){
    int n = QR.size();
    if(!n) return {{0},{1}};
    FPS ans;
    for(int i = 0; i < n; ++i){
      if(QR[i].first.size() >= QR[i].second.size()){
        auto [q,r] = QR[i].first.div(QR[i].second);
        ans += q;
        QR[i].first = r;
      }
    }
    auto comp = [&QR](int i, int j) -> bool {return QR[i].second.size() > QR[j].second.size();};
    priority_queue<int,vector<int>,decltype(comp)> q(comp);
    for(int i = 0; i < n; ++i) q.push(i);
    while(q.size() > 1){
      int i = q.top(); q.pop();
      int j = q.top(); q.pop();
      QR[i] = {QR[i].second*QR[j].first + QR[i].first*QR[j].second,QR[i].second*QR[j].second};
      pair<FPS,FPS>().swap(QR[j]);
      q.push(i);
    }
    int k = q.top();
    if(ans.size()) QR[k].first += ans * QR[k].second;
    return std::move(QR[k]);
  }
  FPS Taylor_shift(T a) const {
    if(a == 0 || this->empty()) return *this;
    int n = this->size();
    if constexpr (requires {factorials.precompute(n-1);}) factorials.precompute(n-1);
    FPS res(n);
    for(int i = 0; i < n; ++i) res[i] = (*this)[i] * factorials.mint_factorial(i);
    FPS g(n,1);
    for(int i = 0; i < n-1; ++i) g[i+1] = g[i] * a;
    for(int i = 0; i < n; ++i) g[i] *= factorials.mint_inv_factorial(i);
    (res.reversed()*=g).pred(n).reversed();
    for(int i = 0; i < n; ++i) res[i] *= factorials.mint_inv_factorial(i);
    return res;
  }
  static vector<T> shift_sampling(vector<T> f, T c, int m = -1){
    if(f.empty()) return vector<T>(0,m);
    int n = f.size()-1;
    if(m == -1) m = n;
    if(m == 1){
      if(c.val() <= (unsigned)n) return {f[c.val()]};
      T s1 = 1, s2 = 1;
      for(int i = 0; i <= n; ++i){
        f[i] *= s1 * factorials.mint_inv_factorial(i), f[n-i] *= s2 * factorials.mint_inv_factorial(i);
        s1 *= c - i, s2 *= c - n + i;
        if((i+n) & 1) f[i] = -f[i];
      }
      T ans = 0;
      for(int i = 0; i <= n; ++i) ans += f[i];
      return {ans};
    }
    ++n;
    FPS F(n);
    for(int i = 0; i < n; ++i) F[i] = f[i] * factorials.mint_inv_factorial(i);
    FPS G(n);
    for(int i = 0; i < n; ++i) G[i] = factorials.mint_inv_factorial(i) * (i & 1 ? -1 : 1);
    (F*=G).pred(n);
    for(int i = 0; i < n; ++i) F[i] *= factorials.mint_factorial(i);
    FPS H(n,1);
    for(int i = 0; i < n-1; ++i) H[i+1] = H[i] * (c-i);
    for(int i = 0; i < n; ++i) H[i] *= factorials.mint_inv_factorial(i);
    (F *= H.reversed()) >>= (n - 1);
    for(int i = 0; i < n; ++i) F[i] *= factorials.mint_inv_factorial(i);
    FPS I(m);
    for(int i = 0; i < m; ++i) I[i] = factorials.mint_inv_factorial(i);
    (F*=I).pred(m);
    for(int i = 0; i < m; ++i) F[i] *= factorials.mint_factorial(i);
    return F;
  }

  FPS times_sparse(const vector<pair<int,T>>& x, int deg = -1) const {
    if(deg == -1) deg = this -> size();
    FPS res(deg);
    for(auto [shift,a] : x) {
      if(shift >= deg) continue;
      const int length = min<int>(this->size(), deg - shift);
      for(int i = 0; i < length; ++i) res[i + shift] += (*this)[i] * a;
    }
    return res;
  }
  // x must be sorted, x[0] = {0,f_0}
  static FPS inv_sparse(const vector<pair<int,T>>& x, int deg){
    if(deg == 0) return {};
    FPS res(deg);
    T a = x[0].second.inv();
    res[0] = a;
    for(int i = 1; i < deg; ++i){
      for(auto [j,b] : x){
        if(j > i) break;
        if(j) res[i] -= res[i-j] * b;
      }
      res[i] *= a;
    }
    return res;
  }
  // x must be sorted, x[0] = {0,f_0}
  FPS div_sparse(const vector<pair<int,T>>& x, int deg = -1){
    if(deg == -1) deg = this->size();
    FPS res(deg,0);
    copy(this->begin(),this->begin()+min((int)this->size(),deg),res.begin());
    T a = x[0].second.inv();
    res[0] *= a;
    for(int i = 1; i < deg; ++i){
      for(auto [j,b] : x){
        if(j > i) break;
        if(j) res[i] -= res[i-j] * b;
      }
      res[i] *= a;
    }
    return res;
  }
  // x must be sorted, x[0] = {0,1}
  static FPS log_sparse(const vector<pair<int,T>>& x, int deg){
    if(deg == 0) return {};
    FPS f = inv_sparse(x,deg-1);
    FPS res(deg,0);
    for(auto [shift,a] : x) if(shift && shift < deg) {
      const T coefficient = a * shift;
      const int length = min<int>(f.size(), deg - shift);
      for(int i = 0; i < length; ++i) res[i + shift] += f[i] * coefficient;
    }
    for(int i = 1; i < deg; ++i) res[i] *= factorials.mint_inv(i);
    return res;
  }
  // x[0].first > 0
  static FPS exp_sparse(const vector<pair<int,T>>& x, int deg){
    if(deg == 0) return {};
    FPS res(deg,0);
    res[0] = 1;
    for(int i = 1; i < deg; ++i){
      for(auto [j,a] : x){
        if(j > i) break;
        res[i] += res[i-j] * a * j;
      }
      res[i] *= factorials.mint_inv(i);
    }
    return res;
  }
  // x must be sorted, x[0] = {0,1}
  static FPS _pow_sparse(const vector<pair<int,T>>& x, T m, int deg){
    if(deg == 0) return {};
    FPS res(deg,0);
    res[0] = 1;
    for(int i = 1; i < deg; ++i){
      for(auto [j,a] : x){
        if(j > i) break;
        if(j) res[i] += res[i-j] * a * ((m+1)*j-i);
      }
      res[i] *= factorials.mint_inv(i);
    }
    return res;
  }
  // x must be sorted
  static FPS pow_sparse(vector<pair<int,T>> x, long long m, int deg){
    if(deg == 0) return {};
    if(m == 0){
      FPS res(deg,0);
      res[0] = 1;
      return res;
    }
    if(x.empty()) return FPS(deg,0);
    int c = x[0].first;
    if(c >= (deg+m-1)/m) return FPS(deg,0);
    T a = x[0].second;
    T ai = a.inv();
    for(auto& [i,b] : x) i -= c, b *= ai;
    return ((_pow_sparse(x,m,deg-c*m) * a.pow(m)) << (c*m));
  }
  // x must be sorted, x[0] = {0,1}
  static FPS sqrt_sparse(const vector<pair<int,T>>& x,  int deg){
    return _pow_sparse(x, factorials.mint_inv(2), deg);
  }
  static FPS sqrt_general_sparse(vector<pair<int,T>> x, int deg){
    if(deg == 0) return {};
    if(x.empty()) return FPS(deg,0);
    int c = x[0].first;
    if(c & 1) return {};
    T a = x[0].second;
    T b = modsqrt<T>(a);
    if(b == -1) return {};
    T ai = a.inv();
    for(auto& [i,b] : x) i -= c, b *= ai;
    FPS res = sqrt_sparse(x,deg-c) * b;
    res.pred(deg-c/2);
    res <<= c/2;
    return res;
  }
  vector<T> NewtonBasis(vector<T> p) const {
    assert(p.size() == this->size());
    int n = this->size();
    if(n == 0) return {};
    int b = soryuusi::bit_ceil(static_cast<unsigned int>(n));
    vector<FPS> g(n*2), r(n*2);
    vector<T> ans(n);
    for(int i = 0; i < n*2-b; ++i) g[b+i] = {-p[i],1};
    for(int i = n; i < b; ++i) g[i] = {-p[i+n-b],1};
    for(int i = n-1; i; --i) g[i] = g[i<<1] * g[i<<1|1];
    r[1] = (*this);
    for(int i = 1; i < n; ++i)  tie(r[i<<1|1],r[i<<1]) = r[i].div(g[i<<1]);
    for(int i = 0; i < n*2-b; ++i) ans[i] = r[i+b].empty() ? 0 : r[i+b][0];
    for(int i = n; i < b; ++i) ans[i+n-b] = r[i].empty() ? 0 : r[i][0];
    return ans;
  }
  // return [X^k](F/G)
  static T Bostan_Mori(FPS F, FPS G, long long int k){
    assert(k >= 0 && !G.empty() && G[0] != 0);
    if(F.empty()) return 0;
    if constexpr (is_ntt_friendly_v<T>) {
      using NTT = NTTEngine<T>;
      const size_t length = max(F.size(), G.size());
      if(k > 0 && length > 60 && length <= (size_t{1} << (NTT::rank2 - 1))){
        const int n = soryuusi::bit_ceil(length);
        F.resize(2*n);
        G.resize(2*n);
        NTT::ntt(F);
        NTT::ntt(G);
        const T half = NTT::inverse_size(2);
        while(true){
          const bool odd = k&1;
          T phase = half;
          for(int i = 0; i < n; ++i){
            const T left = F[2*i]*G[2*i+1], right = F[2*i+1]*G[2*i];
            F[i] = odd ? (left-right)*phase : (left+right)*half;
            G[i] = G[2*i]*G[2*i+1];
            if(odd && i+1 < n) phase *= NTT::inv_rate2(soryuusi::countr_zero(~static_cast<unsigned int>(i)));
          }
          F.resize(n);
          G.resize(n);
          if(!(k >>= 1)) break;
          NTT::ntt_double(F);
          NTT::ntt_double(G);
        }
        // The inverse-transform constant is the mean of its evaluations.
        return accumulate(F.begin(),F.end(),T(0)) / accumulate(G.begin(),G.end(),T(0));
      }
    }
    FPS G2(G.size());
    while(k){
      if(F.empty()) return 0;
      G2.resize(G.size());
      for(int i = 0; i < (int)G.size(); ++i) G2[i] = (i&1 ? -G[i] : G[i]);
      F *= G2;
      G2 *= G;
      for(int i = 0; i < (int)G.size(); ++i) G[i] = G2[i<<1];
      if(k&1){
        for(int i = 0; (i<<1|1) < (int)F.size(); ++i) F[i] = F[i<<1|1];
        F.resize(F.size()/2);
      }else {
        for(int i = 0; (i<<1) < (int)F.size(); ++i) F[i] = F[i<<1];
        F.resize((F.size()+1)/2);
      }
      k >>= 1;
    }
    return F.empty() ? T(0) : F[0]/G[0];
  }
  //return (cx+d)^n f((ax+b)/(cx+d))
  FPS Mobius_Transform(T a, T b, T c, T d) const {
    if(this->empty()) return {};
    T tmp;
    int n = this->size()-1;
    if(c == 0){
      if(d == 0) {
        FPS res(n+1);
        tmp = this->back();
        for(int i = 0; i <= n; ++i) res[i] = factorials.nCk(n,i) * tmp, tmp *= a;
        tmp = 1;
        for(int i = n; i >= 0; --i) res[i] *= tmp, tmp *= b;
        return res;
      }
      FPS res(n+1);
      tmp = 1;
      for(int i = n; i >= 0; --i) res[i] = (*this)[i] * tmp, tmp *= d;
      for(int i = 0; i <= n; ++i) res[i] *= factorials.mint_factorial(i);
      FPS g(n+1,1);
      for(int i = 0; i < n; ++i) g[i+1] = g[i] * b * factorials.mint_inv(i+1);
      (res.reversed()*=g).pred(n+1).reversed();
      for(int i = 0; i <= n; ++i) res[i] *= factorials.mint_inv_factorial(i);
      tmp = 1;
      for(int i = 0; i <= n; ++i) res[i] *= tmp, tmp *= a;
      return res;
    }
    a /= c;
    FPS res(n+1);
    for(int i = 0; i <= n; ++i) res[i] = (*this)[n-i] * factorials.mint_factorial(n-i);
    FPS g(n+1,1);
    for(int i = 0; i < n; ++i) g[i+1] = g[i] * a * factorials.mint_inv(i+1);
    (res *= g).pred(n+1);
    b -= a*d;
    tmp = 1;
    for(int i = n; i >= 0; --i) res[i] *= tmp * factorials.mint_factorial(i) * factorials.mint_inv_factorial(n-i), tmp *= b;
    for(int i = 0; i < n; ++i) g[i+1] = g[i] * d * factorials.mint_inv(i+1);
    (res.reversed() *= g).pred(n+1).reversed();
    tmp = 1;
    for(int i = 0; i <= n; ++i) res[i] *= tmp * factorials.mint_inv_factorial(i), tmp *= c;
    return res;
  }
  // return f(0)f(1)...f(k-1)
  T prefix_product(long long k) const {
    int deg = this->size()-1;
    if(k <= 32){
      T res = 1;
      for(int i = 0; i < k; ++i) res *= eval(i);
      return res;
    }
    if(k <= deg * 4 + 32){
      vector<T> x(k);
      for(int i = 0; i < k; ++i) x[i] = i;
      auto y = mult_eval(x);
      T res = 1;
      for(T a : y) res *= a;
      return res;
    }
    int b = 0;
    while((((deg << (b+1)) + 1)<<(b+1)) < k) b++;
    //mint B = (1ll << b);
    vector<T> G(deg+1);
    for(int i = 0; i <= deg; ++i) G[i] = eval(i << b);
    T twoi = factorials.mint_inv(2);
    T t = 1;
    for(int i = 0; i < b; ++i){
      t*=twoi;
      vector<T> G1 = shift_sampling(G,t, (deg << i)+1);
      vector<T> G2 = shift_sampling(G, (deg << i)+1, deg << i);
      vector<T> G3 = shift_sampling(G, (deg << i)+1+ t, deg << i);
      for(int j = 0; j < (int)G.size(); ++j) G[j] *= G1[j];
      for(int j = 0; j < (int)G2.size(); ++j) G.push_back(G2[j] * G3[j]);
    }
    T res = 1;
    G = shift_sampling(G,0,k>>b);
    for(T a:G) res *= a;
    int k0 = k & ~((1ll << b)-1);
    vector<T> x(k-k0);
    for(int i = 0; i < k-k0; ++i) x[i] = i+k0;
    auto y = mult_eval(x);
    for(T a : y) res *= a;
    return res;
  }

  // g(0) = 0 and g(x+1)-g(x) = f(x).
  FPS prefix_sum() const {
    const int n = this->size();
    if(n == 0) return {};
    if constexpr (requires { factorials.precompute(n); }) factorials.precompute(n);
    FPS bernoulli(n), reversed(n);
    for(int i = 0; i < n; ++i){
      bernoulli[i] = factorials.mint_inv_factorial(i+1);
      reversed[n-1-i] = (*this)[i]*factorials.mint_factorial(i);
    }
    // B(x) = x/(exp(x)-1). Apply B(d/dx), then integrate with zero constant.
    reversed *= bernoulli.inv();
    FPS result(n+1);
    for(int i = 0; i < n; ++i) result[i+1] = reversed[n-1-i]*factorials.mint_inv_factorial(i+1);
    return result;
  }

private:
  static T exponential_sum_constant(const vector<T>& values, T r, T fact, const vector<T>& inverse){
    // The (d+1)-st difference of r^(-n)*(S(n)-C) is zero.
    const int n = values.size();
    T result = 0, prefix = 0, power = 1;
    for(int i = 0; i < n; ++i){
      prefix += power*values[i];
      result = result*(-r)+prefix*inverse[i+1]*inverse[n-1-i];
      power *= r;
    }
    return result*fact*(T(1)-r).inv().pow(n);
  }

  static T evaluate_consecutive_samples(const vector<T>& values, T x, const vector<T>& inverse){
    if(static_cast<uint64_t>(x.val()) < values.size()) return values[x.val()];
    const int n = values.size();
    vector<T> suffix(n+1,T(1));
    for(int i = n-1; i >= 0; --i) suffix[i] = suffix[i+1]*(x-T(i));
    T answer = 0, prefix = 1;
    for(int i = 0; i < n; ++i){
      const T term = values[i]*prefix*suffix[i+1]*inverse[i]*inverse[n-1-i];
      if((n-1-i)&1) answer -= term;
      else answer += term;
      prefix *= x-T(i);
    }
    return answer;
  }

  T exponential_sum_from_coefficients(T r, optional<long long> count) const {
    int n = this->size();
    while(n > 1 && (*this)[n-1] == T(0)) --n;
    const bool unweighted = r == T(1);
    const auto inverse = internal::factorial_and_inverse_factorials<T>(n-1+unweighted).second;
    FPS denominator(n);
    if(unweighted){
      for(int i = 0; i < n; ++i) denominator[i] = inverse[i+1];
    }else{
      denominator[0] = T(1)-r;
      for(int i = 1; i < n; ++i) denominator[i] = -r*inverse[i];
    }
    FPS moments = denominator.inv();
    if(count){
      // The exponential generating function is (1-r^count*exp(count*x))/(1-r*exp(x)).
      // At r=1 remove the common factor x before forming the quotient.
      const T weight = r.pow(*count), step = T(*count);
      if(weight != T(0)){
        FPS numerator(n);
        T power = unweighted ? step : -weight;
        for(int i = 0; i < n; ++i){
          numerator[i] = power*inverse[i+unweighted];
          power *= step;
        }
        if(!unweighted) numerator[0] += T(1);
        moments *= numerator;
      }
    }
    T result = 0, fact = 1;
    for(int i = 0; i < n; ++i){
      result += (*this)[i]*moments[i]*fact;
      fact *= T(i+1);
    }
    return result;
  }

public:
  // With d+1 samples at 0,...,d of a polynomial of degree at most d.
  static T sum_of_exponential_times_polynomial_limit(const vector<T>& values, T r){
    assert(r != T(1));
    if(values.empty()) return 0;
    if(r == T(0)) return values[0];
    const auto [fact,inverse] = internal::factorial_and_inverse_factorials<T>(values.size());
    return exponential_sum_constant(values,r,fact,inverse);
  }

  static T sum_of_exponential_times_polynomial(vector<T> values, T r, long long n){
    assert(n >= 0);
    if(n == 0 || values.empty()) return 0;
    if(r == T(0)) return values[0];
    if(static_cast<uint64_t>(n) <= values.size()){
      T result = 0;
      for(long long i = n; i > 0; --i) result = result*r + values[i-1];
      return result;
    }
    const auto [fact,inverse] = internal::factorial_and_inverse_factorials<T>(values.size());
    if(r == T(1)){
      values.push_back(0);
      T prefix = 0;
      for(T& x : values){
        const T next = prefix+x;
        x = prefix;
        prefix = next;
      }
      return evaluate_consecutive_samples(values,T(n),inverse);
    }
    const T constant = exponential_sum_constant(values,r,fact,inverse);
    const T inverse_r = r.inv();
    T prefix = 0, power = 1, inverse_power = 1;
    for(T& x : values){
      const T next = prefix+power*x;
      x = (prefix-constant)*inverse_power;
      prefix = next;
      power *= r;
      inverse_power *= inverse_r;
    }
    return constant+r.pow(n)*evaluate_consecutive_samples(values,T(n),inverse);
  }

  T sum_of_exponential_times_polynomial_limit(T r) const {
    assert(r != T(1));
    if(this->empty()) return 0;
    if(r == T(0)) return this->front();
    return exponential_sum_from_coefficients(r,nullopt);
  }

  T sum_of_exponential_times_polynomial(T r, long long n) const {
    assert(n >= 0);
    if(n == 0 || this->empty()) return 0;
    if(r == T(0)) return this->front();
    if(n <= 4){
      T result = 0;
      for(long long i = n; i > 0; --i) result = result*r+eval(T(i-1));
      return result;
    }
    return exponential_sum_from_coefficients(r,n);
  }

  struct Mat {
    FPS a, b, c, d;
    Mat() = default;
    Mat(const FPS& a_, const FPS& b_, const FPS& c_, const FPS& d_) : a(a_), b(b_), c(c_), d(d_) {}
    Mat& operator*=(const Mat& r){
      FPS A = a * r.a + b * r.c;
      FPS B = a * r.b + b * r.d;
      FPS C = c * r.a + d * r.c;
      FPS D = c * r.b + d * r.d;
      A.shrink(); B.shrink(); C.shrink(); D.shrink();
      swap(A, a); swap(B, b); swap(C, c); swap(D, d);
      return *this;
    }
    static Mat I() { return {{T(1)}, {}, {}, {T(1)}}; }
    Mat operator*(const Mat& r) const { Mat result(*this); result *= r; return result; }
    pair<FPS, FPS> app(const pair<FPS, FPS>& p) const {
      FPS q1 = a * p.first + b * p.second;
      FPS q2 = c * p.first + d * p.second;
      q1.shrink(); q2.shrink();
      return {std::move(q1), std::move(q2)};
    }
  };

  static void Naive_GCD(Mat& M, pair<FPS, FPS>& p, T& res, int pre){
    auto [q, r] = p.first.div(p.second);
    FPS C = M.a - M.c * q;
    FPS D = M.b - M.d * q;
    res *= p.second.back().pow(p.first.size() - r.size());
    res *= ((~(p.first.size() ^ pre) & 1) && (~(p.second.size() ^ pre) & 1) ? T(-1) : T(1));
    C.shrink(); D.shrink();
    swap(C, M.c); swap(D, M.d); swap(C, M.a); swap(D, M.b);
    p.first = std::move(p.second);
    p.second = std::move(r);
  }

  static Mat Half_GCD(pair<FPS, FPS> p, T& res, int pre = 0){
    int n = p.first.size();
    int m = p.second.size();
    int k = (n + 1) / 2;
    if(m <= k) return Mat::I();
    auto R1 = Half_GCD({p.first >> k, p.second >> k}, res, pre + k);
    p = R1.app(p);
    if((int)p.second.size() < k) res *= p.first.back().pow(k - p.second.size());
    if((int)p.second.size() <= k) return R1;
    Naive_GCD(R1, p, res, pre);
    if((int)p.second.size() <= k) return R1;
    int l = p.first.size() - 1;
    int j = 2 * k - l;
    p.first >>= j;
    p.second >>= j;
    return Half_GCD(p, res, pre + j) * R1;
  }

  // Return c[0], ..., c[d-1] with a[i] = sum_j c[j]*a[i-1-j] for i >= d.
  static FPS find_linear_recurrence(FPS samples){
    const int n = samples.size(), middle = (n+1)/2;
    if(samples.shrink().empty()) return {};
    pair<FPS,FPS> p{FPS(n+1),std::move(samples)};
    p.first[n] = 1;
    T unused_resultant = 1;
    Mat basis = Mat::I();
    if((int)p.second.size() > middle){
      basis = Half_GCD(p,unused_resultant);
      p = basis.app(p);
      while((int)p.second.size() > middle) Naive_GCD(basis,p,unused_resultant,0);
    }

    // A row (R,Q) of this Euclidean basis obeys Q*A = R mod x^n.
    // If Q has zero constant, use the better of the adjacent Euclidean rows.
    if(basis.d[0] == T(0)){
      if(p.second.empty() || p.first.size() <= n+1-p.second.size()){
        p.second = std::move(p.first);
        basis.d = std::move(basis.b);
      }else Naive_GCD(basis,p,unused_resultant,0);
    }
    FPS q = std::move(basis.d);
    const auto order = max(q.size()-1,p.second.size());
    const T scale = -q[0].inv();
    q >>= 1;
    q.resize(order);
    q *= scale;
    return q;
  }

private:
  static FPS recurrence_denominator(const FPS& recurrence){
    FPS q = -recurrence;
    q <<= 1;
    q[0] = 1;
    return q;
  }

  // Return [x^(end-length+1)], ..., [x^end] of 1/q, including zero for negative indices.
  static FPS reciprocal_window(FPS q, long long end, int length){
    if(end < length){
      FPS result = q.inv(static_cast<int>(end)+1);
      result <<= length-(int)result.size();
      return result;
    }
    FPS opposite = q;
    for(int i = 1; i < (int)q.size(); i += 2) opposite[i] = -opposite[i];
    {
      FPS even = q*opposite;
      for(int i = 0; i < (int)q.size(); ++i) q[i] = even[2*i];
    }
    FPS half = reciprocal_window(std::move(q),end/2,length);
    FPS lifted(2*length);
    for(int i = 0; i < length; ++i) lifted[2*i+1-(end&1)] = half[i];
    return internal::fps_middle_product<T>(std::move(lifted),std::move(opposite));
  }

public:
  static T kth_term_of_linear_recurrence(const FPS& initial, const FPS& recurrence, long long k){
    assert(initial.size() == recurrence.size() && k >= 0);
    if(k < (long long)initial.size()) return initial[k];
    if(initial.empty()) return 0;
    FPS q = recurrence_denominator(recurrence), p = initial*q;
    p.resize(initial.size());
    return Bostan_Mori(std::move(p.shrink()),std::move(q.shrink()),k);
  }

  static FPS linear_recurrence_terms(const FPS& initial, const FPS& recurrence, long long k, int count){
    assert(initial.size() == recurrence.size() && k >= 0 && count >= 0);
    if(count == 0) return {};
    if(count == 1) return {kth_term_of_linear_recurrence(initial,recurrence,k)};
    const int d = recurrence.size();
    if(k < d && count <= d-k) return FPS(initial.begin()+k,initial.begin()+k+count);
    if(d == 0) return FPS(count);
    FPS q = recurrence_denominator(recurrence), p = initial*q;
    p.resize(d);
    if(k <= d){
      const int start = k, precision = start+count;
      return (p*q.inv(precision)).pred(precision) >> start;
    }

    // The low d terms of the quotient of x^k by x^d*q(1/x) are a
    // reversed slice of 1/q. They determine x^k mod the characteristic polynomial.
    FPS quotient = reciprocal_window(q,k-d,d);
    FPS reduced = quotient.reversed()*q.rev().pred(d);
    reduced.resize(d);
    reduced *= T(-1);

    const int take = min(d,count), precision = d+take-1;
    FPS inverse = q.inv(max(precision,count));
    FPS prefix = (p*inverse.pre(precision)).pred(precision);
    FPS result(internal::fps_middle_product<T>(std::move(prefix),std::move(reduced.reversed())));
    if(count <= d) return result;
    (result *= q).pred(d);
    (result *= inverse).pred(count);
    return result;
  }

  static T kth_term_of_linear_recurrence(const FPS& samples, long long k){
    assert(k >= 0);
    if(k < (long long)samples.size()) return samples[k];
    FPS recurrence = find_linear_recurrence(samples);
    return kth_term_of_linear_recurrence(samples.pre(recurrence.size()),recurrence,k);
  }

  static FPS linear_recurrence_terms(const FPS& samples, long long k, int count){
    assert(k >= 0 && count >= 0);
    if(count == 0) return {};
    const long long n = samples.size();
    if(k < n && count <= n-k) return FPS(samples.begin()+k,samples.begin()+k+count);
    FPS recurrence = find_linear_recurrence(samples);
    return linear_recurrence_terms(samples.pre(recurrence.size()),recurrence,k,count);
  }

  static pair<Mat, T> GCD_Mat(const FPS& f, const FPS& g){
    pair<FPS, FPS> p{f, g};
    T res = 1;
    p.first.shrink();
    p.second.shrink();
    int n = p.first.size();
    int m = p.second.size();
    if(n < m){
      auto [resM, res0] = GCD_Mat(p.second, p.first);
      if((~n & 1) && (~m & 1)) res0 *= -1;
      swap(resM.a, resM.b);
      swap(resM.c, resM.d);
      return {std::move(resM), res0 * res};
    }
    Mat resM = Mat::I();
    while(true){
      auto m1 = Half_GCD(p, res);
      p = m1.app(p);
      if(p.second.empty()) return {m1 * resM, res};
      Naive_GCD(m1, p, res, 0);
      if(p.second.empty()) return {m1 * resM, res};
      resM = m1 * resM;
    }
  }

  static FPS PolyGCD(const FPS& f, const FPS& g){
    pair<FPS, FPS> p{f, g};
    Mat m = GCD_Mat(f, g).first;
    p = m.app(p);
    if(!p.first.empty()) p.first *= p.first.back().inv();
    return std::move(p.first);
  }
  FPS gcd(const FPS& g) const { return PolyGCD(*this, g); }

  static pair<bool, FPS> PolyInv(const FPS& f, const FPS& g){
    pair<FPS, FPS> p{f, g};
    Mat m = GCD_Mat(f, g).first;
    FPS gcd_ = m.app(p).first;
    if((int)gcd_.size() != 1) return {false, FPS()};
    return {true, m.a * gcd_.back().inv()};
  }
  pair<bool, FPS> inv_mod(const FPS& g) const { return PolyInv(*this, g); }

  static T Resultant(const FPS& f, const FPS& g){
    pair<FPS, FPS> p{f, g};
    auto [m, res] = GCD_Mat(f, g);
    FPS gcd_ = m.app(p).first;
    if((int)gcd_.size() != 1) return 0;
    return res * gcd_.back().inv();
  }
  T resultant(const FPS& g) const { return Resultant(*this, g); }

  vector<T> FindRoot() const {
    static mt19937 mt(random_device{}());
    static uniform_int_distribution<long long> randp(0, (long long)T::mod() - 1);
    static constexpr long long q = (T::mod() - 1) >> 1;
    
    FPS F(*this);
    F.shrink();
    int n = F.size();
    if(n <= 1) return {};
    if(n == 2) return {-F[0] / F[1]};
    
    FPS Frevinv = F.rev().inv();
    auto modf = [&](FPS& G, FPS& f, FPS& finv){
      if(G.size() >= f.size()){
        FPS Q = G.rev() * finv;
        Q.pred(G.size() - f.size() + 1).reversed();
        G -= f * Q;
        G.shrink();
      }
    };
    
    FPS G = {T(1)};
    FPS G2 = {T(0), T(1)};
    long long k = T::mod();
    while(k){
      if(k & 1) modf(G *= G2, F, Frevinv);
      modf(G2 *= G2, F, Frevinv);
      k >>= 1;
    }
    G -= FPS{T(0), T(1)};
    F = PolyGCD(F, G);
    n = F.size();
    if(n <= 1) return {};
    if(n == 2) return {-F[0] / F[1]};
    
    Frevinv = F.rev().inv();
    vector<T> ans = {};
    vector<FPS> FS = {F};
    while(!FS.empty()){
      T s = randp(mt);
      vector<FPS> NFS;
      for(FPS& f : FS){
        if(f.eval(s) == 0) ans.push_back(s);
        FPS frevinv = f.rev().inv();
        FPS g = {T(1)};
        FPS g2 = {-s, T(1)};
        long long k_pow = q;
        while(k_pow){
          if(k_pow & 1) modf(g *= g2, f, frevinv);
          modf(g2 *= g2, f, frevinv);
          k_pow >>= 1;
        }
        FPS f1 = PolyGCD(f, g -= T(1));
        FPS f2 = PolyGCD(f, g += T(2));
        if(f1.size() == 2) ans.push_back(-f1[0]);
        else if(f1.size() > 2) NFS.push_back(f1);
        if(f2.size() == 2) ans.push_back(-f2[0]);
        else if(f2.size() > 2) NFS.push_back(f2);
      }
      FS = NFS;
    }
    return ans;
  }


  // Returns [x^{N-1}] G * F^i for i=0,...,M-1.
  static FPS PowerProjection(FPS F, FPS G, int N, int M){
    assert(N >= 0 && M >= 0);
    if(M == 0 || N == 0 || G.empty()) return FPS(M,0);
    F.resize(min<int>(N,F.size()));
    F.shrink();
    const T coefficient = N <= (int)G.size() ? G[N-1] : T(0);
    if(M == 1) return FPS{coefficient};
    if(F.size() <= 1){
      const T constant = F.empty() ? T(0) : F[0];
      FPS result(M);
      result[0] = coefficient;
      for(int i = 1; i < M; ++i) result[i] = result[i-1]*constant;
      return result;
    }
    if constexpr (is_ntt_friendly_v<T>){
      using NTT = NTTEngine<T>;
      if((F.empty() || F[0] == T(0)) && N <= (1 << (NTT::rank2-1)) && M > 1){
        const int requested = N;
        N = soryuusi::bit_ceil(static_cast<unsigned int>(N));
        G.resize(requested);
        G <<= N-requested;
        F.resize(N);
        int rows = 1;
        vector<FPS> P = {std::move(G)}, Q = {-F};
        while(N > 1){
          vector<FPS> NP(2*rows), NQ(2*rows);
          for(int i = rows; i < 2*rows; ++i) NP[i].resize(2*N), NQ[i].resize(2*N);
          if(rows == 1){
            copy_n(P[0].begin(),N,NP[1].begin());
            copy_n(Q[0].begin(),N,NQ[1].begin());
          }else{
            vector<T> p(rows), q(rows);
            for(int j = 0; j < N; ++j){
              for(int i = 0; i < rows; ++i) p[i] = P[i][j], q[i] = Q[i][j];
              NTT::ntt_odd_extend(p);
              NTT::ntt_odd_extend(q);
              for(int i = 0; i < rows; ++i) NP[i+rows][j] = p[i], NQ[i+rows][j] = q[i];
            }
          }
          for(int i = 0; i < rows; ++i){
            NP[i] = std::move(P[i]); NP[i].resize(2*N);
            NQ[i] = std::move(Q[i]); NQ[i].resize(2*N);
          }
          P.resize(2*rows); Q.resize(2*rows);
          vector<T> phase(N);
          phase[0] = NTT::inverse_size(2*N);
          for(int j = 1; j < N; ++j)
            phase[j] = phase[j-1]*NTT::inv_rate2(soryuusi::countr_zero(static_cast<unsigned int>(j)));
          for(int i = 0; i < 2*rows; ++i){
            NQ[i][0] += i < rows ? T(1) : T(-1);
            NTT::ntt(NP[i]);
            NTT::ntt(NQ[i]);
            P[i].resize(N);
            for(int j = 0; j < N; ++j)
              P[i][j] = (NP[i][2*j]*NQ[i][2*j+1]-NP[i][2*j+1]*NQ[i][2*j])*phase[j];
            NTT::intt_unnormalized(P[i]);
            P[i].resize(N/2);
            if(N == 2) continue;
            Q[i].resize(N);
            for(int j = 0; j < N; ++j) Q[i][j] = NQ[i][2*j]*NQ[i][2*j+1];
            NTT::intt_unnormalized(Q[i]);
            Q[i].resize(N/2);
            NTT::normalize(Q[i].begin(),Q[i].end(),N);
            Q[i][0] -= T(1);
          }
          N /= 2; rows *= 2;
        }
        FPS result(rows);
        for(int i = 0; i < rows; ++i) result[i] = P[i][0];
        NTT::intt(result);
        reverse(result.begin(),result.end());
        result.resize(M);
        return result;
      }
    }
    return internal::FPSCompositionCRT<FPS>::project(F,G,N,M);
  }

  static FPS Compositional_inverse(FPS F, int m = -1){
    if(m == -1) m = F.size();
    assert(m >= 0);
    if(m <= 1) return FPS(m,0);
    assert(F.size() >= 2 && F[0] == T(0));
    const T c = F[1].inv();
    if(m == 2) return FPS{T(0),c};
    const auto modulus = static_cast<std::uint64_t>(T::mod());
    if(static_cast<std::uint64_t>(m) <= modulus && (is_ntt_friendly_v<T> || internal::is_prime64_runtime(modulus))){
      F.resize(min<int>(F.size(),m));
      F *= c;
      FPS G = PowerProjection(F,FPS{T(1)},m,m);
      for(int i = 1; i < m; ++i) G[i] *= factorials.mint_inv(i)*T(m-1);
      reverse(G.begin(),G.end());
      G.pop_back();
      G = (G.log()*(-factorials.mint_inv(m-1))).exp() << 1;
      T e = c;
      for(int i = 1; i < m; ++i) G[i] *= e, e *= c;
      return G;
    }else{
      FPS g{T(0),c};
      F.resize(min<int>(m,F.size()));
      auto derivative = F.diff();
      while((int)g.size() < m){
        const int k = g.size(), length = min(2*k,m);
        internal::FPSCompositionPlan<FPS> plan(g,length);
        FPS error = plan.composition(F) >> k;
        // The lower k coefficients of F(g)-x already vanish.
        error /= derivative.composition(g,length-k);
        for(T& x : error) g.push_back(-x);
      }
      return g;
    }
  }

  static FPS Composition(FPS F, FPS G, int m = -1){
    if(m == -1) m = max(F.size(),G.size());
    assert(m >= 0);
    if(m == 0 || F.empty()) return FPS(m,0);
    if(G.empty() || F.size() == 1){
      FPS result(m,0);
      result[0] = F[0];
      return result;
    }
    if(m == 1) return FPS{F.eval(G[0])};
    G.resize(min<int>(G.size(),m));
    if(G[0] == T(0)) F.resize(min<int>(F.size(),m));
    return internal::FPSCompositionPlan<FPS>(G,m).composition(std::move(F));
  }
  FPS composition(const FPS& g, int d = -1) const { return Composition(*this, g, d); }
  FPS compositional_inverse(int d = -1) const { return Compositional_inverse(*this, d); }
};

template <typename T, bool ntt_friendly = is_ntt_friendly_v<T>>
struct static_composition {
  using FPS = FormalPowerSeries<T>;
private:
  internal::FPSCompositionPlan<FPS,ntt_friendly && is_ntt_friendly_v<T>> plan;
public:
  static_composition(const FPS& g) : static_composition(g,g.size()) {}
  static_composition(const FPS& g, int m) : plan(g,m) {}
  FPS composition(FPS f) const {return plan.composition(std::move(f));}
};

template <typename T, int b = 4, bool ntt_friendly = is_ntt_friendly_v<T>>
struct online_composition {
  using FPS = FormalPowerSeries<T>;
private:
  static_assert(0 <= b && b < 30);
  static constexpr int B = 1 << b;
  int n, order = 0;
  FPS inner, normalized, low, power, scratch;
  vector<T> coefficients, answer;
  vector<optional<static_composition<T,ntt_friendly>>> plans;
  optional<FPS> logarithm;

  FPS coefficient_power(int exponent, int precision){
    if constexpr (is_ntt_friendly_v<T> && ntt_friendly){
      if(!logarithm) logarithm = (normalized/normalized[0]).log(n);
      auto result = (logarithm->pre(precision)*T(exponent)).exp();
      return result *= normalized[0].pow(exponent);
    }else{
      FPS result{T(1)}, base = normalized.pre(precision);
      while(exponent){
        if(exponent&1) (result *= base).pred(precision);
        if(exponent >>= 1) (base *= base).pred(precision);
      }
      return result;
    }
  }

  void cross_block(int boundary){
    if(order == 0) return;
    const int half = boundary & -boundary, first = boundary-half;
    const int end = min(boundary+half,n), precision = end-first;
    const long long shift = first == 0 ? 0 : 1LL*(order-1)*first;
    if(first != 0 && shift >= precision) return;
    const int level = soryuusi::countr_zero(static_cast<unsigned int>(half));
    auto& plan = plans[level];
    if(!plan) plan.emplace(inner,precision);
    auto contribution = plan->composition(FPS(coefficients.begin()+first,coefficients.end()));
    if(first != 0){
      contribution.resize(precision-shift);
      contribution *= coefficient_power(first,precision-shift);
    }
    for(int j = max<int>(half,shift); j < precision; ++j) answer[first+j] += contribution[j-shift];
  }

public:
  online_composition(FPS g, int n_) : n(n_), inner(std::move(g)), answer(n){
    assert(n >= 0 && (inner.empty() || inner[0] == T(0)));
    inner.resize(min<int>(n,inner.size()));
    coefficients.reserve(n);
    plans.resize(soryuusi::bit_width(static_cast<unsigned int>(n)));
    for(int i = 1; i < (int)inner.size(); ++i) if(inner[i] != T(0)){order = i; break;}
    if(order != 0) normalized = inner >> order;
    low = (inner >> 1).pre(min(B,n));
    power.resize(low.size());
    scratch.resize(low.size());
    if(n > 0) power[0] = 1;
  }

  T query(T x){
    const int i = coefficients.size();
    assert(i < n);
    for(int j = 0; j < min(n-i,B-(i&(B-1))); ++j) answer[i+j] += x*power[j];
    coefficients.push_back(x);
    if(i+1 < n){
      const int length = min(B,n-i-1);
      for(int k = 0; k < length; ++k){
        T sum = 0;
        for(int j = 0; j <= k; ++j) sum += power[j]*low[k-j];
        scratch[k] = sum;
      }
      power.swap(scratch);
      if(!((i+1)&(B-1))) cross_block(i+1);
    }
    return answer[i];
  }

  T get(int i) const {
    assert(0 <= i && i < n);
    if(i <= (int)coefficients.size()) return answer[i];
    const auto weights = FPS::PowerProjection(inner,FPS{T(1)},i+1,coefficients.size());
    return inner_product(coefficients.begin(),coefficients.end(),weights.begin(),T(0));
  }
};

// Returns [x^{n-1}] D * (C2 o F^{o i} o C1), i=0,...,m-1.
template <typename T, class Factorials = modint_factorials<T>>
vector<T> CompositionalPowerProjection(FormalPowerSeries<T> F, int n = -1, int m = -1, FormalPowerSeries<T> C1 = {}, FormalPowerSeries<T> C2 = {}, FormalPowerSeries<T> D = {}){
  using FPS = FormalPowerSeries<T>;
  if(n == -1) n = F.size();
  if(m == -1) m = n;
  assert(n >= 0 && m >= 0);
  if(n == 0 || m == 0) return vector<T>(m,0);
  if(C1.empty()) C1 = {T(0),T(1)};
  if(C2.empty()) C2 = {T(0),T(1)};
  if(D.empty()) D = {T(1)};
  F.resize(n); D.resize(n);
  assert(F[0] == T(0) && C1[0] == T(0));
  if(n == 1) return vector<T>(m,C2[0]*D[0]);

  auto is_unit = [](T x){return std::gcd(static_cast<std::uint64_t>(x.val()),
                                        static_cast<std::uint64_t>(T::mod())) == 1;};
  const T a = F[1];
  if((a == T(0) || is_unit(a)) && all_of(F.begin()+2,F.end(),[](T x){return x == T(0);})){
    auto weights = FPS::PowerProjection(C1,D,n,n);
    C2.resize(n);
    for(int j = 0; j < n; ++j) C2[j] *= weights[j];
    return C2.geo_eval(T(1),a,m);
  }
  if(m > 8 && a != T(1) && is_unit(a)){
    vector<T> differences(n);
    T power = a;
    int i = 2;
    for(; i < n; ++i){
      power *= a;
      differences[i] = power-a;
      if(!is_unit(differences[i])) break;
    }
    if(i == n){
      // G(F(x)) = a*G(x). The nonresonance test makes the recurrence solvable.
      T product = 1;
      for(int j = 2; j < n; ++j) differences[j] *= product, product = differences[j];
      T inverse = product.inv(), p = a.pow(n-1);
      const T a_inverse = a.inv();
      for(int j = n-1; j >= 2; --j){
        const T prefix = j == 2 ? T(1) : differences[j-1];
        differences[j] = inverse*prefix;
        inverse *= p-a;
        p *= a_inverse;
      }
      online_composition<T> online(FormalPowerSeries<T>(F.begin(),F.end()),n);
      FPS g(n);
      g[1] = 1;
      online.query(T(0)); online.query(T(1));
      for(int j = 2; j < n; ++j){
        g[j] = -online.get(j)*differences[j];
        online.query(g[j]);
      }
      auto h = C2.composition(g.compositional_inverse(n),n);
      g = g.composition(C1,n);
      auto weights = FPS::PowerProjection(g,D,n,n);
      for(int j = 0; j < n; ++j) h[j] *= weights[j];
      return h.geo_eval(T(1),a,m);
    }
  }
  bool parabolic = m > 8 && n >= 4 && a == T(1) && is_unit(F[2]) && F[3] == F[2]*F[2];
  if constexpr (!is_ntt_friendly_v<T>) parabolic &= internal::fps_crt_fits<T>(n);
  if(parabolic){
    for(int i = 2; i < n && parabolic; ++i) parabolic = is_unit(T(i));
  }
  if(parabolic){
    Factorials factorials;
    FPS H = {0,F[2].inv(),0,0};
    for(int k = 3; k < n-1;){
      int k2 = min(k*2-1,n);
      FPS C(k2-k+1);
      for(int i = 0; i < k2-k+1; ++i) C[i] = factorials.nCk(k-1,i) * (i&1 ? -1 : 1);
      FPS B = (H.compositional_inverse(k2).composition(F,k2).composition(H,k2) >> k)*C;
      for(int i = 0; i < k2-k; ++i) B[i] -= factorials.nCk(k-2,i) * (i&1 ? -1 : 1);
      semi_online_convolution ocA([&]() -> vector<T> {
        vector<T> A(k2-k-1);
        for(int i = 0; i < k2-k-1 && i+2 <= k-3; ++i)
          A[i] = factorials.nCk(k-3,i+2) * (i&1 ? -1 : 1);
        return A;
      }()), ocV([&]() -> vector<T> {
        vector<T> res(k2-k-1);
        for(int i = 0; i < k2-k-1; ++i) res[i] = factorials.mint_inv_factorial(i+2);
        return res;
      }());
      FPS u(k2-k,0);
      for(int i = 0; i < k2-k-1; ++i){
        u[i] = B[i+1] * factorials.mint_inv(k+i-2);
        T x = ocA.push(u[i]);
        T y = ocV.push(u[i]*factorials.mint_inv_factorial(i)) * factorials.mint_factorial(i+2);
        B[i+2] += x-y;
      }
      u[k2-k-1] = B[k2-k] / factorials.mint_inv(k2-3);
      H = ((H.diff() * u).pre(k2-k) << k) + H;
      k = k2-1;
    }
    auto G = H.compositional_inverse(n);
    G = G.composition(C1,n);
    H = C2.composition(H,n);
    FPS v = FPS::PowerProjection(G,G*D,n,n-1);
    FPS w(H.rbegin(), H.rend()-1);
    for(int i = 0; i < n-1; ++i) v[i] *= factorials.mint_factorial(i);
    for(int i = 0; i < n-1; ++i) w[i] *= factorials.mint_inv_factorial(n-i-2);
    auto vw = v*w;
    FPS f(n-1);
    for(int i = 0; i < n-1; ++i) f[i] = vw[n+i-2] * factorials.mint_inv_factorial(i);
    f[0] += H[0]*D[n-1];
    vector<T> x(m);
    for(int i = 0; i < m; ++i) x[i] = T(i);
    return f.mult_eval(x);
  }

  auto weights = FPS::PowerProjection(C1,D,n,n);
  internal::FPSCompositionPlan<FPS> iterate(F,n);
  FPS current = C2.pre(n);
  vector<T> result(m);
  for(int i = 0; i < m; ++i){
    result[i] = inner_product(current.begin(),current.end(),weights.begin(),T(0));
    if(i+1 < m) current = iterate.composition(std::move(current));
  }
  return result;
}

}  // namespace soryuusi

// soryuusi::modint_factorials<mint> mf(1'000'000);
// using FPS = soryuusi::FormalPowerSeries<mint>;
