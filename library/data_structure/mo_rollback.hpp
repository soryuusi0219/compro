// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/mo_rollback.hpp
#pragma once
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace soryuusi {

using namespace std;

struct Mo_Rollback{
  int n = 0, q = 0, b = 0;
  vector<int> left, right;
  Mo_Rollback(int n_, int q_) : n(n_), q(q_){
    b = max<int>(1, 1.0 * n / max<double>(1.0, sqrt(q * 2.0 / 3.0)));
    left.reserve(q_);
    right.reserve(q_);
  }
  void add_query(int l, int r) {
    left.push_back(l);
    right.push_back(r);
  }
  template <typename ADD, typename DEL, typename RES, typename SNAP, typename ROL, typename SOL>
  void run(const ADD &add, const DEL &del, const RES& reset, const SNAP &snapshot, const ROL &rollback, const SOL &solve){
    vector<int> order(q);
    iota(order.begin(),order.end(),0);
    sort(begin(order), end(order), [&](int l, int r) {
      int lblock = left[l] / b, rblock = left[r] / b;
      if (rblock != lblock) return lblock < rblock;
      return right[l] < right[r];
    });

    int nowr = 0, lbend = 0, nowlb = -1;
    for (auto idx : order) {
      auto l = left[idx], r = right[idx];
      int lb = l/b;
      if(nowlb != lb){
        reset();
        nowlb = lb;
        lbend = min(n, (nowlb+1)*b);
        nowr = lbend;
      }
      if(r < lbend){
        snapshot();
        for(int j = r-1; j >= l; j--) add(j);
        solve(idx);
        for(int j = l; j < r; j++) del(j);
        rollback();
      }else{
        while (nowr < r) add(nowr++);
        snapshot();
        for(int j = lbend-1; j >= l; j--) add(j);
        solve(idx);
        for(int j = l; j < lbend; j++) del(j);
        rollback();
      }
    }
  }
};

}  // namespace soryuusi
