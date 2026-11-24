// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_2d_sparse.hpp
#pragma once
#include <algorithm>
#include <iterator>
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
    struct Row {
      int b = 1;
      vector<Y> ys;
      vector<S> data;

      void init() {
        b = ys.empty() ? 1 : ys.size();
        data.assign(b*2,e());
      }
      void add(const Y& y, const S& a) {
        int i = lower_bound(ys.begin(),ys.end(),y)-ys.begin();
        for(i += b; i; i >>= 1) data[i] = op(data[i],a);
      }
      S prod(const Y& lower, const Y& upper) const {
        int l = lower_bound(ys.begin(),ys.end(),lower)-ys.begin();
        int r = lower_bound(ys.begin(),ys.end(),upper)-ys.begin();
        if(l == 0 && r == b) return data[1];
        S result = e();
        for(l += b, r += b; l < r; l >>= 1, r >>= 1) {
          if(l & 1) result = op(result,data[l++]);
          if(r & 1) result = op(result,data[--r]);
        }
        return result;
      }
    };
    int b = 1;
    vector<X> vx;
    vector<pair<X,Y>> points;
    vector<Row> rows;

  public:
    Segtree2D_Sparse(){}
    Segtree2D_Sparse(int sz){points.reserve(sz);}
    void add_point(X x, Y y){points.emplace_back(x,y);}
    void build(){
      sort(points.begin(),points.end(),[](const auto& a,const auto& b){
        if(a.first < b.first) return true;
        if(b.first < a.first) return false;
        return a.second < b.second;
      });
      points.erase(unique(points.begin(),points.end()),points.end());
      vx.clear();
      for(const auto& [x,y] : points) if(vx.empty() || !(vx.back() == x)) vx.push_back(x);
      b = vx.empty() ? 1 : vx.size();
      rows.clear();
      rows.resize(2*b);
      int i = 0;
      for(const auto& [x,y] : points) {
        if(!(vx[i] == x)) ++i;
        rows[b+i].ys.push_back(y);
      }
      for(i = b-1; i; --i) {
        const auto& left = rows[i<<1].ys;
        const auto& right = rows[i<<1|1].ys;
        auto& ys = rows[i].ys;
        ys.reserve(left.size()+right.size());
        set_union(left.begin(),left.end(),right.begin(),right.end(),back_inserter(ys));
      }
      for(i = 1; i < 2*b; ++i) rows[i].init();
    }
    void add(X x, Y y, S a){
      int i = lower_bound(vx.begin(),vx.end(),x)-vx.begin();
      for(i += b; i; i >>= 1) rows[i].add(y,a);
    }
    S prod(X xl, X xr, Y yl, Y yr){
      int l = lower_bound(vx.begin(),vx.end(),xl)-vx.begin();
      int r = lower_bound(vx.begin(),vx.end(),xr)-vx.begin();
      if(l == 0 && r == b) return rows[1].prod(yl,yr);
      S result = e();
      for(l += b, r += b; l < r; l >>= 1, r >>= 1) {
        if(l & 1) result = op(result,rows[l++].prod(yl,yr));
        if(r & 1) result = op(result,rows[--r].prod(yl,yr));
      }
      return result;
    }
    S all_prod(){return rows.empty() ? e() : rows[1].data[1];}
};

}  // namespace soryuusi
