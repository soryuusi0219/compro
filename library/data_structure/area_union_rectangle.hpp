// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/area_union_rectangle.hpp
#pragma once
#include <algorithm>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/segtree_ops.hpp"

namespace soryuusi {

using namespace std;

template<class T>
struct Area_Union_Rectangle{
  private:
    struct Rectangle { T l, r, d, u; };
    struct Event { T x; int index; };
    struct Range { int l, r; };
    vector<Rectangle> rectangles;

    struct CoverTree {
      int b, h;
      vector<int> count;
      vector<T> length, covered;

      explicit CoverTree(const vector<T>& y) : b(soryuusi::bit_ceil(static_cast<unsigned>(y.size()-1))), h(soryuusi::countr_zero(static_cast<unsigned>(b))), count(2*b), length(2*b,T(0)), covered(2*b,T(0)) {
        for(int i = 1; i < static_cast<int>(y.size()); ++i) length[b+i-1] = y[i]-y[i-1];
        for(int k = b-1; k; --k) length[k] = length[k<<1]+length[k<<1|1];
      }

      void pull(int k) {
        if(count[k]) covered[k] = length[k];
        else if(k < b) covered[k] = covered[k<<1]+covered[k<<1|1];
        else covered[k] = T(0);
      }

      void change(int l, int r, int delta) {
        l += b;
        r += b;
        internal::SegtreeBoundary boundary(l,r,h);
        auto update = [&](int k){ count[k] += delta; pull(k); };
        for(; l < r; l >>= 1, r >>= 1) {
          if(l & 1) update(l++);
          if(r & 1) update(--r);
        }
        boundary.pull([&](int k){ pull(k); });
      }
    };

  public:
    Area_Union_Rectangle(){}
    Area_Union_Rectangle(int q){rectangles.reserve(q);}
    void add(T l, T r, T d, T u){
      if(l < r && d < u) rectangles.push_back({l,r,d,u});
    }
    T area(){
      if(rectangles.empty()) return T(0);
      vector<T> y;
      y.reserve(rectangles.size()*2);
      for(const auto& rect : rectangles) {
        y.push_back(rect.d);
        y.push_back(rect.u);
      }
      sort(y.begin(),y.end());
      y.erase(unique(y.begin(),y.end()),y.end());
      vector<Range> ranges;
      vector<Event> events;
      ranges.reserve(rectangles.size());
      events.reserve(rectangles.size()*2);
      for(int i = 0; i < static_cast<int>(rectangles.size()); ++i) {
        const auto& [l,r,d,u] = rectangles[i];
        int low = lower_bound(y.begin(),y.end(),d)-y.begin();
        int high = lower_bound(y.begin(),y.end(),u)-y.begin();
        ranges.push_back({low,high});
        events.push_back({l,i*2});
        events.push_back({r,i*2+1});
      }
      sort(events.begin(),events.end(),[](const Event& a,const Event& b){ return a.x < b.x; });
      CoverTree tree(y);
      T last = events[0].x, answer = 0;
      for(const auto& [x,index] : events) {
        answer += (x-last)*tree.covered[1];
        last = x;
        auto [low,high] = ranges[index>>1];
        tree.change(low,high,(index & 1) ? -1 : 1);
      }
      return answer;
    }
};

}  // namespace soryuusi
