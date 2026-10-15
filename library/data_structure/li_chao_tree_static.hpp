// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/li_chao_tree_static.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/li_chao_tree_line.hpp"

namespace soryuusi {

using namespace std;

template<class X, class Y, class Line, auto INF, class CompareX = less<X>, class CompareY = less<Y>>
struct Static_LiChaoTree{
  static_assert(is_invocable_r_v<Line, decltype(INF)>, "INF must be callable with no arguments and return a value convertible to Line");

  private:
    int n, b;
    vector<X> xl, xr;
    vector<Line> seg;
    void _add_Segment(Line L, X ql, X qr, int i = 1){
        const X l = xl[i], r = xr[i];
        if(!CompareX()(l,qr) || CompareX()(r,ql)) return;
        if(!CompareX()(l,ql) && CompareX()(r,qr)) {
          _add_Line(L, i);
          return;
        }
        if(i < b){
          _add_Segment(L, ql, qr, i << 1);
          _add_Segment(L, ql, qr, i << 1 | 1);
        }
    }
    void _add_Line(Line L, int i = 1){
      while(true){
        const X l = xl[i], r = xr[i];
        Y yl = seg[i].get(l);
        Y yr = seg[i].get(r);
        Y nyl = L.get(l);
        Y nyr = L.get(r);
        if(i >= b || CompareY()(nyl,yl) == CompareY()(nyr,yr)){
          if(CompareY()(nyl,yl)) swap(seg[i],L);
          return;
        }
        const X m = xr[i << 1];
        Y ym = seg[i].get(m);
        Y nym = L.get(m);
        if(CompareY()(nym,ym)){
          swap(seg[i],L);
          swap(yr,nyr);
        }
        i = i << 1 | CompareY()(nyr,yr);
      }
    }
  public:
    Static_LiChaoTree(vector<X> x){
      assert(!x.empty());
      sort(x.begin(), x.end(),CompareX());
      x.erase(unique(x.begin(), x.end()), x.end());
      n = x.size();
      b = 1; while(b < n) b<<=1;
      xl.resize(b<<1);
      xr.resize(b<<1);
      for(int i = 0; i < n; i++) xl[b+i] = xr[b+i] = x[i];
      for(int i = n; i < b; i++) xl[b+i] = xr[b+i] = x[n-1];
      for(int i = b-1; i > 0; i--) xl[i] = xl[i<<1], xr[i] = xr[i<<1 | 1];
      seg.assign(b<<1, INF());
    }
    Y get_min(X x){
        Y res = seg[1].get(x);
        for(int i = 1; i < b; res = min(res, seg[i = i<<1 | CompareX()(xr[i<<1],x)].get(x), CompareY()));
        return res;
    }
    void add_Line(Line L){_add_Line(L);}
    void add_Segment(Line L, X l, X r){_add_Segment(L,l,r);}

};
template<class T, T INF>
using StaticLiChaoTree = Static_LiChaoTree<T,T,Line<T>,INF_Line<T,INF>>;

}  // namespace soryuusi
