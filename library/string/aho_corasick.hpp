// https://github.com/soryuusi0219/compro/blob/main/library/string/aho_corasick.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class C = char, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z'>
struct AhoCorasick{
  private:
  static constexpr int ALPHA = MAX_CHARCTER - MIN_CHARCTER + 1;
    using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
    bool built = false;
    int addNode(int p){
      next.emplace_back();
      fail.emplace_back(0);
      par.emplace_back(p);
      cnt.emplace_back(0);
      return par.size() - 1;
    }
  public:
    vector<array<int,ALPHA>> next;
    vector<int> par, fail, cnt;
    AhoCorasick() {addNode(0);}
    AhoCorasick(int N) {
      next.reserve(N);
      par.reserve(N);
      fail.reserve(N);
      cnt.reserve(N);
      addNode(0);  
    }
    void insert(const VC& s) {
      assert(!built);
      int p = 0;
      for(C ch : s) {
        int i = ch - MIN_CHARCTER;
        int child = next[p][i];
        if(!child){
          child = addNode(p);
          next[p][i] = child;
        }
        p = child;
      }
      cnt[p]++;
    }
    void build(){
      if(built) return;
      vector<int> q;
      q.reserve(next.size()-1);
      for(int child : next[0]) if(child) q.push_back(child);
      for(int i = 0; i < (int)q.size(); ++i){
        const int u = q[i], fallback = fail[u];
        auto& edges = next[u];
        const auto& fallback_edges = next[fallback];
        cnt[u] += cnt[fallback];
        for(int c = 0; c < ALPHA; ++c) {
          const int child = edges[c], suffix_child = fallback_edges[c];
          if(child){
            fail[child] = suffix_child;
            q.push_back(child);
          }else edges[c] = suffix_child;
        }
      }
      built = true;
    }
    inline int size() const {return par.size();}
    vector<int> lex_order(){
      vector<int> res;
      res.reserve(size());
      res.emplace_back(0);
      vector<int> st;
      for(int child : next[0]) if(child) st.push_back(child);
      while(!st.empty()){
        int u = st.back(); st.pop_back();
        res.emplace_back(u);
        for(int c = 0; c < ALPHA; ++c)
          if(next[u][c] && next[u][c] != next[fail[u]][c]) st.push_back(next[u][c]);
      }
      return res;
    }
    int node(const VC& s, int p = 0){
      for(C ch : s) p = next[p][ch - MIN_CHARCTER];
      return p;
    }
    int query(const VC& s, int p = 0){
      int res = 0;
      for(C ch : s) {
        p = next[p][ch - MIN_CHARCTER];
        res += cnt[p];
      }
      return res;
    }
};

}  // namespace soryuusi
