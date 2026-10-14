// https://github.com/soryuusi0219/compro/blob/main/library/tree/heavy_light_decomposition.hpp
#pragma once
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct HLdecomposition{
  private:
    int n;
    vector<int> par, deg, in, sz, hd, dep;
  public:
    HLdecomposition(int n_) : n(n_), par(n,0), deg(n,0), in(n), sz(n,1), hd(n,-1), dep(n,0){}
    void add_edge(int u, int v){
      par[u] ^= v, par[v] ^= u, ++deg[u], ++deg[v];
    }
    void build(int root){
      deg[root] = 0;
      vector<int> nxt(n,-1), mxsz(n,0), bk(n,-1);
      iota(bk.begin(),bk.end(),0);
      for(int i = 0, u = 0, v; i < n; u = ++i) while(deg[u] == 1){
        v = par[u];
        par[v] ^= u;
        --deg[u], --deg[v];
        sz[v] += sz[u];
        if(sz[u] > mxsz[v]){
          mxsz[v] = sz[u];
          nxt[bk[u]] = nxt[v];
          nxt[v] = u;
          if(bk[v] == v) bk[v] = bk[u];
        }else{
          nxt[bk[v]] = u;
          bk[v] = bk[u];
        }
        u = v;
      }
      for(int i = 0, x = root, h = root; ~x; ++i, x = nxt[x]) {
        in[x] = i, hd[x] = h;
        if(sz[x] == 1) h = nxt[x];
        if(i) dep[x] = dep[par[x]] + 1;
      }
    }
    inline int inid(int u){return in[u];}
    inline int outid(int u){return in[u] + sz[u];}
    inline int depth(int u){return dep[u];}

    int lca(int u, int v){
      while(hd[u] != hd[v]){
        if(depth(hd[u]) > depth(hd[v])) u = par[hd[u]];
        else v = par[hd[v]];
      }
      return depth(u) < depth(v) ? u : v;
    }
    
    template<class F> void for_each(int u, int v, F f){
      while(hd[u] != hd[v]){
        if(depth(hd[u]) < depth(hd[v])) swap(u,v);
        f(inid(hd[u]),inid(u)+1);
        u = par[hd[u]];
      }
      if(depth(u) < depth(v)) swap(u,v);
      f(inid(v),inid(u)+1);
    }

    template<class F1, class F2> void for_each(int u, int v, F1 f1, F2 f2){
      while(hd[u] != hd[v]){
        if(depth(hd[u]) > depth(hd[v])){
          f1(inid(hd[u]),inid(u)+1);
          u = par[hd[u]];
        }else{
          f2(inid(hd[v]),inid(v)+1);
          v = par[hd[v]];
        }
      }
      if(depth(u) > depth(v)) f1(inid(v),inid(u)+1);
      else f2(inid(u),inid(v)+1);
    }
};

}  // namespace soryuusi
