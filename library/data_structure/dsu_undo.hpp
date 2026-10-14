// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dsu_undo.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct dsu_undo {
  int n;
  vector<int> data;
  vector<int> history;
  dsu_undo() : n(0) {}
  dsu_undo(int n_) : n(n_), data(n, -1) {
    history.reserve(n*4);
  }

  int merge(int a, int b) {
    int x = leader(a), y = leader(b);
    if (x == y) return -1;
    if (-data[x] < -data[y]) swap(x, y);
    history.push_back(x);
    history.push_back(data[x]);
    history.push_back(y);
    history.push_back(data[y]);
    data[x] += data[y];
    data[y] = x;
    return x;
  }
  bool same(int a, int b) {
    return leader(a) == leader(b);
  }
  int leader(int a) {
    while(data[a] >= 0) a = data[a];
    return a;
  }
  int size(int a) {
    assert(0 <= a && a < n);
    return -data[leader(a)];
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
  int get_state(){return (history.size() >> 2);}
  void undo(){
    data[history[history.size()-2]] = history.back();
    history.pop_back();
    history.pop_back();
    data[history[history.size()-2]] = history.back();
    history.pop_back();
    history.pop_back();
  }
  void rollback(int state){
    state <<= 2;
    while(state < (int)history.size()) undo();
  }
};

}  // namespace soryuusi
