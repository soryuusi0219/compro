// https://github.com/soryuusi0219/compro/blob/main/library/string/rolling_hash.hpp
#pragma once
#include <algorithm>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<int N = 5, int id = -1>
struct rolhash{
  using ull = unsigned long long;
  static constexpr ull MOD = (1UL << 61) - 1;
  static constexpr ull MASK30 = (1UL << 30) - 1;
  static constexpr ull MASK31 = (1UL << 31) - 1;
  static constexpr ull MASK61 = MOD;
  static constexpr ull POSITIVISER = MOD << 2;
  static ull take_mod(ull x){
    x = (x&MASK61) + (x >> 61);
    if(x>MOD) x -= MOD;
    return x;
  }
  static ull mul(ull a, ull b){
    ull au = a >> 31;
    ull ad = a & MASK31;
    ull bu = b >> 31;
    ull bd = b & MASK31;
    ull mid = ad * bu + au * bd;
    ull midu = mid >> 30;
    ull midd = mid & MASK30;
    return au * bu * 2 + midu + (midd << 31) + ad * bd;
  }
  static bool is_initialized;
  int n;
  static ull base[N];
  vector<vector<ull>> base_pow;
  vector<vector<ull>> data;
  template<class T>
  rolhash(vector<T> v){
    if(!is_initialized){
      random_device seed;
      mt19937 mt(seed());
      for(int i = 0; i < N; i++) base[i] = (mt() << 32) + mt();
      is_initialized = true;
    }
    data.resize(N);
    base_pow.resize(N);
    n = v.size();
    for(int i = 0; i < N; i++) {
      base_pow[i].resize(n+1);
      data[i].resize(n+1);
      base_pow[i][0] = 1;
      data[i][0] = 0;
      for(int j = 0 ; j < n; j++){
        base_pow[i][j+1] = take_mod(mul(base_pow[i][j], base[i]));
        data[i][j+1] = take_mod(mul(data[i][j],base[i]) + (ull)v[j]);
      }
    }
  }
  rolhash(string v){
    if(!is_initialized){
      random_device seed;
      mt19937 mt(seed());
      for(int i = 0; i < N; i++) base[i] = take_mod((mt() << 32) + mt());
      is_initialized = true;
    }
    data.resize(N);
    base_pow.resize(N);
    n = v.size();
    for(int i = 0; i < N; i++) {
      base_pow[i].resize(n+1);
      data[i].resize(n+1);
      base_pow[i][0] = 1;
      data[i][0] = 0;
      for(int j = 0 ; j < n; j++){
        base_pow[i][j+1] = take_mod(mul(base_pow[i][j], base[i]));
        data[i][j+1] = take_mod(mul(data[i][j],base[i]) + (ull)v[j]);
      }
    }
  }
  inline int size() const {return n;}
  template<class T> void push_back(T x){
    for(int i = 0; i < N; i++) {
      base_pow[i].push_back(take_mod(mul(base_pow[i][n], base[i])));
      data[i].push_back(take_mod(mul(data[i][n],base[i]) + (ull)x));
      n++;
    }
  }
  vector<long long> get_hash(int l, int r) const {
    vector<long long> ret(N);
    for(int i = 0; i < N; i++) ret[i] = take_mod(data[i][r] + POSITIVISER - mul(data[i][l], base_pow[i][r-l]));
    return ret;
  }
  bool is_same(int l1, int r1, int l2, int r2){
    return get_hash(l1,r1) == get_hash(l2,r2);
  }
  bool is_same(int l1, int r1, int l2, int r2, const rolhash& y){
    return get_hash(l1,r1) == y.get_hash(l2,r2);
  }
  int lcp(int l1, int l2){
    int ok = 0; int ng = min(n-l1, n-l2)+1;
    while(ng - ok > 1){
      int mid = (ok+ng)/2;
      if(is_same(l1,l1+mid,l2,l2+mid)) ok = mid;
      else ng = mid;
    }
    return ok;
  }
  int lcp(int l1, int l2, const rolhash& y){
    int ok = 0; int ng = min(n-l1, y.size()-l2)+1;
    while(ng - ok > 1){
      int mid = (ok+ng)/2;
      if(is_same(l1,l1+mid,l2,l2+mid,y)) ok = mid;
      else ng = mid;
    }
    return ok;
  }
};
template<int N, int id> bool rolhash<N,id>::is_initialized = false;
template<int N, int id> unsigned long long rolhash<N,id>::base[N];

}  // namespace soryuusi
