// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_2d_sparse.hpp
#pragma once
#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class X = int, class Y = int>
struct Segtree2D_Sparse{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  private:
    struct Segtree1D_Sparse{
      private:
        int n, b;
        vector<Y> vy;
        vector<S> data;
        void _update(int i){data[i] = op(data[i << 1], data[i << 1 | 1]);}
      public:
        Segtree1D_Sparse(){}
        Segtree1D_Sparse(int sz){vy.reserve(sz);}
        void add_point(Y y){vy.push_back(y);}
        void build(){
          sort(vy.begin(),vy.end());
          vy.erase(unique(vy.begin(),vy.end()),vy.end());
          n = vy.size();
          b = 1; while(b < n) b <<= 1;
          data.assign(b*2,e());
        }
        void add(Y y, S a){
          int i = lower_bound(vy.begin(),vy.end(),y) - vy.begin();
          for(i += b; i; i >>= 1) data[i] = op(data[i],a);
        }
        S prod(Y x, Y y){
          int l = lower_bound(vy.begin(),vy.end(),x) - vy.begin();
          int r = lower_bound(vy.begin(),vy.end(),y) - vy.begin();
          S res = e();
          for(l += b, r += b; l < r; l >>= 1, r >>= 1){
            if(l&1) res = op(res, data[l++]);
            if(r&1) res = op(res, data[--r]);
          }
          return res;
        }
        S all_prod(){return data[1];}
    };
    int n, b;
    vector<X> vx;
    vector<pair<X,Y>> vxy;
    vector<Segtree1D_Sparse> data;
  public:
    Segtree2D_Sparse(){}
    Segtree2D_Sparse(int sz){vx.reserve(sz), vxy.reserve(sz);}
    void add_point(X x, Y y){vx.emplace_back(x), vxy.emplace_back(x,y);}
    void build(){
      sort(vx.begin(),vx.end());
      vx.erase(unique(vx.begin(),vx.end()),vx.end());
      n = vx.size();
      b = 1; while(b < n) b <<= 1;
      data.resize(2*b);
      for(auto [x,y] : vxy){
        auto i = lower_bound(vx.begin(),vx.end(),x) - vx.begin();
        for(i += b; i; i >>= 1) data[i].add_point(y);
      }
      for(auto& seg : data) seg.build();
    }
    void add(X x, Y y, S a){
      int i = lower_bound(vx.begin(),vx.end(),x) - vx.begin();
      for(i += b; i; i >>= 1) data[i].add(y,a);
    }
    S prod(X xl, X xr, Y yl, Y yr){
      int l = lower_bound(vx.begin(),vx.end(),xl) - vx.begin();
      int r = lower_bound(vx.begin(),vx.end(),xr) - vx.begin();
      S res = e();
      for(l += b, r += b; l < r; l >>= 1, r >>= 1){
        if(l&1) res = op(res, data[l++].prod(yl,yr));
        if(r&1) res = op(res, data[--r].prod(yl,yr));
      }
      return res;
    }
    S all_prod(){return data[1];}
};

}  // namespace soryuusi
