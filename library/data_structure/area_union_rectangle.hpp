// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/area_union_rectangle.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <iostream>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct Area_Union_Rectangle{
  private:
    struct LazySegtree{
      private:
        struct S{int mn; T sum;};
        int n, b, h;
        vector<S> data;
        vector<T> y;
        vector<int> lz;
        void upd(int k){
          if(data[k << 1].mn == data[k << 1 | 1].mn) data[k] = {data[k << 1].mn, data[k << 1].sum + data[k << 1 | 1].sum};
          else if(data[k << 1].mn < data[k << 1 | 1].mn) data[k] = data[k << 1];
          else data[k] = data[k << 1 | 1];
        }
        void act(int k, int f) {
          data[k].mn += f;
          if(k < b) lz[k] += f;
        }
        void push(int k) {
          act(k << 1, lz[k]), act(k << 1 | 1, lz[k]);
          lz[k] = 0;
        }
      public:
        T w;
        LazySegtree(const vector<T>& y) : n(y.size()-1), w(0) {
          h = bit_width((unsigned)n - 1), b = 1 << h;
          data.assign(2 * b, {0,0}), lz.assign(b, 0);
          for(int i = 0; i < n; i++) data[b + i] = {0,y[i+1] - y[i]};
          for(int i = b-1; i; --i) upd(i);
        }
        void add(int l, int r) {
          int il = __builtin_ctz(l += b), ir = __builtin_ctz(r += b);
          for(int i = h; i > il; --i) push(l >> i);
          for(int i = h; i > ir; --i) push((r - 1) >> i);

          for(int l2 = l, r2 = r; l2 < r2; l2 >>= 1, r2 >>= 1){
            if(l2&1){
              if(!data[l2].mn) w += data[l2].sum;
              act(l2, 1);
              ++l2;
            }
            if(r2&1){
              --r2;
              if(!data[r2].mn) w += data[r2].sum;
              act(r2, 1);
            }
          }
          for(int i = il + 1; i <= h; ++i) upd(l >> i);
          for(int i = ir + 1; i <= h; ++i) upd((r - 1) >> i);
        }
        void del(int l, int r) {
          int il = __builtin_ctz(l += b), ir = __builtin_ctz(r += b);
          for(int i = h; i > il; --i) push(l >> i);
          for(int i = h; i > ir; --i) push((r - 1) >> i);

          for(int l2 = l, r2 = r; l2 < r2; l2 >>= 1, r2 >>= 1){
            if(l2&1){
              act(l2, -1);
              if(!data[l2].mn) w -= data[l2].sum;
              ++l2;
            }
            if(r2&1){
              --r2;
              act(r2, -1);
              if(!data[r2].mn) w -= data[r2].sum;
            }
          }
          for(int i = il + 1; i <= h; ++i) upd(l >> i);
          for(int i = ir + 1; i <= h; ++i) upd((r - 1) >> i);
        }
        // void print(){
        //   for(auto [mn,sum] : data) cout << "(" << mn << "," << sum << ") ";
        //   cout << w << endl;
        // }
    };
    struct Query{
      T x, yl, yr;
      bool f;
    };
    vector<T> vy;
    vector<Query> query;
  public:
    Area_Union_Rectangle(){}
    Area_Union_Rectangle(int q){query.reserve(q*2); vy.reserve(q*2);}
    void add(T l, T r, T d, T u){
      query.emplace_back(l,d,u,true);
      query.emplace_back(r,d,u,false);
      vy.push_back(d);
      vy.push_back(u);
    }
    T area(){
      if(query.empty()) return T(0);
      sort(query.begin(),query.end(),[](const Query&l, const Query&r){return l.x < r.x;});
      sort(vy.begin(),vy.end());
      vy.erase(unique(vy.begin(),vy.end()),vy.end());
      if(vy.size() < 2) return T(0);
      LazySegtree seg(vy);
      T prevx = query[0].x;
      T ans = 0;
      for(auto [x,yl,yr,f] : query){
        ans += (x - prevx) * seg.w;
        prevx = x;
        int l = lower_bound(vy.begin(),vy.end(),yl) - vy.begin();
        int r = lower_bound(vy.begin(),vy.end(),yr) - vy.begin();
        if(f) seg.add(l,r);
        else seg.del(l,r);
      }
      return ans;
    }
};

}  // namespace soryuusi
