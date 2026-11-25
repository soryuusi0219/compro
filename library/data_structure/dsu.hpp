// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dsu.hpp
#pragma once
#include <algorithm>
#include <vector>
#include <utility>

namespace soryuusi {

using namespace std;

struct DSU{
  private:
    friend struct Range_Parallel_Unionfind;
    std::vector<int> e;
    int merge_roots(int u, int v) {
      if(e[u] > e[v]) swap(u, v);
      e[u] += e[v];
      return e[v] = u;
    }
  public:
    DSU() {};
    DSU(int n) : e(n,-1) {}
    int leader(int u) { return e[u] < 0 ? u : e[u] = leader(e[u]); }
    bool same(int u, int v) { return leader(u) == leader(v);}
    int size(int u) { return -e[leader(u)]; }
    int merge(int u, int v) {
      u = leader(u), v = leader(v);
      return u == v ? u : merge_roots(u,v);
    }
    vector<vector<int>> groups() {
      int n = e.size();
      vector<vector<int>> result(n);
      for(int i = 0; i < n; ++i) {
        if(e[i] < 0) result[i].reserve(-e[i]);
      }
      for(int i = 0; i < n; ++i) result[leader(i)].push_back(i);
      result.erase(remove_if(result.begin(),result.end(),[](const auto& group){ return group.empty(); }),result.end());
      return result;
    }

};

}  // namespace soryuusi
