// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_segtree_2d.hpp
#pragma once
#include <utility>

namespace soryuusi {

using namespace std;

template<class S, S (*op)(S,S), S (*e)()>
struct Dynamic_Segtree2D{
  private:
    int h, w;
    struct node1D{
      int pos;
      S val, sum;
      node1D* lp;
      node1D* rp;
      node1D(int pos_, S val_) : pos(pos_), val(val_), sum(val), lp(nullptr), rp(nullptr) {}
      void calc(){sum = op(lp ? lp->sum : e(), op(val, rp ? rp->sum : e()));}
    };
    void _add1D(node1D*& p, int l, int r, int i, S x){
      if(!p) {
        p = new node1D(i, x);
        return;
      }
      p->sum = op(p->sum, x);
      if(p->pos == i) {
        p->val = op(p->val, x);
        return;
      }
      int m = (l+r) >> 1;
      if(i < m) {
        if(i > p->pos) swap(i, p->pos), swap(x, p->val);
        _add1D(p->lp, l, m, i, x);
      }
      else {
        if(i < p->pos) swap(i, p->pos), swap(x, p->val);
        _add1D(p->rp, m, r, i, x);
      }
    }

    void _set1D(node1D*& p, int l, int r, int i, S x){
      if(!p) {
        p = new node1D(i, x);
        return;
      }
      if(p->pos == i) {
        p->val = x;
        p->calc();
        return;
      }
      int m = (l+r) >> 1;
      if(i < m) {
        if(i > p->pos) swap(i, p->pos), swap(x, p->val);
        _set1D(p->lp, l, m, i, x);
      }
      else {
        if(i < p->pos) swap(i, p->pos), swap(x, p->val);
        _set1D(p->rp, m, r, i, x);
      }
      p->calc();
    }

    S _get1D(node1D* p, int l, int r, int i){
      if(!p) return e();
      if(p->pos == i) return p->val;
      int m = (l+r) >> 1;
      if(i < m) return _get1D(p->lp, l, m, i);
      else return _get1D(p->rp, m, r, i);
    }

    S _prod1D(node1D* p, int l, int r, int i, int j){
      if(!p || r <= i || j <= l) return e();
      if(i <= l && r <= j) return p->sum;
      int m = (l+r) >> 1;
      S res = _prod1D(p->lp, l, m, i, j);
      if(i <= p->pos && p->pos < j) res = op(res,p->val);
      return op(res,_prod1D(p->rp, m, r, i, j));
    }

    struct node2D{
      node1D* tree;
      node2D* lp;
      node2D* rp;
      node2D() : tree(nullptr), lp(nullptr), rp(nullptr) {}
    };
    node2D* root;
    void calc(node2D* p, int j){
      if(!p) return;
      _set1D(p->tree, 0, w, j, op(
        p->lp ? _get1D(p->lp->tree,0,w,j) : e(), 
        p->rp ? _get1D(p->rp->tree,0,w,j) : e()
      ));
    }
    void _add2D(node2D*& p, int l, int r, int i, int j, S x){
      if(!p) p = new node2D();
      _add1D(p->tree,0,w,j,x);
      if(r - l == 1) return;
      int m = (l+r) >> 1;
      if(i < m) _add2D(p->lp, l, m, i, j, x);
      else _add2D(p->rp, m, r, i, j, x);
    }

    void _set2D(node2D*& p, int l, int r, int i, int j, S x){
      if(!p) p = new node2D();
      if(r - l == 1) {
        _set1D(p->tree, 0, w, j, x);
        return;
      }
      int m = (l+r) >> 1;
      if(i < m) _set2D(p->lp, l, m, i, j, x);
      else _set2D(p->rp, m, r, i, j, x);
      calc(p,j);
    }

    S _get2D(node2D*& p, int l, int r, int i, int j){
      if(!p) return e();
      if(r - l == 1) return _get1D(p->tree,0,w,j);
      int m = (l+r) >> 1;
      if(i < m) return _get2D(p->lp, l, m, i, j);
      else return _get2D(p->rp, m, r, i, j);
    }
    S _prod2D(node2D*& p, int l, int r, int hi, int hj, int wi, int wj){
      if(!p || r <= hi || hj <= l) return e();
      if(hi <= l && r <= hj) return _prod1D(p->tree, 0, w, wi, wj);
      int m = (l+r) >> 1;
      return op(_prod2D(p->lp, l, m, hi, hj, wi, wj), _prod2D(p->rp, m, r, hi, hj, wi, wj));
    }
  public:
    Dynamic_Segtree2D(int h_, int w_) : h(h_), w(w_), root(nullptr){}
    void set(int i, int j, S x){_set2D(root,0,h,i,j,x);}
    void add(int i, int j, S x){_add2D(root,0,h,i,j,x);}
    S get(int i, int j){return _get2D(root,0,h,i,j);}
    S prod(int hl, int hr, int wl, int wr){return _prod2D(root,0,h,hl,hr,wl,wr);}
    S all_prod(){return (root && root->tree ? root->tree->sum : e());}
};

}  // namespace soryuusi
