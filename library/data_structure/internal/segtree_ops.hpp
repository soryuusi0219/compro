// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/segtree_ops.hpp
#pragma once
#include <algorithm>
#include <vector>

#include "../../math/internal/internal_bit.hpp"

namespace soryuusi::internal {

template<class F, auto composition, auto id>
class SegtreeTags {
  std::vector<F> tags;
  std::vector<unsigned char> pending;

public:
  SegtreeTags() = default;
  explicit SegtreeTags(int b) : tags(b,id()), pending(b) {}

  void apply(int k, F& f) {
    if(pending[k]) tags[k] = composition(f,tags[k]);
    else tags[k] = F(f);
    pending[k] = true;
  }

  void push(int k, auto act) {
    if(!pending[k]) return;
    act(k << 1, tags[k]), act(k << 1 | 1, tags[k]);
    pending[k] = false;
    tags[k] = id();
  }
};

class SegtreeBoundary {
  int left, right, height, left_aligned, right_aligned, shared;

public:
  // b <= l < r <= 2*b. At heights >= shared the paths meet.
  SegtreeBoundary(int l, int r, int h) : left(l), right(r-1), height(h), left_aligned(soryuusi::countr_zero(static_cast<unsigned>(l))), right_aligned(soryuusi::countr_zero(static_cast<unsigned>(r))), shared(soryuusi::bit_width(static_cast<unsigned>(left ^ right))) {}

  void push(auto visit) const {
    int end = std::max(shared-1,std::min(left_aligned,right_aligned));
    for(int i = height; i > end; --i) visit(left >> i);
    for(int i = shared-1; i > left_aligned; --i) visit(left >> i);
    for(int i = shared-1; i > right_aligned; --i) visit(right >> i);
  }

  void pull(auto visit) const {
    for(int i = left_aligned+1; i < shared; ++i) visit(left >> i);
    for(int i = right_aligned+1; i < shared; ++i) visit(right >> i);
    for(int i = std::max(shared,std::min(left_aligned,right_aligned)+1); i <= height; ++i) visit(left >> i);
  }
};

template<class Visit>
void segtree_push_point(int p, int h, Visit visit) {
  for(int i = h; i > 0; --i) visit(p >> i);
}

template<class Visit>
void segtree_pull_point(int p, Visit visit) {
  for(int k = p >> 1; k; k >>= 1) visit(k);
}

}  // namespace soryuusi::internal
