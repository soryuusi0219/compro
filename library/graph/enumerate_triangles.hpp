// https://github.com/soryuusi0219/compro/blob/main/library/graph/enumerate_triangles.hpp
#pragma once
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"

namespace soryuusi {

using namespace std;

struct EnumerateTriangles{
  private:
    int n;
    vector<int> from, to, degree;
  public:
    EnumerateTriangles(int n_) : n(n_), degree(n,0) {}
    EnumerateTriangles(int n_, int m_) : n(n_), degree(n,0) {from.reserve(m_), to.reserve(m_);}
    void add_edge(int u, int v){
      if(u > v) swap(u,v);
      from.push_back(u), to.push_back(v);
      ++degree[u], ++degree[v];
    }
    template<class F>
    void solve(F f){
      const int m = from.size();
      vector<int> offsets(n+1), adjacent(m);
      for(int i = 0; i < m; ++i){
        const int u = from[i], v = to[i];
        ++offsets[1+(degree[u] <= degree[v] ? u : v)];
      }
      partial_sum(offsets.begin(),offsets.end(),offsets.begin());
      auto end = offsets;
      for(int i = 0; i < m; ++i){
        int u = from[i], v = to[i];
        if(degree[u] > degree[v]) swap(u,v);
        adjacent[--end[u+1]] = v;
      }
      if(8LL*m >= 1LL*n*n){
        solve_dense(offsets,adjacent,f);
        return;
      }
      vector<int> marked(n,-1);
      for(int u = 0; u < n; ++u){
        if(offsets[u+1]-offsets[u] < 2) continue;
        for(int i = offsets[u]; i < offsets[u+1]; ++i) marked[adjacent[i]] = u;
        for(int i = offsets[u]; i < offsets[u+1]; ++i){
          const int v = adjacent[i];
          for(int j = offsets[v]; j < offsets[v+1]; ++j) if(marked[adjacent[j]] == u) f(u,v,adjacent[j]);
        }
      }
    };

  private:
    template<class F>
    void solve_dense(const vector<int>& offsets, const vector<int>& adjacent, F& f) const {
      const int words = (n+63)/64;
      vector<uint64_t> masks(size_t(n)*words);
      for(int u = 0; u < n; ++u) for(int i = offsets[u]; i < offsets[u+1]; ++i){
        const int v = adjacent[i];
        masks[size_t(u)*words+v/64] |= uint64_t{1}<<(v%64);
      }
      for(int u = 0; u < n; ++u){
        const auto* a = masks.data()+size_t(u)*words;
        for(int i = offsets[u]; i < offsets[u+1]; ++i){
          const int v = adjacent[i];
          const auto* b = masks.data()+size_t(v)*words;
          for(int k = 0; k < words; ++k) for(uint64_t bits = a[k]&b[k]; bits; bits &= bits-1) f(u,v,64*k+soryuusi::countr_zero(bits));
        }
      }
    }
};

}  // namespace soryuusi
