// https://github.com/soryuusi0219/compro/blob/main/library/math/formal_power_series.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <numeric>
#include <queue>
#include <random>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/math/convolution.hpp"
#include "library/math/modint_traits.hpp"
#include "library/math/modint_factorials.hpp"
#include "library/math/convolution_mod.hpp"
#include "library/math/convolution_semi_online.hpp"
#include "library/math/convolution_online.hpp"

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
      return soryuusi::convolution(x,y);
    }else{
      return convolution_mod(x,y);
    }
  }
  friend FPS& operator*=(FPS& x, const FPS& y) {
    if constexpr (is_ntt_friendly_v<T>) {
      if(&x == &y) return x = x * y;
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
    if constexpr (is_ntt_friendly_v<T>) {
      while((int)g.size() < N) {
        const int k = g.size();
        const int z = k << 1;
        FPS f_ntt(this->begin(), this->begin() + min<int>(this->size(), z));
        f_ntt.resize(z);
        FPS g_ntt(g);
        g_ntt.resize(z);
        NTTEngine<T>::ntt(f_ntt);
        NTTEngine<T>::ntt(g_ntt);
        for(int i = 0; i < z; ++i) f_ntt[i] *= g_ntt[i];
        NTTEngine<T>::intt(f_ntt);
        fill(f_ntt.begin(), f_ntt.begin() + k, T(0));
        for(int i = k; i < z; ++i) f_ntt[i] = -f_ntt[i];
        NTTEngine<T>::ntt(f_ntt);
        for(int i = 0; i < z; ++i) f_ntt[i] *= g_ntt[i];
        NTTEngine<T>::intt(f_ntt);
        const int next = min(z, N);
        g.reserve(next);
        g.insert(g.end(), f_ntt.begin() + k, f_ntt.begin() + next);
      }
      return g;
    } else {
      for(int k = 1; k < N;) k <<= 1, (g *= (-pre(k)*g+2)).pred(k);
      g.pred(N);
      return g;
    }
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
    for(int i = this->size()-1; i >= 0; --i) (ans *= x) += (*this)[i];
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
    if(N == 0) return {};
    FPS res(this->begin(), this->begin() + min((int)this->size(), N));
    res.diffed();
    res *= inv(N-1);
    res.pred(N-1).integraled();
    return res;
  }
  FPS exp(int N = -1) const {
    assert(this->empty() || this->front() == 0);
    if(N == -1) N = this->size();
    FPS g = {1};
    for(int k = 1; k < N; k <<= 1) {
      FPS g2 = g*((pre(k<<1)-g.log(k<<1)) >> k);
      g.insert(g.end(), g2.begin(), g2.begin() + k);
    }
    g.pred(N);
    return g;
  }
  FPS pow(long long int k, int N = -1) const {
    if(N == -1) N = this->size();
    if(k == 0){
      FPS res(N,0);
      if(N > 0) res[0] = 1;
      return res;
    }
    int c = 0, n = min((int)this->size(),N);
    while(c < n && (*this)[c] == 0) ++c;
    if(c == n || c >= (N+k-1)/k) return FPS(N,0);
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
    FPS g = {1};
    for(int k = 1; k < N; k <<= 1) {
      FPS g2 = ((pre(k<<1)*g.inv(k<<1)) >> k) / 2;
      g.insert(g.end(), g2.begin(), g2.begin() + k);
    }
    g.resize(N);
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
    FPS sq = (res/=res[0]).sqrt().pre(N-c/2);
    sq *= b;
    sq <<= c/2;
    return sq;
  }
  pair<FPS,FPS> div(FPS g) const {
    FPS f(*this);
    f.shrink(), g.shrink();
    if(f.size() < g.size()) return {{},std::move(f)};
    FPS q = f.rev() / g.reversed();
    q.pred(f.size()-g.size()+1).reversed();
    FPS r = f-g.reversed()*q;
    r.shrink();
    return {std::move(q),std::move(r)};
  }
  vector<T> mult_eval(const vector<T>& p) const {
    int n = p.size();
    if(n == 0) return {};
    vector<T> ans(n);
    constexpr int B = 6;
    if(n < (1 << (B+1))){
      for(auto i = 0; i < n; ++i) ans[i] = eval(p[i]);
      return ans;
    }
    vector<FPS> g(2*n);
    vector<FPS> r(((n*2-1) >> B)+1);
    for(int i = 0; i < n; ++i) g[i+n] = {-p[i],1};
    for(int i = n-1; i; --i) g[i] = g[i<<1] * g[i<<1|1];
    r[1] = (*this);
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
    if(r == 0){
      vector<T> res(m, this->front());
      res[0] = eval(a);
      return res;
    }
    FPS x(n+m,1), y(n,1);
    T e = 1, f = 1;
    T ri = r.inv();
    for(int i = 0; i < n+m-1; ++ i) x[i+1] = e*x[i], e *= r;
    e = 1;
    for(int i = n-2; i >= 0; --i) y[i] = a*e*y[i+1], e *= ri;
    for(int i = 0; i < n; ++i) y[i] *= (*this)[n-i-1];
    FPS z = x*y;
    vector<T> res(m);
    e = 1;
    for(int i = 0; i < m; ++i) res[i] = f*z[n+i-1], f *= e, e *= ri;
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
    if(a == 0) return *this;
    int n = this->size();
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
    int b = std::bit_ceil(static_cast<unsigned int>(n));
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
    return F[0]/G[0];
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


  // returns [x^{N-1}] G * F^i for i=0,...,M-1
  static FPS PowerProjection(FPS F, FPS G, int N, int M){
    if constexpr (is_ntt_friendly_v<T>) {
      using NTT = NTTEngine<T>;
      static constexpr int inv2 = (T::mod() + 1) >> 1;
      
      int n = 32 - __builtin_clz((unsigned)(N-1));
      G <<= ((1 << n) - N);
      N = 1 << n;
      F.pred(N); G.pred(N);
      int n2 = 0, N2 = 1;
      T invN = T(N).inv(), invN2 = 1;
      vector<FPS> P = {G}, Q = {-F};
      while(N > 1){
        vector<FPS> NP(N2 << 1, FPS(N << 1, 0)), NQ(N2 << 1, FPS(N << 1, 0));
        for(int i = 0; i < N2; ++i) {
          copy_n(P[i].begin(), N, NP[i].begin());
          copy_n(Q[i].begin(), N, NQ[i].begin());
        }
        vector<T> p(N2), q(N2);
        for(int j = 0; j < N; ++j){
          for(int i = 0; i < N2; ++i) p[i] = P[i][j], q[i] = Q[i][j];
          NTTEngine<T>::intt_unnormalized(p); NTTEngine<T>::intt_unnormalized(q);
          T e = 1;
          for(int i = 0; i < N2; ++i){
            p[i] *= e; q[i] *= e;
            if(i + 1 != N2) e *= NTT::root(n2+1);
          }
          NTTEngine<T>::ntt(p); NTTEngine<T>::ntt(q);
          for(int i = 0; i < N2; ++i) NP[i+N2][j] = p[i] * invN2, NQ[i+N2][j] = q[i] * invN2;
        }
        P = vector<FPS>(N2 << 1, FPS(N)); Q = vector<FPS>(N2 << 1, FPS(N));
        const T projection_scale = invN * T(inv2);
        for(int i = 0; i < (N2 << 1); ++i){
          NQ[i][0] += (i & N2 ? T(-1) : T(1));
          NTTEngine<T>::ntt(NP[i]); NTTEngine<T>::ntt(NQ[i]);
          T e = 1;
          for(int j = 0; j < N; ++j){
            Q[i][j] = (NQ[i][2*j] * NQ[i][2*j+1]) * invN;
            P[i][j] = (NP[i][2*j] * NQ[i][2*j+1] - NP[i][2*j+1] * NQ[i][2*j]) * projection_scale * e;
            if(j + 1 != N) e *= NTT::inv_rate2(__builtin_ctz(~(unsigned int)(j)));
          }
          NTTEngine<T>::intt_unnormalized(P[i]); NTTEngine<T>::intt_unnormalized(Q[i]);
          P[i].resize(N >> 1); Q[i].resize(N >> 1);
          Q[i][0] -= 1;
        }
        N >>= 1; n--; invN *= 2;
        N2 <<= 1; n2++; invN2 *= inv2;
      }
      FPS p(N2), q(N2);
      for(int i = 0; i < N2; ++i) p[i] = P[i][0]*invN2, q[i] = Q[i][0]*invN2;
      NTTEngine<T>::intt_unnormalized(p); NTTEngine<T>::intt_unnormalized(q);
      q.push_back(1);
      reverse(p.begin(), p.end()); reverse(q.begin(), q.end());
      p.resize(M); q.resize(M);
      return p / q;
    } else {
      assert(false && "PowerProjection is not implemented for non-NTT-friendly types.");
      return {};
    }
  }

  static FPS Compositional_inverse(FPS F, int m = -1){
    if(m == -1) m = F.size();
    assert(F.size() > 1);
    assert(F[0] == 0);
    T c = F[1].inv();
    F *= c;
    FPS G = PowerProjection(F, FPS{T(1)}, m, m);
    for(int i = 1; i < m; ++i) G[i] *= factorials.mint_inv(i) * (m-1);
    reverse(G.begin(), G.end());
    G.pop_back();
    G = (G.pow((-factorials.mint_inv(m-1)).val())) << 1;
    T e = c;
    for(int i = 1; i < m; ++i) G[i] *= e, e *= c;
    return G;
  }

  // return F(G(X))
  static FPS Composition(FPS F, FPS G, int m = -1){
    if constexpr (is_ntt_friendly_v<T>) {
      using NTT = NTTEngine<T>;
      static constexpr int inv2 = (T::mod() + 1) >> 1;
      if(m == -1) m = max(F.size(), G.size());
      if(F.empty()) return FPS(m, 0);
      if(G.empty()) { FPS ret(m, 0); ret[0] = F[0]; return ret; }
      if(m == 1) return {F[0]};
      int n = 32 - __builtin_clz((unsigned)(m-1));
      int N = 1 << n;
      F.resize(N); G.resize(N);
      int n2 = 0, N2 = 1;
      T invN = T(N).inv(), invN2 = 1;
      vector<FPS> Q = {-G};
      vector<vector<FPS>> NQ(n);
      while(N > 1){
        NQ[n2] = vector<FPS>(N2 << 1, FPS(N << 1, 0));
        for(int i = 0; i < N2; ++i) copy_n(Q[i].begin(), N, NQ[n2][i].begin());
        vector<T> q(N2);
        for(int j = 0; j < N; ++j){
          for(int i = 0; i < N2; ++i) q[i] = Q[i][j];
          NTTEngine<T>::intt_unnormalized(q);
          T e = 1;
          for(int i = 0; i < N2; ++i){
            q[i] *= e;
            if(i + 1 != N2) e *= NTT::root(n2+1);
          }
          NTTEngine<T>::ntt(q);
          for(int i = 0; i < N2; ++i) NQ[n2][i+N2][j] = q[i] * invN2;
        }
        Q = vector<FPS>(N2 << 1, FPS(N));
        for(int i = 0; i < (N2 << 1); ++i){
          NQ[n2][i][0] += (i & N2 ? T(-1) : T(1));
          NTTEngine<T>::ntt(NQ[n2][i]);
          for(int j = 0; j < N; ++j) Q[i][j] = (NQ[n2][i][2*j] * NQ[n2][i][2*j+1]) * invN;
          NTTEngine<T>::intt_unnormalized(Q[i]);
          Q[i].resize(N >> 1);
          Q[i][0] -= 1;
        }
        N >>= 1; n--; invN *= 2;
        N2 <<= 1; n2++; invN2 *= inv2;
      }
      FPS q(N2);
      for(int i = 0; i < N2; ++i) q[i] = Q[i][0] * invN2;
      NTTEngine<T>::intt_unnormalized(q);
      q.push_back(1);
      reverse(q.begin(), q.end());
      q = q.inv(N2);
      reverse(q.begin(), q.end());
      F *= q;
      F[0] = F.back();
      for(int i = 1; i < N2; ++i) F[i] = F[i+N2-2];
      F.resize(N2);
      NTTEngine<T>::ntt(F);

      vector<FPS> P(N2, FPS(1));
      for(int i = 0; i < N2; ++i) P[i][0] = F[i] * invN2;

      while(N2 > 1){
        N <<= 1; n++; invN *= T(inv2);
        N2 >>= 1; n2--; invN2 *= 2;
        vector<FPS> NP(N2 << 1, FPS(N << 1, 0));
        const T composition_scale = invN * T(inv2);
        for(int i = 0; i < (N2 << 1); ++i){
          P[i].resize(N);
          reverse(P[i].begin()+1, P[i].end());
          NTTEngine<T>::ntt(P[i]);
          T e = 1;
          for(int j = 0; j < N; ++j){
            const T coefficient = P[i][j] * composition_scale * e;
            NP[i][2*j] = coefficient * NQ[n2][i][2*j+1];
            NP[i][2*j+1] = -coefficient * NQ[n2][i][2*j];
            if(j + 1 != N) e *= NTT::inv_rate2(__builtin_ctz(~(unsigned int)(j)));
          }
          NTTEngine<T>::intt_unnormalized(NP[i]);
          reverse(NP[i].begin()+1, NP[i].end());
        }
        P.clear();
        P.reserve(N2);
        for(int i = 0; i < N2; ++i) P.emplace_back(NP[i].begin(), NP[i].begin() + N);
        vector<T> p(N2);
        for(int j = 0; j < N; ++j){
          for(int i = 0; i < N2; ++i) p[i] = NP[i+N2][j] * invN2;
          NTTEngine<T>::intt_unnormalized(p);
          const T root = NTT::root(n2+1);
          T e = root;
          for(int i = 1; i < N2; ++i) {
            p[N2-i] *= e;
            if(i + 1 != N2) e *= root;
          }
          NTTEngine<T>::ntt(p);
          for(int i = 0; i < N2; ++i) P[i][j] += p[i];
        }
      }
      reverse(P[0].begin(), P[0].end());
      return P[0].pre(m);
    } else {
      assert(false && "composition is not implemented for non-NTT-friendly types.");
      return {};
    }
  }
  FPS composition(const FPS& g, int d = -1) const { return Composition(*this, g, d); }
  FPS compositional_inverse(int d = -1) const { return Compositional_inverse(*this, d); }
};

template <typename T, bool ntt_friendly = is_ntt_friendly_v<T>>
struct static_composition {
  static_composition(const FormalPowerSeries<T>& G) { assert(false && "Not implemented"); }
  static_composition(const FormalPowerSeries<T>& G, int m_) { assert(false && "Not implemented"); }
  FormalPowerSeries<T> composition(FormalPowerSeries<T> F) const { assert(false && "Not implemented"); return {}; }
};

template <typename T>
struct static_composition<T, true> {
  private:
    using NTT = NTTEngine<T>;
    static constexpr int inv2 = (T::mod()+1) >> 1;

    int m, n, N, n2, N2;
    T invN, invN2;
    FormalPowerSeries<T> q;
    vector<vector<FormalPowerSeries<T>>> NQ;

    void init(const FormalPowerSeries<T>& G){
      n = 32 - __builtin_clz(m-1); N = 1 << n; n2 = 0, N2 = 1;
      invN = T(N).inv(), invN2 = 1;
      vector<FormalPowerSeries<T>> Q = {-G}; Q[0].resize(N); NQ.resize(n);
      while(N > 1){
        NQ[n2] = vector<FormalPowerSeries<T>>(N2 << 1, FormalPowerSeries<T>(N << 1, 0));
        for(int i = 0; i < N2; ++i) copy_n(Q[i].begin(), N, NQ[n2][i].begin());
        vector<T> qt(N2);
        for(int j = 0; j < N; ++j){
          for(int i = 0; i < N2; ++i) qt[i] = Q[i][j];
          NTTEngine<T>::intt_unnormalized(qt);
          T e = 1;
          for(int i = 0; i < N2; ++i){
            qt[i] *= e;
            if(i + 1 != N2) e *= NTT::root(n2+1);
          }
          NTTEngine<T>::ntt(qt);
          for(int i = 0; i < N2; ++i) NQ[n2][i+N2][j] = qt[i] * invN2;
        }
        Q = vector<FormalPowerSeries<T>>(N2 << 1, FormalPowerSeries<T>(N));
        for(int i = 0; i < (N2 << 1); ++i){
          NQ[n2][i][0] += (i & N2 ? T(-1) : T(1));
          NTTEngine<T>::ntt(NQ[n2][i]);
          for(int j = 0; j < N; ++j) Q[i][j] = (NQ[n2][i][2*j] * NQ[n2][i][2*j+1]) * invN;
          NTTEngine<T>::intt_unnormalized(Q[i]);
          Q[i].resize(N >> 1); Q[i][0] -= T(1);
        }
        N >>= 1; n--; invN *= 2; N2 <<= 1; n2++; invN2 *= inv2;
      }
      q.resize(N2);
      for(int i = 0; i < N2; ++i) q[i] = Q[i][0]*invN2;
      NTTEngine<T>::intt_unnormalized(q);
      q.push_back(1);
      reverse(q.begin(), q.end());
      q = q.inv(N2);
      reverse(q.begin(), q.end());
    }
  public:
    static_composition(const FormalPowerSeries<T>& G) : m(G.size()) {init(G);}
    static_composition(const FormalPowerSeries<T>& G, int m_) : m(m_) {init(G);}
    FormalPowerSeries<T> composition(FormalPowerSeries<T> F) const {
      if(F.empty()) return FormalPowerSeries<T>(m, 0);
      if(m == 1) return {F[0]};
      F.resize(N2); F *= q; F[0] = F.back();
      for(int i = 1; i < N2; ++i) F[i] = F[i+N2-2];
      F.resize(N2);
      NTTEngine<T>::ntt(F);
      vector<FormalPowerSeries<T>> P(N2, FormalPowerSeries<T>(1));
      for(int i = 0; i < N2; ++i) P[i][0] = F[i] * invN2;

      int N_ = N, n_ = n, N2_ = N2, n2_ = n2;
      T invN_ = invN, invN2_ = invN2; 
      while(N2_ > 1){
        N_ <<= 1; n_++; invN_ *= T(inv2); N2_ >>= 1; n2_--; invN2_ *= 2;
        vector<FormalPowerSeries<T>> NP(N2_ << 1, FormalPowerSeries<T>(N_ << 1, 0));
        const T composition_scale = invN_ * T(inv2);
        for(int i = 0; i < (N2_ << 1); ++i){
          P[i].resize(N_); reverse(P[i].begin()+1, P[i].end());
          NTTEngine<T>::ntt(P[i]);
          T e = 1;
          for(int j = 0; j < N_; ++j){
            const T coefficient = P[i][j] * composition_scale * e;
            NP[i][2*j] = coefficient * NQ[n2_][i][2*j+1];
            NP[i][2*j+1] = -coefficient * NQ[n2_][i][2*j];
            if(j + 1 != N_) e *= NTT::inv_rate2(__builtin_ctz(~(unsigned int)(j)));
          }
          NTTEngine<T>::intt_unnormalized(NP[i]); reverse(NP[i].begin()+1, NP[i].end());
        }
        P.clear();
        P.reserve(N2_);
        for(int i = 0; i < N2_; ++i) P.emplace_back(NP[i].begin(), NP[i].begin() + N_);
        vector<T> p(N2_);
        for(int j = 0; j < N_; ++j){
          for(int i = 0; i < N2_; ++i) p[i] = NP[i+N2_][j] * invN2_;
          NTTEngine<T>::intt_unnormalized(p);
          const T root = NTT::root(n2_+1);
          T e = root;
          for(int i = 1; i < N2_; ++i){
            p[N2_-i] *= e;
            if(i + 1 != N2_) e *= root;
          }
          NTTEngine<T>::ntt(p);
          for(int i = 0; i < N2_; ++i) P[i][j] += p[i];
        }
      }
      reverse(P[0].begin(), P[0].end());
      return P[0].pre(m);
    }
};

template <typename T, int b = 4, bool ntt_friendly = is_ntt_friendly_v<T>>
struct online_composition {
  online_composition(FormalPowerSeries<T> G, int n_) { assert(false && "Not implemented"); }
  T query(T x) { assert(false && "Not implemented"); return 0; }
  inline T get(int i) const { assert(false && "Not implemented"); return 0; }
};

template<typename T, int b>
struct online_composition<T, b, true> {
  private:
    static constexpr int B = 1 << b;
    int N0, n, N, k, c;
    T a, G1;
    FormalPowerSeries<T> logG;
    vector<FormalPowerSeries<T>> powG;
    vector<T> F, ans;
    vector<static_composition<T>> comps;
    inline FormalPowerSeries<T> Gpow(int m, int len){
      if(!m) return {T(1)};
      if(len <= (long long)c*m || c == N-1) return {T(0)};
      return ((FormalPowerSeries<T>(logG.begin(), logG.begin()+(len-c*m)) * T(m)).exp() * a.pow(m)) << (c*m);
    }
  public:
    online_composition(FormalPowerSeries<T> G, int n_) : N0(n_), n(__bit_width((unsigned)n_ - 1)), N(1 << n), k(0), c(0) {
      G.resize(N); powG.resize(N0); F.resize(N0); ans.assign(N0, 0);
      assert(G[0] == 0);
      if(N0 == 1) return;
      G1 = G[1];
      while(c < N && G[c] == 0) ++c;
      if(c < N){ a = G[c]; logG = ((G >> c)/a).log(); --c; }
      powG[1] = FormalPowerSeries<T>(G.begin() + 1, G.begin() + min(N0, B+1));
      for(int i = 1; i < N0-1; ++i) powG[i+1] = (powG[i] * powG[1]).pre(min(N0-i-1, B));
      while(G.size() > B){ comps.emplace_back(G); G.resize(G.size() >> 1); }
      reverse(comps.begin(), comps.end());
    }
    T query(T x){
      if(N0 == 1) { ans[0] = x; return x; }
      F[k] = x;
      ans[k] += x * G1.pow(k);
      for(int i = k+1; i < min<int>(N0, powG[k].size() + k) && (i & (B-1)); ++i) ans[i] += powG[k][i-k] * x;
      ++k;
      if(!(k&(B-1)) && k < N0){
        int d = k & -k;
        FormalPowerSeries<T> F0(F.begin()+k-d, F.begin()+k);
        auto H = comps[__builtin_ctz(d)-b].composition(F0) * Gpow(k-d, min(d<<1, N0-k+d));
        for(int i = 0; i < min(d, N0-k); ++i) ans[k+i] += H[d+i];
      }
      return ans[k-1];
    }
    inline T get(int i) {
      T res = ans[i];
      int k2 = k;
      while(i >= k2){
        ++k2;
        if(!(k2&(B-1)) && k2 < N0){
          int d = k2 & -k2;
          FormalPowerSeries<T> F0(F.begin()+k2-d, F.begin()+k2);
          auto H = comps[__builtin_ctz(d)-b].composition(F0) * Gpow(k2-d, min(d<<1, N0-k2+d));
          for(int j = 0; j < min(d, N0-k2); ++j) if(k2+j == i) res += H[d+j];
        }
      }
      return res;
    }
};

//return [X^{n-1}] D \times (C2 \circ F^{\circ i} \circ C1) for i=0...m-1
template <typename T, class Factorials = modint_factorials<T>>
vector<T> CompositionalPowerProjection(FormalPowerSeries<T> F, int n = -1, int m = -1, FormalPowerSeries<T> C1 = {}, FormalPowerSeries<T> C2 = {}, FormalPowerSeries<T> D = {}){
  Factorials factorials;
  if constexpr (is_ntt_friendly_v<T>) {
    using FPS = FormalPowerSeries<T>;
    if(n == -1) n = F.size();
    if(m == -1) m = n;
    assert(F[0] == 0);
    if(F[1] != T(1)){
      T a = F[1];
      online_composition<T> oc(F, n);
      FPS G(n, 0); G[1] = a;
      oc.query(T(0)); oc.query(a);
      T c = a;
      for(int i = 2; i < n; ++i){
        c *= a;
        auto t = oc.get(i);
        G[i] -= t/(c-a);
        oc.query(G[i]);
      }
      FPS H = G.compositional_inverse();
      if(!C1.empty()) G = G.composition(C1);
      if(!C2.empty()) H = C2.composition(H);
      vector<T> g = FPS::PowerProjection(G, D.empty() ? FPS{T(1)} : D, n, n);
      for(int i = 0; i < n; ++i) H[i] *= g[i];
      return H.geo_eval(T(1), a, m);
    } else {
      if(n == 1) return vector<T>(m, 0);
      if(n == 2) {
        T c1 = C1.empty() ? T(1) : C1[1];
        T c2 = C2.empty() ? T(1) : C2[1];
        T d = D.empty() ? T(1) : D[1];
        return vector<T>(m, c1*c2*d);
      }
      F.resize(n);
      assert(F[3] == F[2]*F[2]);
      FPS H = {0,F[2].inv(),0,0};
      for(int k = 3; k < n-1;){
        int k2 = min(k*2-1,n);
        FPS C(k2-k+1);
        for(int i = 0; i < k2-k+1; ++i) C[i] = factorials.nCk(k-1,i) * (i&1 ? -1 : 1);
        FPS B = (H.compositional_inverse(k2).composition(F,k2).composition(H,k2) >> k)*C;
        for(int i = 0; i < k2-k; ++i) B[i] -= factorials.nCk(k-2,i) * (i&1 ? -1 : 1);
        semi_online_convolution ocA([&]() -> vector<T> {
          vector<T> A(k2-k-1);
          for(int i = 0; i < k2-k-1; ++i) A[i] = factorials.nCk(k-3,i+2) * (i&1 ? -1 : 1);
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
      auto G = H.compositional_inverse();
      if(!C1.empty()) G = G.composition(C1);
      if(!C2.empty()) H = C2.composition(H);
      vector<T> v = !D.empty() ? FPS::PowerProjection(G, G*D, n, n-1) : FPS::PowerProjection(G, G, n, n-1);
      vector<T> w(H.rbegin(), H.rend()-1);
      for(int i = 0; i < n-1; ++i) v[i] *= factorials.mint_factorial(i);
      for(int i = 0; i < n-1; ++i) w[i] *= factorials.mint_inv_factorial(n-i-2);
      auto vw = soryuusi::convolution(v, w);
      FPS f(n-1);
      for(int i = 0; i < n-1; ++i) f[i] = vw[n+i-2] * factorials.mint_inv_factorial(i);
      vector<T> x(m);
      for(int i = 0; i < m; ++i) x[i] = T(i);
      return f.mult_eval(x);
    }
  } else {
    assert(false && "CompositionalPowerProjection is not implemented for non-NTT-friendly types.");
    return {};
  }
}

}  // namespace soryuusi

// soryuusi::modint_factorials<mint> mf(1'000'000);
// using FPS = soryuusi::FormalPowerSeries<mint>;
