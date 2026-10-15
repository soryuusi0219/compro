// https://github.com/soryuusi0219/compro/blob/main/library/tree/heavy_light_decomposition_balanced.hpp
#pragma once
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e>
struct HLdecomposition_Balanced{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  private:
    int n;
    vector<int> par, deg, sz, hd, in, rev, lp, rp, pp, dep;
    vector<S> lsum, rsum;
    inline void calc(int i){lsum[i] = op(lsum[rp[i]],lsum[lp[i]]), rsum[i] = op(rsum[lp[i]],rsum[rp[i]]);}
    S lprod(int i){
      S res = lsum[i];
      while(dep[i]){
        if(rp[pp[i]] == i) res = op(res,lsum[lp[pp[i]]]);
        i = pp[i];
      }
      return res;
    }
    S rprod(int i){
      S res = rsum[i];
      while(dep[i]){
        if(rp[pp[i]] == i) res = op(rsum[lp[pp[i]]],res);
        i = pp[i];
      }
      return res;
    }
    S lprod(int i, int j){
      if(i == j) return lsum[i];
      S ls = lsum[i], rs = lsum[j];
      while(pp[i] != pp[j]){
        if(dep[i] > dep[j]){
          if(lp[pp[i]] == i) ls = op(lsum[rp[pp[i]]],ls);
          i = pp[i];
        }else{
          if(rp[pp[j]] == j) rs = op(rs,lsum[lp[pp[j]]]);
          j = pp[j];
        }
      }
      return op(ls,rs);
    }
    S rprod(int i, int j){
      if(i == j) return rsum[i];
      S ls = rsum[i], rs = rsum[j];
      while(pp[i] != pp[j]){
        if(dep[i] > dep[j]){
          if(lp[pp[i]] == i) ls = op(ls,rsum[rp[pp[i]]]);
          i = pp[i];
        }else{
          if(rp[pp[j]] == j) rs = op(rsum[lp[pp[j]]],rs);
          j = pp[j];
        }
      }
      return op(rs,ls);
    }
  public:
    HLdecomposition_Balanced(int n_) : n(n_), par(n,0), deg(n,0), sz(n*2,1), hd(n,-1), in(n), rev(n), lp(n*2,-1), rp(n*2,-1), pp(n*2,-1), dep(2*n,0), lsum(n*2,e()), rsum(n*2,e()){}
    void add_edge(int u, int v){
      par[u] ^= v, par[v] ^= u, ++deg[u], ++deg[v];
    }
    void build(int root, const vector<S>& data = {}){
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
      if(!data.empty()) copy(data.begin(), data.end(), lsum.begin()), copy(data.begin(), data.end(), rsum.begin());
      vector<int> st;
      st.reserve(n);
      int cur = n;
      for(int x = root, i = 0, h = root; ~x; x = nxt[x], ++i) {
        hd[x] = h, in[x] = i, rev[i] = x;
        if(sz[x] == 1) h = nxt[x];
      }
      vector<int> d(n*2,0);
      for(int i = n, u, len; i;){
        st.push_back(u = rev[--i]);
        while(true){
          len = st.size();
          if(len > 2 && (d[st[len-3]] == d[st[len-2]] || d[st[len-3]] <= d[st[len-1]])){
            int x = st[len-2], y = st[len-3];
            pp[x] = cur, pp[y] = cur, lp[cur] = x, rp[cur] = y;
            lsum[cur] = op(lsum[y],lsum[x]), rsum[cur] = op(rsum[x],rsum[y]), d[cur] = max(d[x],d[y])+1;
            st[len-3] = cur++;
            st[len-2] = st[len-1];
            st.pop_back();
          }else if(len > 1 && d[st[len-2]] <= d[st[len-1]]){
            int x = st[len-1], y = st[len-2];
            pp[x] = cur, pp[y] = cur, lp[cur] = x, rp[cur] = y;
            lsum[cur] = op(lsum[y],lsum[x]), rsum[cur] = op(rsum[x],rsum[y]), d[cur] = max(d[x],d[y])+1;
            st.pop_back();
            st.back() = cur++;
          }else break;
        }
        if(hd[u] == u){
          while((len = st.size()) > 1){
            int x = st[len-1], y = st[len-2];
            pp[x] = cur, pp[y] = cur, lp[cur] = x, rp[cur] = y;
            lsum[cur] = op(lsum[y],lsum[x]), rsum[cur] = op(rsum[x],rsum[y]), d[cur] = max(d[x],d[y])+1;
            st.pop_back();
            st.back() = cur++;
          }
          if(~par[u] && d[par[u]] <= d[st[0]]) d[par[u]] = d[st[0]] + 1;
          st.pop_back();
        }
      }
      while(cur) if(~pp[--cur]) dep[cur] = dep[pp[cur]]+1;
    }
    void set(int u, S x){
      lsum[u] = rsum[u] = x;
      for(u = pp[u]; ~u; u = pp[u]) calc(u);
    }
    S get(int u){
      return(lsum[u]);
    }
    S prod(int u, int v){
      S ls = e(), rs = e();
      while(hd[u] != hd[v]){
        if(in[u] > in[v]){
          ls = op(ls,lprod(u));
          u = par[hd[u]];
        }else{
          rs = op(rprod(v),rs);
          v = par[hd[v]];
        }
      }
      if(in[u] > in[v]) return op(op(ls,lprod(v,u)),rs);
      else return op(op(ls,rprod(u,v)),rs);
    }
};

}  // namespace soryuusi
