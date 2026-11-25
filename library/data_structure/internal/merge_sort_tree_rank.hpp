// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/merge_sort_tree_rank.hpp
#pragma once
#include <limits>
#include <vector>

#include "../../math/internal/internal_bit.hpp"
#include "../static_bit_vector.hpp"

namespace soryuusi::internal {

class MergeSortTreeRank {
  std::vector<StaticBitVector> levels;
  static constexpr int direct_search_limit = 512;

public:
  void build(int n, int height) {
    levels.resize(height);
    for(auto& level : levels) level.build(n);
  }

  void set_right(int level, int i) { levels[level].set(i); }
  void finish(int level) { levels[level].init(); }

  // Project a root key interval down to the canonical index blocks. Visit
  // them in the same order as the bottom-up segment tree traversal.
  void visit(int n, int l, int r, auto bounds, auto take) const {
    if(l == r) return;
    if(r-l <= direct_search_limit) {
      for(int offset = static_cast<int>(levels.size())*n, width = 1; l < r; offset -= n, width <<= 1) {
        if(l & width) {
          auto [a,b] = bounds(offset+l,offset+l+width);
          if(a < b) take(a,b);
          l += width;
        }
        if(r & width) {
          r -= width;
          auto [a,b] = bounds(offset+r,offset+r+width);
          if(a < b) take(a,b);
        }
      }
      return;
    }
    auto [first,last] = bounds(0,n);
    if(first >= last) return;
    struct Range { int first, last; };
    Range ranges[std::numeric_limits<int>::digits][2];
    unsigned present[2] = {};
    int height = levels.size();
    auto descend = [&](auto&& self, int level, int start, int a, int b) -> void {
      if(a == b) return;
      int width = 1 << level;
      if(l <= start && start+width <= r) {
        int side = !(start & width);
        int offset = (height-level)*n;
        ranges[level][side] = {offset+a,offset+b};
        present[side] |= 1U << level;
        return;
      }
      int mid = start+(width >> 1);
      const auto& rank = levels[level-1];
      // Earlier nodes at this level are full. Exactly half their keys
      // came from right children, so the rank at start is start/2.
      int ra = rank.rank1(a)-start/2, rb = rank.rank1(b)-start/2;
      if(l < mid) self(self,level-1,start,a-ra,b-rb);
      if(mid < r) self(self,level-1,mid,mid+ra,mid+rb);
    };
    descend(descend,height,0,first,last);
    for(unsigned mask = present[0] | present[1]; mask; mask &= mask-1) {
      int level = soryuusi::countr_zero(mask);
      for(int side = 0; side < 2; ++side) {
        if((present[side] >> level) & 1) take(ranges[level][side].first,ranges[level][side].last);
      }
    }
  }
};

}  // namespace soryuusi::internal
