// https://github.com/soryuusi0219/compro/blob/main/library/math/stern_brocot.hpp
#pragma once
#include <algorithm>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct SternBrocot{
  using Fraction = pair<T,T>;
  using Path = vector<T>;
  private:
    static pair<Fraction,Fraction> __decode_path(const Path& p){
      Fraction x = {0,1}, y = {1,0};
      bool f = false;
      for(auto k : p){
        if((f = !f)){
          x.first += y.first * k;
          x.second += y.second * k;
        }else{
          y.first += x.first * k;
          y.second += x.second * k;
        }
      }
      return {x,y};
    }
    static Path lca_path(const Path& p, const Path& q){
      Path r;
      for(int i = 0; i < min<int>(p.size(),q.size()); ++i){
        r.emplace_back(min(p[i],q[i]));
        if(p[i] != q[i]) return r;
      }
      return r;
    }
  public:
    static Path encode_path(Fraction x){
      Path res;
      auto [p,q] = x;
      while(p != q){
        T k = (p-1)/q;
        res.emplace_back(k);
        p -= q*k;
        swap(p,q);
      }
      return res;
    }
    static Fraction decode_path(const Path &p){
      auto [x,y] = __decode_path(p);
      return {x.first + y.first, x.second + y.second};
    }
    static Fraction lca(Fraction x, Fraction y){
      return decode_path(lca_path(encode_path(x),encode_path(y)));
    }
    static T depth(Fraction x){
      auto p = encode_path(x);
      return reduce(p.begin(),p.end());
    }
    static Fraction ancestor(T d, Fraction x){
      if(!d) return {1,1};
      auto p = encode_path(x);
      for(int i = 0; i < (int)p.size(); ++i){
        if(p[i] >= d){
          p[i] = d;
          p.resize(i+1);
          return decode_path(p);
          break;
        }
        d -= p[i];
      }
      return {0,0};
    }
    static pair<Fraction,Fraction> range(Fraction x){
      return __decode_path(encode_path(x));
    }
    template<class F> static pair<Fraction,Fraction> bisect(F f, T MAX){
      T p = 1, q = 0, r = 0, s = 1;
      bool flag = false;
      while(p+r <= MAX && q+s <= MAX){
        T ng = 0;
        T ok = 1;
        for(int i = 0; r <= ((MAX-p) >> i) && s <= ((MAX-q) >> i) && f(Fraction{p + (r << i), q + (s << i)}) != flag; ++i) ok <<= 1;
        while(ok - ng > 1){
          T m = (ok + ng) >> 1;
          if(m * r > MAX - p || m * s > MAX - q || f(Fraction{p+m*r,q+m*s}) == flag) ok = m;
          else ng = m;
        }
        p += r*ng;
        q += s*ng;
        swap(p,r);
        swap(q,s);
        flag = !flag;
      }
      if(!flag) swap(p,r), swap(q,s);
      return {{p,q},{r,s}};
    }
};
using SB = SternBrocot<long long>;

}  // namespace soryuusi
