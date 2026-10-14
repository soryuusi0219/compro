// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/lazy_kd_tree.hpp
#pragma once
#include <algorithm>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class X, class Y, class S, S (*op)(S,S), S (*e)(), class F, S (*mapping)(F, S), F (*composition)(F, F), F (*id)()>
struct LazyKDTree{
  private:
    using Point = pair<X,Y>;
    struct node{
      Point s,t;
      S val;
      node() : val(e()) {}
      node(Point s_, Point t_, S val_) : s(s_), t(t_), val(val_) {}
    };
    vector<node> data;
    vector<F> lz;
    void upd(int i){data[i].val = op(data[i << 1].val, data[i << 1 | 1].val);}
    void act(int i, F f){data[i].val = mapping(f, data[i].val); if(i < b) lz[i] = composition(f,lz[i]);}
    void push(int i) {if(i < b && lz[i] != id()) {act(i << 1, lz[i]), act(i << 1 | 1, lz[i]); lz[i] = id();}}

    static bool comp_x(const Point &p1, const Point &p2) { return p1.first < p2.first; }
    static bool comp_y(const Point &p1, const Point &p2) { return p1.second < p2.second; }
    static Point maxp(const Point &p1, const Point &p2) { return {max(p1.first, p2.first), max(p1.second, p2.second)}; }
    static Point minp(const Point &p1, const Point &p2) { return {min(p1.first, p2.first), min(p1.second, p2.second)}; }

    int n, b;
    vector<Point> pts;
    vector<int> ref;
    node* root;

    template<bool xy>
    void _build_rec(int l, int r, int idx, const vector<S>& v, vector<int>& ids){
      if(r - l <= 1) {
        data[idx] = node{pts[ids[l]],pts[ids[l]],v[ids[l]]};
        ref[ids[l]] = idx;
        return;
      }
      int m = (l+r) >> 1;
      nth_element(ids.begin()+l,ids.begin()+m,ids.begin()+r,[&](int i, int j){
        if constexpr (xy) return pts[i].first < pts[j].first;
        else return pts[i].second < pts[j].second;
      });
      _build_rec<!xy>(l,m,idx << 1, v, ids);
      _build_rec<!xy>(m,r,idx << 1 | 1, v, ids);
      data[idx].val = op(data[idx << 1].val,data[idx << 1 | 1].val);
      data[idx].s = minp(data[idx << 1].s,data[idx << 1 | 1].s);
      data[idx].t = maxp(data[idx << 1].t,data[idx << 1 | 1].t);
    }

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
      b = 1; while(b < n) b <<= 1;
      data.resize(b*2);
      lz.assign(b, id());
      ref.resize(n);
      vector<int> ids(n);
      iota(ids.begin(),ids.end(),0);
      _build_rec<true>(0,n,1,v,ids);

      // for(auto [s,t,val] : data) cout << s << ", " << t << ", " << val << endl;
    }

    void set(int i, S x){
      for(int k = 31 - __builtin_clz(i = ref[i])-1; k; --k) push(i >> k); 
      for(data[i].val = x; i >>= 1;) upd(i);
    }

    S get(int i){
      for(int k =  31 - __builtin_clz(i = ref[i])-1; k; --k) push(i >> k); 
      return data[i].val;
    }

    S prod(X xmn, X xmx, Y ymn, Y ymx){
      auto rec = [&](int i, auto self) -> S {
        // cout << i << " " << data[i].s << " " << data[i].t << " " << data[i].val << endl;
        if(i < b) push(i);
        if(data[i].t.first < xmn || xmx <= data[i].s.first || data[i].t.second < ymn || ymx <= data[i].s.second) return e();
        if(xmn <= data[i].s.first && data[i].t.first < xmx && ymn <= data[i].s.second && data[i].t.second < ymx) return data[i].val;
        return op(self(i << 1, self), self(i << 1 | 1, self));
      };
      return rec(1, rec);
    }

    void apply(X xmn, X xmx, Y ymn, Y ymx, F f){
      auto rec = [&](int i, auto self) -> void {
        if(i < b) push(i);
        if(data[i].t.first < xmn || xmx <= data[i].s.first || data[i].t.second < ymn || ymx <= data[i].s.second) return;
        if(xmn <= data[i].s.first && data[i].t.first < xmx && ymn <= data[i].s.second && data[i].t.second < ymx) {
          act(i,f);
          return;
        }
        self(i << 1, self), self(i << 1 | 1, self);
        upd(i);
      };
      rec(1, rec);
    }
    S all_prod(){return data[1].val;}
    S all_apply(F f){return act(1,f);}
};

}  // namespace soryuusi
