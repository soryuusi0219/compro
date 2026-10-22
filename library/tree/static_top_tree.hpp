// https://github.com/soryuusi0219/compro/blob/main/library/tree/static_top_tree.hpp
#pragma once
#include <algorithm>
#include <numeric>
#include <queue>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto rake, auto compress>
struct StaticTopTree{
  static_assert(is_invocable_r_v<S, decltype(rake), S, S>, "rake must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(compress), S, S>, "compress must be callable with (S, S) and return a value convertible to S");

  private:
    int n;
    vector<int> par, deg, sz, hd, rev, lp, rp, pp, dep;
    vector<bool> type;
    vector<S> data;
    inline int add_rake(int x, int y){
      int m = data.size();
      lp.push_back(x), rp.push_back(y), pp.push_back(-1), type.emplace_back(0);
      pp[x] = m, pp[y] = m;
      data.emplace_back(rake(data[x],data[y]));
      dep.push_back(max(dep[x],dep[y])+1);
      return m;
    }
    inline int add_compress(int x, int y){
      int m = data.size();
      lp.push_back(x), rp.push_back(y), pp.push_back(-1), type.emplace_back(1);
      pp[x] = m, pp[y] = m;
      data.emplace_back(compress(data[x],data[y]));
      dep.push_back(max(dep[x],dep[y])+1);
      return m;
    }
  public:
    StaticTopTree() {}
    StaticTopTree(int n_) : n(n_), par(n,0), deg(n,0), sz(n,1), lp(n,-1), rp(n,-1), pp(n,-1), dep(n,1), data(n){}
    StaticTopTree(const vector<S>& v_) : n(v_.size()), par(n,0), deg(n,0), sz(n,1), lp(n,-1), rp(n,-1), pp(n,-1), dep(n,0), data(v_) {}
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
      vector<int> st_compress, st_heavypath = {root}, st_rake, st, ids(n+1,-1);
      auto comp = [&](int i, int j){return dep[i] > dep[j];};
      priority_queue<int,vector<int>,decltype(comp)> rake_pq(comp);
      for(int x = root, h = root, ii = 0; ~x; x = nxt[x], ++ii){
        ids[x] = ii;
        st_compress.push_back(x);
        if(sz[x] == 1) {
          h = ~nxt[x] ? par[nxt[x]] : n;
          while(st_heavypath.size() && ids[st_heavypath.back()] > ids[h]){
            int nowh = st_heavypath.back(); st_heavypath.pop_back();
            int pos = st_compress.size()-1;
            while(st_compress[pos] != nowh) --pos;
            for(; pos < (int)st_compress.size(); ++pos){
              int t = st_compress[pos];
              if(rake_pq.size()){
                t = add_rake(t, rake_pq.top());
                rake_pq.pop();
              }
              while(st_rake.size() && pp[st_rake.back()] == st_compress[pos]) {
                rake_pq.push(st_rake.back());
                st_rake.pop_back();
              }
              while(rake_pq.size() > 1){
                int i = rake_pq.top(); rake_pq.pop();
                int j = rake_pq.top(); rake_pq.pop();
                rake_pq.push(add_rake(i,j));
              }
              st.push_back(t);
              while(1){
                int len = st.size();
                if(len > 2 && (dep[st[len-3]] == dep[st[len-2]] || dep[st[len-3]] <= dep[st[len-1]])){
                  st[len-3] = add_compress(st[len-3],st[len-2]);
                  st[len-2] = st[len-1];
                  st.pop_back();
                }else if(len > 1 && dep[st[len-2]] <= dep[st[len-1]]){
                  st[len-2] = add_compress(st[len-2],st[len-1]);
                  st.pop_back();
                }else break;
              }
            }
            while(st.size() > 1){
              int len = st.size();
              st[len-2] = add_compress(st[len-2],st[len-1]);
              st.pop_back();
            }
            pp[st.back()] = par[nowh];
            st_rake.push_back(st.back());
            st.pop_back();
            while(st_compress[--pos] != nowh) st_compress.pop_back();
            st_compress.pop_back();
          }
          if(~nxt[x]) st_heavypath.push_back(nxt[x]);
        }
      }
      pp.back() = -1;
    }
    inline int parent(int v) const {return par[v];}
    void set_data(const vector<S>& v){
      for(int i = 0; i < n; ++i) data[i] = v[i];
      for(int i = n; i < n*2-1; ++i) data[i] = (type[i-n] ? compress : rake)(data[lp[i]],data[rp[i]]);
    }
    void set(int v, S x){
      data[v] = x;
      for(v = pp[v]; ~v; v = pp[v]) data[v] = (type[v-n] ? compress : rake)(data[lp[v]],data[rp[v]]);
    }
    inline S get() const {return data.back();}
};

}  // namespace soryuusi
