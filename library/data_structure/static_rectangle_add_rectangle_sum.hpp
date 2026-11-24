// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_rectangle_add_rectangle_sum.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, class X = int, class Y = int>
struct StaticRectangleAddRectangleSum{
  private:
    struct Rectangle { X xl, xr; Y yl, yr; };
    struct Update { Rectangle bounds; S w; };
    struct Event { X x; int index; };
    struct Range { int l, r; };
    struct Coefficients { S xy, y, x, constant; };
    vector<Update> updates;
    vector<Rectangle> queries;
    vector<Y> ys;
  public:
    StaticRectangleAddRectangleSum() {}
    StaticRectangleAddRectangleSum(int n, int q) {updates.reserve(n); queries.reserve(q); ys.reserve(n*2);}
    void add_rectangle(X xl, X xr, Y yl, Y yr, S w){
      if(!(xl < xr) || !(yl < yr)) return;
      ys.push_back(yl);
      ys.push_back(yr);
      updates.push_back({{xl,xr,yl,yr},w});
    }
    void add_query(X xl, X xr, Y yl, Y yr){
      queries.push_back({xl,xr,yl,yr});
    }
    vector<S> solve(){
      vector<S> ans(queries.size(),0);
      if(updates.empty() || queries.empty()) return ans;
      sort(ys.begin(),ys.end());
      ys.erase(unique(ys.begin(),ys.end()),ys.end());
      vector<Event> events;
      vector<Range> update_y, query_y;
      events.reserve((updates.size()+queries.size())*2);
      update_y.reserve(updates.size());
      query_y.reserve(queries.size());
      auto compress = [&](const Rectangle& rect){
        int l = lower_bound(ys.begin(),ys.end(),rect.yl)-ys.begin();
        int r = lower_bound(ys.begin(),ys.end(),rect.yr)-ys.begin();
        return Range{l,r};
      };
      for(int i = 0; i < static_cast<int>(updates.size()); ++i) {
        const auto& rect = updates[i].bounds;
        update_y.push_back(compress(rect));
        events.push_back({rect.xl,~(i*2)});
        events.push_back({rect.xr,~(i*2+1)});
      }
      for(int i = 0; i < static_cast<int>(queries.size()); ++i) {
        const auto& rect = queries[i];
        query_y.push_back(compress(rect));
        if(rect.xl < rect.xr && rect.yl < rect.yr) {
          events.push_back({rect.xl,i*2});
          events.push_back({rect.xr,i*2+1});
        }
      }
      sort(events.begin(),events.end(),[](const Event& l,const Event& r){ return l.x < r.x; });
      const int m = ys.size();
      vector<Coefficients> tree(m+1,{0,0,0,0});
      auto add = [&](int p, S w, const X& x, const Y& y) {
        const S wx = w*x, wy = w*y, wxy = wx*y;
        for(++p; p <= m; p += p & -p) {
          auto& c = tree[p];
          c.xy += w;
          c.y += wx;
          c.x += wy;
          c.constant += wxy;
        }
      };
      auto evaluate = [&](int p, const X& x, const Y& y) -> S {
        Coefficients sum{0,0,0,0};
        for(; p; p -= p & -p) {
          const auto& c = tree[p];
          sum.xy += c.xy;
          sum.y += c.y;
          sum.x += c.x;
          sum.constant += c.constant;
        }
        return (sum.xy*y - sum.x)*x - sum.y*y + sum.constant;
      };
      for(const auto& [x,index] : events) {
        if(index < 0) {
          int key = ~index;
          const auto& update = updates[key>>1];
          auto [l,r] = update_y[key>>1];
          S w = (key & 1) ? -update.w : update.w;
          add(l,w,x,update.bounds.yl);
          add(r,-w,x,update.bounds.yr);
        } else {
          int q = index>>1;
          auto [l,r] = query_y[q];
          S value = evaluate(r,x,queries[q].yr)-evaluate(l,x,queries[q].yl);
          if(index & 1) ans[q] += value;
          else ans[q] -= value;
        }
      }
      return ans;
    }
};

}  // namespace soryuusi
