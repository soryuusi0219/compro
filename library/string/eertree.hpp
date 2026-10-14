// https://github.com/soryuusi0219/compro/blob/main/library/string/eertree.hpp
#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class C = char, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z', bool USE_MAP = false>
struct Eertree{
  static constexpr int ALPHA = MAX_CHARCTER - MIN_CHARCTER + 1;
  using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
  using maptype = conditional_t<USE_MAP, map<C,int>, array<int,ALPHA>>;
  private:
    int n;
    VC s;
    vector<int> p, sl, r, len, bk;
    vector<maptype> nxt;
    inline int addNode(int par, int r0, int len0, C c){
      p.push_back(par);
      sl.push_back(-1);
      r.push_back(r0);
      len.push_back(len0);
      nxt.emplace_back();
      if constexpr (!USE_MAP) fill(nxt.back().begin(),nxt.back().end(),-1);
      return nxt[par][c-MIN_CHARCTER] = p.size() - 1;
    }
  public:
    Eertree() : n(0), p(2,-1), sl(2,-1), r(2,0), len(2,-1), bk(1,1), nxt(2){
      len[1] = 0;
      sl[1] = 0;
      if constexpr (!USE_MAP) {
        fill(nxt[0].begin(),nxt[0].end(),-1);
        fill(nxt[1].begin(),nxt[1].end(),-1);
      }
    }
    Eertree(int n_) : Eertree(){s.reserve(n_), p.reserve(n_+2), sl.reserve(n_+2), r.reserve(n_+2), len.reserve(n_+2), bk.reserve(n_+2), nxt.reserve(n_+2);}
    Eertree(const VC& s) : Eertree(s.size()){for(auto c : s) push_back(c);}
    void push_back(C c){
      s.push_back(c);
      int k = bk[n++];
      while(len[k]+2 > n || s[n-len[k]-2] != c) k = sl[k];
      if constexpr (USE_MAP){
        if(nxt[k].count(c-MIN_CHARCTER)) {
          bk.emplace_back(nxt[k][c-MIN_CHARCTER]);
          return;
        }
      }else{
        if(~nxt[k][c-MIN_CHARCTER]) {
          bk.emplace_back(nxt[k][c-MIN_CHARCTER]);
          return;
        }
      }
      bk.emplace_back(addNode(k, n, len[k]+2, c));
      do k = sl[k]; while(~k && s[n-len[k]-2] != c);
      sl[bk.back()] = ~k ? nxt[k][c-MIN_CHARCTER] : 1;
    }
    inline int node_size() const {return p.size();}
    inline int suffix_link(int i) const {return sl[i];}
    inline int parent(int i) const {return p[i];}
    inline int get_next(int i, C c) const {return nxt[i][c-MIN_CHARCTER];}
    inline pair<int,int> ref(int i) const {return {r[i] - len[i], r[i]};}
    inline int length(int i) const {return len[i];}
    inline int maximal_suffix(int i) const {return bk[i+1];}
};

}  // namespace soryuusi
