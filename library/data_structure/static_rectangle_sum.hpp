// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_rectangle_sum.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, class X = int, class Y = int>
struct StaticRectangleSum{
  private:
    struct Point { X x; Y y; S w; };
    struct Query { X xl, xr; Y yl, yr; };
    struct Event { X x; int index; };
    struct Range { int l, r; };
    vector<Point> points;
    vector<Query> queries;
    vector<Y> ys;
  public:
    StaticRectangleSum() {}
    StaticRectangleSum(int n, int q) {points.reserve(n); queries.reserve(q); ys.reserve(n);}
    void add_point(X x, Y y, S w){
      ys.push_back(y);
      points.push_back({x,y,w});
    }
    void add_query(X xl, X xr, Y yl, Y yr){
      queries.push_back({xl,xr,yl,yr});
    }
    vector<S> solve(){
      vector<S> ans(queries.size(),0);
      if(points.empty() || queries.empty()) return ans;
      sort(points.begin(),points.end(),[](const Point& l,const Point& r){ return l.x < r.x; });
      sort(ys.begin(),ys.end());
      ys.erase(unique(ys.begin(),ys.end()),ys.end());
      vector<Range> ranges;
      vector<Event> events;
      ranges.reserve(queries.size());
      events.reserve(queries.size()*2);
      for(int i = 0; i < static_cast<int>(queries.size()); ++i) {
        const auto& [xl,xr,yl,yr] = queries[i];
        int l = lower_bound(ys.begin(),ys.end(),yl)-ys.begin();
        int r = lower_bound(ys.begin(),ys.end(),yr)-ys.begin();
        ranges.push_back({l,r});
        if(l != r && xl < xr) {
          events.push_back({xl,i*2});
          events.push_back({xr,i*2+1});
        }
      }
      sort(events.begin(),events.end(),[](const Event& l,const Event& r){ return l.x < r.x; });
      const int n = points.size(), m = ys.size();
      vector<S> tree(m+1,0);
      int i = 0;
      for(const auto& [x,index] : events) {
        while(i < n && points[i].x < x) {
          const auto& point = points[i++];
          int p = lower_bound(ys.begin(),ys.end(),point.y)-ys.begin();
          for(++p; p <= m; p += p & -p) tree[p] += point.w;
        }
        auto [l,r] = ranges[index>>1];
        // Shared Fenwick ancestors cancel in a difference of two prefixes.
        S value = 0;
        while(l != r) {
          if(l < r) {
            value += tree[r];
            r -= r & -r;
          } else {
            value -= tree[l];
            l -= l & -l;
          }
        }
        if(index & 1) ans[index>>1] += value;
        else ans[index>>1] -= value;
      }
      return ans;
    }
};

}  // namespace soryuusi
