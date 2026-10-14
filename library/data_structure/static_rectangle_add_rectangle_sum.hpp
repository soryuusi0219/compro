// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_rectangle_add_rectangle_sum.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, class X = int, class Y = int>
struct StaticRectangleAddRectangleSum{
  private:
    struct Query{X x; int idx; Y yl, yr; S w;};
    vector<Query> query;
    vector<Y> vy;
    int q = 0;
  public:
    StaticRectangleAddRectangleSum() {}
    StaticRectangleAddRectangleSum(int n, int q_) {query.reserve(n*2+q_*2), vy.reserve(n*2);}
    void add_rectangle(X xl, X xr, Y yl, Y yr, S w){
      query.emplace_back(xl,-1,yl,yr,-w);
      query.emplace_back(xr,-1,yl,yr,w);
      vy.push_back(yl);
      vy.push_back(yr);
    }
    void add_query(X xl, X xr, Y yl, Y yr){
      query.emplace_back(xl, q << 1, yl, yr, 0);
      query.emplace_back(xr, q << 1 | 1, yl, yr, 0);
      ++q;
    }
    vector<S> solve(){
      sort(query.begin(),query.end(),[](const Query& l, const Query& r){return l.x < r.x;});
      sort(vy.begin(),vy.end());
      vy.erase(unique(vy.begin(),vy.end()),vy.end());
      int m = vy.size();
      vector<S> s(m+1, 0), sx(m+1, 0), sy(m+1, 0), sxy(m+1, 0), ans(q,0);
      for(const auto& [x, idx, yl, yr, w] : query){
        if(~idx){
          int qid = idx >> 1;
          bool f = idx & 1;
          int pl = lower_bound(vy.begin(),vy.end(),yl) - vy.begin();
          int pr = lower_bound(vy.begin(),vy.end(),yr) - vy.begin();
          if(f){
            for(; pl; pl -= pl & -pl) ans[qid] -= (s[pl]*yl-sy[pl])*x - sx[pl]*yl + sxy[pl];
            for(; pr; pr -= pr & -pr) ans[qid] += (s[pr]*yr-sy[pr])*x - sx[pr]*yr + sxy[pr];
          }else{
            for(; pl; pl -= pl & -pl) ans[qid] += (s[pl]*yl-sy[pl])*x - sx[pl]*yl + sxy[pl];
            for(; pr; pr -= pr & -pr) ans[qid] -= (s[pr]*yr-sy[pr])*x - sx[pr]*yr + sxy[pr];
          }
        }else{
          int pl = lower_bound(vy.begin(),vy.end(),yl) - vy.begin();
          int pr = lower_bound(vy.begin(),vy.end(),yr) - vy.begin();
          for(++pl; pl <= m; pl += pl & -pl) s[pl] -= w, sx[pl] -= w*x, sy[pl] -= w*yl, sxy[pl] -= w*x*yl;
          for(++pr; pr <= m; pr += pr & -pr) s[pr] += w, sx[pr] += w*x, sy[pr] += w*yr, sxy[pr] += w*x*yr;
        }
      }
      return ans;
    }
};

}  // namespace soryuusi
