// https://github.com/soryuusi0219/compro/blob/main/library/graph/incremental_msf.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <utility>
#include <vector>

#include "library/data_structure/dsu_undo.hpp"

namespace soryuusi {

using namespace std;

void incrementalMSF(int n, const vector<pair<int,int>>& es, auto comp, auto add, auto snapshot, auto rollback, auto solve){
  int M = es.size();
  int B = bit_ceil((unsigned)(M+1));
  vector<int> now;
  now.reserve(M);
  dsu_undo d(n);
  auto dfs = [&](auto&& self, int x, int l, int r, int k) -> void {
    if(l > M) return;
    sort(now.begin()+k,now.end(),comp);
    int m = now.size();
    int state = d.get_state();
    for(int i = l; i < min(r-1,M); ++i) d.merge(es[i].first, es[i].second);
    int j = k;
    for(int i = k; i < m; ++i) if(~d.merge(es[now[i]].first, es[now[i]].second)) swap(now[i],now[j++]);
    d.rollback(state);
    snapshot();
    for(int i = k; i < j; ++i) {
      d.merge(es[now[i]].first, es[now[i]].second);
      add(now[i]);
    }
    if(x < B){
      sort(now.rbegin(),now.rend()-j,comp);
      int state2 = d.get_state();
      for(int i = m-1; i >= j; --i) if(!~d.merge(es[now[i]].first, es[now[i]].second)) swap(now[i],now[--m]);
      now.resize(m);
      d.rollback(state2);
      self(self, x << 1, l, (l+r) >> 1, j);
      self(self, x << 1 | 1, (l+r) >> 1, r, j);
    }else{
      solve(x-B);
      if(x-B < M) now.push_back(x-B);
    }
    d.rollback(state);
    rollback();
  };
  dfs(dfs,1, 0, B, 0);
}

}  // namespace soryuusi
