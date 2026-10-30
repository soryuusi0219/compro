// https://github.com/soryuusi0219/compro/blob/main/library/math/light/fps_light.hpp
#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

#include "library/math/convolution.hpp"
#include "library/math/modint_traits.hpp"

namespace soryuusi {

using namespace std;
using ll = long long;
using vll = vector<ll>;
#define rep(i, n) for(ll i=0;i<(ll)(n);i++)

template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
struct FPS : vector<Mint>{
  using mint = Mint;
  using vm = vector<mint>;
  using vm::vm;
  using vm::begin;
  using vm::end;
  using vm::front;
  using vm::insert;
  using vm::resize;
  using vm::size;
  FPS(const vm& v){resize(v.size());rep(i,v.size()) (*this)[i] = v[i];}
  FPS(const vll& v){resize(v.size());rep(i,v.size()) (*this)[i] = v[i];}
  FPS operator+=(const FPS& x){
    if(x.size() > size()) resize(x.size());
    rep(i,x.size()) (*this)[i]+=x[i];
    return *this;
  }
  FPS operator-=(const FPS& x){
    if(x.size() > size()) resize(x.size());
    rep(i,x.size()) (*this)[i]-=x[i];
    return *this;
  }
  FPS operator*=(const mint& m){
    rep(i,size()) (*this)[i] *= m;
    return *this;
  }
  FPS operator/=(const mint& m){
    mint mi = m.inv(); 
    rep(i,size()) (*this)[i] *= mi;
    return *this;
  }
  FPS operator+(const FPS& x) const {return FPS(*this)+=x;}
  FPS operator-(const FPS& x) const {return FPS(*this)-=x;}
  FPS operator-() const {return (*this)*mint(-1);}
  FPS operator*(mint m) const {return FPS(*this)*=m;}
  FPS operator/(mint m) const {return FPS(*this)/=m;}
  FPS operator*(const FPS& x) const {return convolution(*this, x);}
  FPS operator/(const FPS& x) const {
    FPS ret = (*this) * x.inv(size());
    ret.resize(size());
    return ret;
  }
  FPS operator*=(const FPS& x){return *this = (*this * x);}
  FPS operator/=(const FPS& other){return *this = (*this) / other;}

  FPS pre(ll k) const {return FPS(begin(), begin() + min((ll)size(), k));}
  
  FPS inv(ll N = -1) const {
    if(N == -1) N = size();
    FPS g = {front().inv()};
    ll k = 1;
    while(k < N){
      k = min(2*k,N);
      g *= -pre(k)*g+FPS{2};
      g.resize(k);
    }
    return g;
  }
  FPS diff() const {
    if(size() == 0) return {};
    FPS res(begin()+1, end());
    rep(i,res.size()) res[i] *= i+1;
    return res;
  }
  FPS integral() const {
    FPS ret(*this);
    rep(i,size()) ret[i] /= (i+1);
    ret.insert(ret.begin(),0);
    return ret;
  }
  FPS log(ll N = -1) const {
    FPS res(*this);
    if(N >= 0) res.resize(N);
    return (res.diff()/res).integral();
  }
  FPS exp(ll N = -1) const {
    if(N == -1) N = size();
    FPS g = {1};
    ll k = 1;
    while(k < N){
      k = min(2*k,N);
      g = (pre(k)-g.log(k)+FPS{1})*g;
      g.resize(k);
    }
    return g;
  }
  FPS pow(ll k, ll N = -1) const {
    if(N == -1) N = size();
    mint a = front();
    FPS ret(*this);
    ret.resize(N);
    return ((ret/a).log(N)*mint(k)).exp(N) * a.pow(k);
  }
};

}  // namespace soryuusi
