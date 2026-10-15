// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/li_chao_tree_dynamic.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/li_chao_tree_line.hpp"

namespace soryuusi {

using namespace std;

template<class X, class Y, class Line, auto INF, X XMIN, X XMAX, X EPS, class CompareX = less<X>, class CompareY = less<Y>>
struct Dynamic_LiChaoTree{
  static_assert(is_invocable_r_v<Line, decltype(INF)>, "INF must be callable with no arguments and return a value convertible to Line");

  private:
    struct node{
      Line L;
      node* lp;
      node* rp;
      node(Line L_) : L(L_), lp(nullptr), rp(nullptr) {}
    };
    node* root;
    vector<Line> seg;
    void _add_Segment(node*& p, X l, X r, Line L, X ql, X qr){
      if(!CompareX()(l,qr) || !CompareX()(ql,r)) return;
      if(!CompareX()(l,ql) && !CompareX()(qr,r)) {
        _add_Line(p, l, r, L);
        return;
      }
      if(r-l > EPS){
        X m = (l+r) / 2;
        if(!p) p = new node(INF());
        _add_Segment(p->lp, l, m, L, ql, qr);
        _add_Segment(p->rp, m, r, L, ql, qr);
      }
    }
    void _add_Line(node*& p, X l, X r, Line L){
      if(!p) {
        p = new node(L);
        return;
      }
      Y yl = p->L.get(l);
      Y yr = p->L.get(r);
      Y nyl = L.get(l);
      Y nyr = L.get(r);
      if(r-l <= EPS || CompareY()(nyl,yl) == CompareY()(nyr,yr)){
        if(CompareY()(nyl,yl)) swap(p->L,L);
        return;
      }
      const X m = (l+r) / 2;
      Y ym = p->L.get(m);
      Y nym = L.get(m);
      if(CompareY()(nym,ym)){
        swap(p->L,L);
        swap(yl,nyl);
      }
      if(CompareY()(nyl,yl)) _add_Line(p->lp, l, m, L);
      else _add_Line(p->rp, m, r, L);
    }
    Y _get_min(node*& p, X l, X r, X x){
      if(!p) return INF().get(x);
      Y res = p->L.get(x);
      if(r-l <= EPS) return res;
      X m = (l+r) / 2;
      if(x < m) return min(res, _get_min(p->lp,l,m,x),CompareY());
      else return min(res, _get_min(p->rp,m,r,x),CompareY());
    }
  public:
    Dynamic_LiChaoTree() : root(nullptr){}
    void add_Line(Line L){_add_Line(root, XMIN, XMAX, L);}
    void add_Segment(Line L, X l, X r){_add_Segment(root, XMIN, XMAX, L, l, r);}
    Y get_min(X x){return _get_min(root, XMIN, XMAX, x);}

};
template<class T, T XMIN, T XMAX, T EPS, T INF>
using DynamicLiChaoTree = Dynamic_LiChaoTree<T,T,Line<T>,INF_Line<T,INF>,XMIN,XMAX,EPS>;

}  // namespace soryuusi
