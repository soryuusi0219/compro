// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/kd_tree.hpp
#pragma once
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include "internal/kd_tree_builder.hpp"

namespace soryuusi {

using namespace std;

template<class X, class Y, class S, auto op, auto e>
struct KDTree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  private:
    using Point = pair<X,Y>;
    struct node{
      Point s,t;
      S val;
      node() : val(e()) {}
      node(Point s_, Point t_, S val_) : s(s_), t(t_), val(val_) {}
    };
    vector<node> data;
    void _update(int i){data[i].val = op(data[i << 1].val,data[i << 1 | 1].val);}

    int n = 0;
    vector<Point> pts;
    vector<int> ref;
    vector<S> initial;

  public:
    KDTree(){}
    KDTree(int n_){pts.reserve(n_);}
    KDTree(const vector<Point>& pts_) : pts(pts_) {}
    int add_point(X x, Y y, S v = e()){
      if(initial.size() != pts.size()) initial.resize(pts.size(),e());
      initial.push_back(v);
      pts.emplace_back(x,y);
      return pts.size()-1;
    }
    
    void build(){
      if(initial.empty()) build(vector<S>(pts.size(),e()));
      else build(initial);
    }
    void build(const vector<S>& v){
      n = pts.size();
      data.resize(size_t(n)*2);
      ref.resize(n);
      internal::KDTreeBuilder<X,Y>(pts).build([&](int i,int p){
        data[i] = node{pts[p],pts[p],v[p]};
        ref[p] = i;
      },[&](int i){
        const auto& left = data[i<<1];
        const auto& right = data[i<<1|1];
        data[i].s = {min(left.s.first,right.s.first),min(left.s.second,right.s.second)};
        data[i].t = {max(left.t.first,right.t.first),max(left.t.second,right.t.second)};
        _update(i);
      });
    }

    void set(int i, S x){for(data[i = ref[i]].val = x; i >>= 1;)  _update(i);}
    S get(int i) const {return data[ref[i]].val;}

    S prod(X xmn, X xmx, Y ymn, Y ymx){
      if(!n || !(xmn < xmx) || !(ymn < ymx)) return e();
      auto rec = [&](int i, auto self) -> S {
        if(data[i].t.first < xmn || xmx <= data[i].s.first || data[i].t.second < ymn || ymx <= data[i].s.second) return e();
        if(xmn <= data[i].s.first && data[i].t.first < xmx && ymn <= data[i].s.second && data[i].t.second < ymx) return data[i].val;
        return op(self(i << 1, self), self(i << 1 | 1, self));
      };
      return rec(1, rec);
    }
};

}  // namespace soryuusi
