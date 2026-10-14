// https://github.com/soryuusi0219/compro/blob/main/library/graph/enumerate_triangles.hpp
#pragma once
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct EnumerateTriangles{
  private:
    int n;
    vector<int> h, from, to, r;
  public:
    EnumerateTriangles(int n_) : n(n_), h(n,-1), r(n,0) {}
    EnumerateTriangles(int n_, int m_) : n(n_), h(n,-1), r(n,0) {from.reserve(m_), to.reserve(m_);}
    void add_edge(int u, int v){
      if(u > v) swap(u,v);
      from.push_back(u), to.push_back(v);
      ++r[u], ++r[v];
    }
    template<class F>
    void solve(F f){
      int m = from.size();
      vector<int> h(n,-1), es(m,-1);
      for(int i = 0; i < m; ++i) if(r[from[i]] > r[to[i]]) swap(from[i],to[i]);
      for(int i = 0; i < m; ++i) --r[to[i]];
      for(int i = 0; i < n-1; ++i) r[i+1] += r[i];
      vector<int> l = r;
      for(int i = 0; i < m; ++i) es[--l[from[i]]] = to[i];
      vector<bool> b(n,false);
      for(int i = 0; i < n; ++i){
        for(int e = l[i]; e < r[i]; ++e) b[es[e]] = true;
        for(int ej = l[i]; ej < r[i]; ++ej) {
          int j = es[ej];
          for(int ek = l[j]; ek < r[j]; ++ek) if(b[es[ek]]) f(i,j,es[ek]);
        }
        for(int e = l[i]; e < r[i]; ++e) b[es[e]] = false;
      }
    };
};

}  // namespace soryuusi
