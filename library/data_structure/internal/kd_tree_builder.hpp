// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/kd_tree_builder.hpp
#pragma once
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

#include "../../math/internal/internal_bit.hpp"

namespace soryuusi::internal {

template<class X, class Y>
class KDTreeBuilder {
  using Point = std::pair<X,Y>;
  const std::vector<Point>& points;
  std::vector<int> order;

public:
  explicit KDTreeBuilder(const std::vector<Point>& p) : points(p), order(p.size()) {
    std::iota(order.begin(),order.end(),0);
  }

  void build(auto leaf, auto join) {
    const int n = order.size();
    if(n == 0) return;
    auto by_x = [&](int a,int b){ return points[a].first < points[b].first; };
    auto by_y = [&](int a,int b){ return points[a].second < points[b].second; };
    bool same_x = true, same_y = true;
    bool ascending_x = true, ascending_y = true, descending_y = true;
    for(int i = 1; i < n; ++i) {
      bool x_up = by_x(i-1,i), x_down = by_x(i,i-1);
      bool y_up = by_y(i-1,i), y_down = by_y(i,i-1);
      same_x &= !(x_up || x_down);
      same_y &= !(y_up || y_down);
      ascending_x &= !x_down;
      ascending_y &= !y_down;
      descending_y &= !y_up;
      if(!same_x && !same_y && (!ascending_x || (!ascending_y && !descending_y))) break;
    }

    // Either axis can split already ordered points. If only one varies,
    // sort on that axis once and keep the order throughout the build.
    if(same_x && !ascending_y && !descending_y) std::sort(order.begin(),order.end(),by_y);
    if(same_y && !ascending_x && !same_x) std::sort(order.begin(),order.end(),by_x);
    const bool ordered = same_x || same_y || (ascending_x && (ascending_y || descending_y));
    auto rec = [&](auto&& self,int l,int r,int k,bool x_axis) -> void {
      if(r-l == 1) {
        leaf(k,order[l]);
        return;
      }
      const int half = soryuusi::bit_floor(static_cast<unsigned>(r-l)) >> 1;
      const int m = l+half+std::min(r-l-2*half,half);
      if(!ordered) {
        if(x_axis) std::nth_element(order.begin()+l,order.begin()+m,order.begin()+r,by_x);
        else std::nth_element(order.begin()+l,order.begin()+m,order.begin()+r,by_y);
      }
      self(self,l,m,k<<1,!x_axis);
      self(self,m,r,k<<1|1,!x_axis);
      join(k);
    };
    rec(rec,0,n,1,true);
  }
};

}  // namespace soryuusi::internal
