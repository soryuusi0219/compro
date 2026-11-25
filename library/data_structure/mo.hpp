// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/mo.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct Mo{
  int n = 0, q = 0, b = 0;
  vector<int> left, right;
  Mo(int n_, int q_) : n(n_), q(q_){
    assert(n >= 0 && q >= 0);
    b = max<int>(1, 1.0 * n / max<double>(1.0, sqrt(q * 2.0 / 3.0)));
    left.reserve(q_);
    right.reserve(q_);
  }
  void add_query(int l, int r) {
    assert(0 <= l && l <= r && r <= n);
    left.push_back(l);
    right.push_back(r);
  }
  template <typename AL, typename AR, typename DL, typename DR, typename SOL>
  void run(const AL &add_left, const AR &add_right, const DL &delete_left, const DR &delete_right, const SOL &solve){
    assert(b > 0 && left.size() == right.size());
    auto order_by = [&](int shift){
      vector<pair<std::uint64_t,int>> order;
      order.reserve(left.size());
      for(int i = 0; i < static_cast<int>(left.size()); ++i){
        std::uint64_t block = (std::uint64_t(left[i])+shift)/b;
        auto end = (block&1) ? n-right[i] : right[i];
        order.emplace_back((block<<32)|std::uint32_t(end),i);
      }
      sort(order.begin(),order.end(),[](const auto& a,const auto& b){return a.first < b.first;});
      return order;
    };
    auto order = order_by(0);
    auto movements = [&](const auto& queries){
      std::int64_t total = 0;
      int l = 0, r = 0;
      for(const auto& entry : queries){
        int i = entry.second;
        total += std::int64_t(abs(left[i]-l))+abs(right[i]-r);
        l = left[i]; r = right[i];
      }
      return total;
    };
    if(b > 1 && !order.empty()){
      auto shifted = order_by(b/2);
      if(movements(shifted) < movements(order)) order.swap(shifted);
    }
    int nl = 0, nr = 0;
    for(const auto& entry : order){
      int idx = entry.second;
      while (nl > left[idx]) add_left(--nl);
      while (nr < right[idx]) add_right(nr++);
      while (nl < left[idx]) delete_left(nl++);
      while (nr > right[idx]) delete_right(--nr);
      solve(idx);
    }
  }
};

}  // namespace soryuusi
