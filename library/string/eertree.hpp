// https://github.com/soryuusi0219/compro/blob/main/library/string/eertree.hpp
#pragma once
#include <array>
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
    VC s;
    vector<int> p, sl, r, len, bk;
    vector<maptype> nxt;
    int addNode(int par, int r0, int len0, int link, C c){
      p.push_back(par);
      sl.push_back(link);
      r.push_back(r0);
      len.push_back(len0);
      nxt.emplace_back();
      const int id = p.size()-1;
      if constexpr (USE_MAP) nxt[par].emplace(c,id);
      else nxt[par][c-MIN_CHARCTER] = id;
      return id;
    }
    int extendable_suffix(int pos, C c, int v) const {
      while(len[v] >= pos || s[pos-len[v]-1] != c) v = sl[v];
      return v;
    }
  public:
    Eertree() : p(2,-1), sl(2,-1), r(2,0), len(2,-1), bk(1,1), nxt(2){
      len[1] = 0;
      sl[1] = 0;
    }
    Eertree(int n_) : Eertree(){s.reserve(n_), p.reserve(n_+2), sl.reserve(n_+2), r.reserve(n_+2), len.reserve(n_+2), bk.reserve(n_+2), nxt.reserve(n_+2);}
    Eertree(const VC& s) : Eertree(s.size()){for(auto c : s) push_back(c);}
    void push_back(C c){
      const int pos = s.size();
      s.push_back(c);
      const int k = extendable_suffix(pos,c,bk.back());
      int child = get_next(k,c);
      if(child < 0){
        const int link = k == 0 ? 1 : get_next(extendable_suffix(pos,c,sl[k]),c);
        child = addNode(k,pos+1,len[k]+2,link,c);
      }
      bk.push_back(child);
    }
    inline int node_size() const {return p.size();}
    inline int suffix_link(int i) const {return sl[i];}
    inline int parent(int i) const {return p[i];}
    inline int get_next(int i, C c) const {
      if constexpr (USE_MAP){
        const auto it = nxt[i].find(c);
        return it == nxt[i].end() ? -1 : it->second;
      }else{
        const int child = nxt[i][c-MIN_CHARCTER];
        return child == 0 ? -1 : child;
      }
    }
    inline pair<int,int> ref(int i) const {return {r[i] - len[i], r[i]};}
    inline int length(int i) const {return len[i];}
    inline int maximal_suffix(int i) const {return bk[i+1];}
};

}  // namespace soryuusi
