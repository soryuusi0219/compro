// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dsu.hpp
#pragma once
#include <vector>
#include <algorithm>

namespace soryuusi {

using namespace std;

struct DSU{
  private:
    std::vector<int> e;
  public:
    DSU() {};
    DSU(int n) : e(n,-1) {}
    int leader(int u) { return e[u] < 0 ? u : e[u] = leader(e[u]); }
    bool same(int u, int v) { return leader(u) == leader(v);}
    int size(int u) { return -e[leader(u)]; }
    int merge(int u, int v) {
      u = leader(u), v = leader(v);
      if(u == v) return u;
      if(e[u] > e[v]) swap(u, v);
      e[u] += e[v];
      return e[v] = u;
    }
    vector<vector<int>> groups() {
      vector<vector<int>> res(e.size());
      for (size_t i = 0; i < e.size(); i++) res[leader(i)].push_back(i);
      res.erase(remove_if(res.begin(), res.end(), [&](const vector<int>& v) { return v.empty(); }),res.end());
      return res;
    }

};

}  // namespace soryuusi
