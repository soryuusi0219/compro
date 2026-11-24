// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/li_chao_tree_dynamic.hpp
#pragma once
#include <cstddef>
#include <utility>
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <type_traits>

#include "li_chao_tree_line.hpp"

#include "internal/node_pool.hpp"

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
    internal::NodePool<node> pool;
    node* root;
    static X middle(X l, X r){
      if constexpr(is_arithmetic_v<X> && !is_same_v<X,bool>) return std::midpoint(l,r);
      else return (l+r)/2;
    }
    static bool can_split(X l, X r){
      if constexpr(is_integral_v<X> && !is_same_v<X,bool>){
        using U = make_unsigned_t<X>;
        return static_cast<U>(U(r)-U(l)) > std::max(U(EPS),U(1));
      } else {
        return r-l > EPS;
      }
    }
    void _add_Segment(node*& p, X l, X r, const Line& line, X ql, X qr){
      if(!CompareX()(l,qr) || !CompareX()(ql,r)) return;
      if(!CompareX()(l,ql) && !CompareX()(qr,r)){
        _add_Line(p,l,r,line);
        return;
      }
      if(!can_split(l,r)) return;
      X m = middle(l,r);
      if constexpr(is_floating_point_v<X>) if(!(l < m && m < r)) return;
      if(!p) p = pool.create(INF());
      if(CompareX()(ql,m)) _add_Segment(p->lp,l,m,line,ql,qr);
      if(CompareX()(m,qr)) _add_Segment(p->rp,m,r,line,ql,qr);
    }
    void _add_Line(node*& p, X l, X r, Line line){
      node** link = &p;
      while(*link){
        node* current = *link;
        Y old_left = current->L.get(l), new_left = line.get(l);
        bool left = CompareY()(new_left,old_left);
        if(!can_split(l,r) || left == CompareY()(line.get(r),current->L.get(r))){
          if(left) current->L = std::move(line);
          return;
        }
        X m = middle(l,r);
        if constexpr(is_floating_point_v<X>){
          if(!(l < m && m < r)){
            if(left) current->L = std::move(line);
            return;
          }
        }
        if(CompareY()(line.get(m),current->L.get(m))){
          swap(current->L,line);
          swap(old_left,new_left);
        }
        if(CompareY()(new_left,old_left)) link = &current->lp, r = m;
        else link = &current->rp, l = m;
      }
      *link = pool.create(line);
    }
  public:
    Dynamic_LiChaoTree(const Dynamic_LiChaoTree&) = delete;
    Dynamic_LiChaoTree& operator=(const Dynamic_LiChaoTree&) = delete;
    Dynamic_LiChaoTree(Dynamic_LiChaoTree&& other) : Dynamic_LiChaoTree() {*this = std::move(other);}
    Dynamic_LiChaoTree& operator=(Dynamic_LiChaoTree&& other) {
      if(this == &other) return *this;
      pool = std::move(other.pool);
      root = std::exchange(other.root, nullptr);
      return *this;
    }
    [[nodiscard]] Dynamic_LiChaoTree clone() const {
      Dynamic_LiChaoTree result;
      auto dfs = [&](auto&& self, node* p) -> node* {
        if(!p) return nullptr;
        node* q = result.pool.create(*p);
        q->lp = self(self,p->lp); q->rp = self(self,p->rp);
        return q;
      };
      result.root = dfs(dfs,root);
      return result;
    }
    std::size_t allocated_nodes() const {return pool.size();}
    Dynamic_LiChaoTree() : root(nullptr){}
    void add_line(Line L){add_Line(L);}
    void add_segment(Line L, X l, X r){add_Segment(L,l,r);}
    void merge(Dynamic_LiChaoTree&& other){
      assert(this != &other);
      if(!root){
        *this = std::move(other);
        return;
      }
      auto dfs = [&](auto&& self, node* p, X l, X r) -> void {
        if(!p) return;
        add_Segment(p->L,l,r);
        X m = middle(l,r);
        if(p->lp) self(self,p->lp,l,m);
        if(p->rp) self(self,p->rp,m,r);
      };
      dfs(dfs,other.root,XMIN,XMAX);
      other.root = nullptr;
    }
    void add_Line(Line L){_add_Line(root, XMIN, XMAX, L);}
    void add_Segment(Line L, X l, X r){if(CompareX()(l,r)) _add_Segment(root,XMIN,XMAX,L,l,r);}
    Y get_min(X x){
      if(!root) return INF().get(x);
      node* p = root;
      X l = XMIN, r = XMAX;
      Y result = p->L.get(x);
      while(can_split(l,r)){
        X m = middle(l,r);
        if(CompareX()(x,m)) p = p->lp, r = m;
        else p = p->rp, l = m;
        if(!p) break;
        Y value = p->L.get(x);
        if(CompareY()(value,result)) result = value;
      }
      return result;
    }

};
template<class T, T XMIN, T XMAX, T EPS, T INF>
using DynamicLiChaoTree = Dynamic_LiChaoTree<T,T,Line<T>,INF_Line<T,INF>,XMIN,XMAX,EPS>;

}  // namespace soryuusi
