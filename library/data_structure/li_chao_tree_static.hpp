// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/li_chao_tree_static.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "li_chao_tree_line.hpp"

namespace soryuusi {

using namespace std;

template<class X, class Y, class Line, auto INF, class CompareX = less<X>, class CompareY = less<Y>>
struct Static_LiChaoTree{
  static_assert(is_invocable_r_v<Line, decltype(INF)>, "INF must be callable with no arguments and return a value convertible to Line");

  private:
    int n, b;
    vector<X> coordinates;
    vector<Line> seg;

    void add_at(Line line, int i, int l, int r){
      while(true){
        const X& x = coordinates[l];
        Y old_left = seg[i].get(x), new_left = line.get(x);
        const bool left = CompareY()(new_left,old_left);
        if(r-l == 1){
          if(left) seg[i] = std::move(line);
          return;
        }
        const X& y = coordinates[r-1];
        Y old_right = seg[i].get(y), new_right = line.get(y);
        if(left == CompareY()(new_right,old_right)){
          if(left) seg[i] = std::move(line);
          return;
        }
        int m = (l+r)/2;
        const X& middle = coordinates[m-1];
        if(CompareY()(line.get(middle),seg[i].get(middle))){
          swap(seg[i],line);
          swap(old_right,new_right);
        }
        if(CompareY()(new_right,old_right)) i = i<<1|1, l = m;
        else i <<= 1, r = m;
      }
    }
    void add_segment(const Line& line, const X& ql, const X& qr, int i, int l, int r){
      if(!CompareX()(coordinates[l],ql) && CompareX()(coordinates[r-1],qr)){
        add_at(line,i,l,r);
        return;
      }
      if(r-l == 1) return;
      int m = (l+r)/2;
      if(!CompareX()(coordinates[m-1],ql)) add_segment(line,ql,qr,i<<1,l,m);
      if(CompareX()(coordinates[m],qr)) add_segment(line,ql,qr,i<<1|1,m,r);
    }

  public:
    Static_LiChaoTree(vector<X> x) : coordinates(std::move(x)) {
      sort(coordinates.begin(),coordinates.end(),CompareX());
      coordinates.erase(unique(coordinates.begin(),coordinates.end(),[](const X& a,const X& b){
        return !CompareX()(a,b) && !CompareX()(b,a);
      }),coordinates.end());
      n = coordinates.size();
      b = soryuusi::bit_ceil(static_cast<unsigned>(n));
      if(n) coordinates.resize(b,coordinates.back());
      seg.assign(b*2,INF());
    }
    Y get_min(X x){
      Y result = seg[1].get(x);
      if(!n) return result;
      for(int i = 1, l = 0, r = b; i < b;){
        int m = (l+r)/2;
        if(CompareX()(coordinates[m-1],x)) i = i<<1|1, l = m;
        else i <<= 1, r = m;
        Y value = seg[i].get(x);
        if(CompareY()(value,result)) result = value;
      }
      return result;
    }
    void add_Line(Line L){if(n) add_at(L,1,0,b);}
    void add_Segment(Line L, X l, X r){
      if(n && CompareX()(l,r) && CompareX()(coordinates.front(),r) && !CompareX()(coordinates.back(),l)) add_segment(L,l,r,1,0,b);
    }

};
template<class T, T INF>
using StaticLiChaoTree = Static_LiChaoTree<T,T,Line<T>,INF_Line<T,INF>>;

}  // namespace soryuusi
