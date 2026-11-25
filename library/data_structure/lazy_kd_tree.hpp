// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/lazy_kd_tree.hpp
#pragma once
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/kd_tree_builder.hpp"
#include "internal/segtree_ops.hpp"

namespace soryuusi {

using namespace std;

template<class X, class Y, class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct LazyKDTree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  private:
    using Point = pair<X,Y>;
    struct node{
      Point s,t;
      S val;
      node() : val(e()) {}
      node(Point s_, Point t_, S val_) : s(s_), t(t_), val(val_) {}
    };
    vector<node> data;
    static constexpr bool scalar_tag = is_arithmetic_v<F>;
    using Tags = conditional_t<scalar_tag,vector<F>,internal::SegtreeTags<F,composition,id>>;
    Tags lz;
    void upd(int i){data[i].val = op(data[i << 1].val, data[i << 1 | 1].val);}
    void act(int i, F f) {
      data[i].val = mapping(f,data[i].val);
      if(i >= n) return;
      if constexpr(scalar_tag) lz[i] = composition(f,lz[i]);
      else lz.apply(i,f);
    }
    void push(int i) {
      if constexpr(scalar_tag) {
        if(lz[i] == F(id())) return;
        act(i<<1,lz[i]);
        act(i<<1|1,lz[i]);
        lz[i] = id();
      } else {
        lz.push(i,[&](int k,auto&& f){ act(k,f); });
      }
    }

    int n = 0;
    vector<Point> pts;
    vector<int> ref;

  public:
    LazyKDTree(){}
    LazyKDTree(int n_){pts.reserve(n_);}
    LazyKDTree(const vector<Point>& pts_) : pts(pts_) {}
    int add_point(X x, Y y){
      pts.emplace_back(x,y);
      return pts.size()-1;
    }
    
    void build(){build(vector<S>(pts.size(),e()));}
    void build(const vector<S>& v){
      n = pts.size();
      data.resize(size_t(n)*2);
      if constexpr(scalar_tag) lz.assign(n,id());
      else lz = Tags(n);
      ref.resize(n);
      internal::KDTreeBuilder<X,Y>(pts).build([&](int i,int p){
        data[i] = node{pts[p],pts[p],v[p]};
        ref[p] = i;
      },[&](int i){
        const auto& left = data[i<<1];
        const auto& right = data[i<<1|1];
        data[i].s = {min(left.s.first,right.s.first),min(left.s.second,right.s.second)};
        data[i].t = {max(left.t.first,right.t.first),max(left.t.second,right.t.second)};
        upd(i);
      });
    }

    void set(int i, S x){
      for(int k = soryuusi::bit_width(static_cast<unsigned>(i = ref[i]))-1; k > 0; --k) push(i >> k);
      for(data[i].val = x; i >>= 1;) upd(i);
    }

    S get(int i){
      for(int k = soryuusi::bit_width(static_cast<unsigned>(i = ref[i]))-1; k > 0; --k) push(i >> k);
      return data[i].val;
    }

    S prod(X xmn, X xmx, Y ymn, Y ymx){
      if(!n || !(xmn < xmx) || !(ymn < ymx)) return e();
      auto rec = [&](int i, auto self) -> S {
        if(data[i].t.first < xmn || xmx <= data[i].s.first || data[i].t.second < ymn || ymx <= data[i].s.second) return e();
        if(xmn <= data[i].s.first && data[i].t.first < xmx && ymn <= data[i].s.second && data[i].t.second < ymx) return data[i].val;
        push(i);
        return op(self(i << 1, self), self(i << 1 | 1, self));
      };
      return rec(1, rec);
    }

    void apply(X xmn, X xmx, Y ymn, Y ymx, F f){
      if(!n || !(xmn < xmx) || !(ymn < ymx)) return;
      auto rec = [&](int i, auto self) -> void {
        if(data[i].t.first < xmn || xmx <= data[i].s.first || data[i].t.second < ymn || ymx <= data[i].s.second) return;
        if(xmn <= data[i].s.first && data[i].t.first < xmx && ymn <= data[i].s.second && data[i].t.second < ymx) {
          act(i,f);
          return;
        }
        push(i);
        self(i << 1, self), self(i << 1 | 1, self);
        upd(i);
      };
      rec(1, rec);
    }
    S all_prod(){return n ? data[1].val : e();}
    S all_apply(F f){
      if(!n) return e();
      act(1,f);
      return data[1].val;
    }
};

}  // namespace soryuusi
