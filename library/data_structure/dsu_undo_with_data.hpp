// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dsu_undo_with_data.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <stack>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, auto op>
struct dsu_undo_data {
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");

  int n;
  int state_snap = 0;
  vector<int> data;
  vector<T> val;
  stack<tuple<int, int, T>> history;
  stack<int> block;
  dsu_undo_data() : n(0) {}
  dsu_undo_data(int n_, T v) : n(n_), data(n,-1), val(n, v) {}
  dsu_undo_data(const vector<T>& val_) : n(val_.size()), data(val_.size(),-1), val(val_) {}

  int merge(int a, int b) {
      assert(0 <= a && a < n);
      assert(0 <= b && b < n);
      int x = leader(a), y = leader(b);
      if (x == y) return -1;
      if (-data[x] < -data[y]) swap(x, y);
      block.emplace(history.size());
      history.emplace(x,data[x],val[x]);
      history.emplace(y,data[y],val[y]);
      data[x] += data[y];
      data[y] = x;
      val[x] = op(val[x], val[y]); 
      return x;
  }
  void apply(int a, T v) {
      assert(0 <= a && a < n);
      int x = leader(a);
      block.emplace(history.size());
      history.emplace(x,data[x],val[x]);
      val[x] = op(val[x], v);
  }

  bool same(int a, int b) {
      assert(0 <= a && a < n);
      assert(0 <= b && b < n);
      return leader(a) == leader(b);
  }
  int leader(int a) {
      assert(0 <= a && a < n);
      while(data[a] >= 0) a = data[a];
      return a;
  }
  int size(int a) {
      assert(0 <= a && a < n);
      return -data[leader(a)];
  }
  T get(int a) {
      assert(0 <= a && a < n);
      return val[leader(a)];
  }
  vector<vector<long long>> groups() {
      vector<int> leader_buf(n), group_size(n);
      for (int i = 0; i < n; i++) {
        leader_buf[i] = leader(i);
        group_size[leader_buf[i]]++;
      }
      vector<vector<long long>> result(n);
      for (int i = 0; i < n; i++) result[i].reserve(group_size[i]);
      for (int i = 0; i < n; i++) result[leader_buf[i]].push_back(i);
      result.erase(remove_if(result.begin(), result.end(),[&](const vector<long long>& v) { return v.empty(); }),result.end());
      return result;
  }
  int get_state(){return block.size();}
  void snapshot(){state_snap = get_state();}
  void undo(){
    assert(!block.empty());
    while((int)history.size() > block.top()){
      auto [x,s,v] = history.top(); history.pop();
      data[x] = s;
      val[x] = v;
    }
    block.pop();
  }
  void rollback(int state = -1){
    if(state == -1) state = state_snap;
    assert(state <= (int)block.size());
    while(state < (int)block.size()) undo();
  }
};

}  // namespace soryuusi
