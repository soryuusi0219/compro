// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/kd_tree.hpp
#pragma once
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class X, class Y, class S, S (*op)(S,S), S (*e)()>
struct KDTree{
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

    static bool comp_x(const Point &p1, const Point &p2) { return p1.first < p2.first; }
    static bool comp_y(const Point &p1, const Point &p2) { return p1.second < p2.second; }
    static Point maxp(const Point &p1, const Point &p2) { return {max(p1.first, p2.first), max(p1.second, p2.second)}; }
    static Point minp(const Point &p1, const Point &p2) { return {min(p1.first, p2.first), min(p1.second, p2.second)}; }

    int n;
    int b;
    vector<Point> pts;
    vector<int> ref;
    node* root;

    template<bool xy>
    void _build_rec(int l, int r, int id, const vector<S>& v, vector<int>& ids){
      if(r - l <= 1) {
        data[id] = node{pts[ids[l]],pts[ids[l]],v[ids[l]]};
        ref[ids[l]] = id;
        return;
      }
      int m = (l+r) >> 1;
      nth_element(ids.begin()+l,ids.begin()+m,ids.begin()+r,[&](int i, int j){
        if constexpr (xy) return pts[i].first < pts[j].first;
        else return pts[i].second < pts[j].second;
      });
      _build_rec<!xy>(l,m,id << 1, v, ids);
      _build_rec<!xy>(m,r,id << 1 | 1, v, ids);
      data[id].val = op(data[id << 1].val,data[id << 1 | 1].val);
      data[id].s = minp(data[id << 1].s,data[id << 1 | 1].s);
      data[id].t = maxp(data[id << 1].t,data[id << 1 | 1].t);
    }

  public:
    KDTree(){}
    KDTree(int n_){pts.reserve(n_);}
    KDTree(const vector<Point>& pts_) : pts(pts_) {}
    int add_point(X x, Y y, S v = e()){
      pts.emplace_back(x,y);
      return pts.size()-1;
    }
    
    void build(){build(vector<S>(pts.size(),e()));}
    void build(const vector<S>& v){
      n = pts.size();
      b = 1; while(b < n) b <<= 1;
      data.resize(b*2);
      ref.resize(n);
      vector<int> ids(n);
      iota(ids.begin(),ids.end(),0);
      _build_rec<true>(0,n,1,v,ids);
    }

    void set(int i, S x){for(data[i = ref[i]].val = x; i >>= 1;)  _update(i);}
    S get(int i) const {return data[ref[i]].val;}

    S prod(X xmn, X xmx, Y ymn, Y ymx){
      auto rec = [&](int i, auto self) -> S {
        if(data[i].t.first < xmn || xmx <= data[i].s.first || data[i].t.second < ymn || ymx <= data[i].s.second) return e();
        if(xmn <= data[i].s.first && data[i].t.first < xmx && ymn <= data[i].s.second && data[i].t.second < ymx) return data[i].val;
        return op(self(i << 1, self), self(i << 1 | 1, self));
      };
      return rec(1, rec);
    }
};

}  // namespace soryuusi
