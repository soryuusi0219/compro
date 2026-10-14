// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_rectangle_sum.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, class X = int, class Y = int>
struct StaticRectangleSum{
  private:
    struct Query{X x; int idx; Y yl, yr; S w;};
    vector<Query> query;
    vector<Y> vy;
    int q = 0;
  public:
    StaticRectangleSum() {}
    StaticRectangleSum(int n, int q_) {query.reserve(n+q_*2), vy.reserve(n);}
    void add_point(X x, Y y, S w){
      query.emplace_back(x,-1,y,y,w);
      vy.push_back(y);
    }
    void add_query(X xl, X xr, Y yl, Y yr){
      query.emplace_back(xl,q << 1,yl,yr,0);
      query.emplace_back(xr,q << 1 | 1,yl,yr,0);
      ++q;
    }
    vector<S> solve(){
      sort(query.begin(),query.end(),[](const Query& l, const Query& r){
        return l.x == r.x ? l.idx > r.idx : l.x < r.x;  
      });
      sort(vy.begin(),vy.end());
      vy.erase(unique(vy.begin(),vy.end()),vy.end());
      int m = vy.size();
      vector<S> s(m+1, 0), ans(q,0);
      for(const auto& [x, idx, yl, yr, w] : query){
        if(~idx){
          int qid = idx >> 1;
          bool f = idx & 1;
          int pl = lower_bound(vy.begin(),vy.end(),yl) - vy.begin();
          int pr = lower_bound(vy.begin(),vy.end(),yr) - vy.begin();
          if(f){
            for(; pl; pl -= pl & -pl) ans[qid] -= s[pl];
            for(; pr; pr -= pr & -pr) ans[qid] += s[pr];
          }else{
            for(; pl; pl -= pl & -pl) ans[qid] += s[pl];
            for(; pr; pr -= pr & -pr) ans[qid] -= s[pr];
          }
        }else{
          int p = lower_bound(vy.begin(),vy.end(),yl) - vy.begin();
          for(++p; p <= m; p += p & -p) s[p] += w;
        }
      }
      return ans;
    }
};

}  // namespace soryuusi
