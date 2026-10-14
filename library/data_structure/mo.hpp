// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/mo.hpp
#pragma once
#include <algorithm>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct Mo{
  int n = 0, q = 0, b = 0;
  vector<int> left, right;
  Mo(int n_, int q_) : n(n_), q(q_){
    b = max<int>(1, 1.0 * n / max<double>(1.0, sqrt(q * 2.0 / 3.0)));
    left.reserve(q_);
    right.reserve(q_);
  }
  void add_query(int l, int r) {
    left.push_back(l);
    right.push_back(r);
  }
  template <typename AL, typename AR, typename DL, typename DR, typename SOL>
  void run(const AL &add_left, const AR &add_right, const DL &delete_left, const DR &delete_right, const SOL &solve){
    vector<int> order1(q), order2(q);
    iota(order1.begin(),order1.end(),0);
    iota(order2.begin(),order2.end(),0);
    sort(begin(order1), end(order1), [&](int l, int r) {
      int lblock = left[l] / b, rblock = left[r] / b;
      if (rblock != lblock) return lblock < rblock;
      if (rblock & 1) return right[r] < right[l];
      return right[r] > right[l];
    });
    sort(begin(order2), end(order2), [&](int l, int r) {
      int lblock = (left[l] + b/2) / b, rblock = (left[r] + b/2) / b;
      if (rblock != lblock) return lblock < rblock;
      if (rblock & 1) return right[r] < right[l];
      return right[r] > right[l];
    });
    int move1 = 0, move2 = 0;
    int nl = 0, nr = 0;
    for(auto idx : order1){
      move1 += abs(left[idx] - nl) + abs(right[idx] - nr);
      nl = left[idx], nr = right[idx];
    }
    nl = 0, nr = 0;
    for(auto idx : order2){
      move2 += abs(left[idx] - nl) + abs(right[idx] - nr);
      nl = left[idx], nr = right[idx];
    }
    if(move2 < move1) order1.swap(order2);
    nl = 0, nr = 0;
    for (auto idx : order1) {
      while (nl > left[idx]) add_left(--nl);
      while (nr < right[idx]) add_right(nr++);
      while (nl < left[idx]) delete_left(nl++);
      while (nr > right[idx]) delete_right(--nr);
      solve(idx);
    }
  }
};

}  // namespace soryuusi
