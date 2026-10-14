// https://github.com/soryuusi0219/compro/blob/main/library/string/aho_corasick.hpp
#pragma once
#include <algorithm>
#include <array>
#include <queue>
#include <stack>
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
    int addNode(int p){
      next.emplace_back();
      next.back().fill(0);
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
      int p = 0;
      for(C ch : s) {
        int i = ch - MIN_CHARCTER;
        if(!next[p][i]) next[p][i] = addNode(p);
        p = next[p][i];
      }
      cnt[p]++;
    }
    void build(){
      queue<int> q;
      for(int c = 0; c < ALPHA; ++c) if(next[0][c]) q.push(next[0][c]);
      while(!q.empty()){
        int u = q.front(); q.pop();
        cnt[u] += cnt[fail[u]];
        for(int c = 0; c < ALPHA; ++c) {
          int& v = next[u][c];
          if(v) {
            fail[v] = next[fail[u]][c];
            q.push(v);
          } else v = next[fail[u]][c];
        }
      }
    }
    inline int size() const {return par.size();}
    vector<int> lex_order(){
      vector<int> res;
      res.reserve(size());
      res.emplace_back(0);
      stack<int> st;
      for(int c = 0; c < ALPHA; ++c) if(next[0][c]) st.push(next[0][c]);
      while(!st.empty()){
        int u = st.top(); st.pop();
        res.emplace_back(u);
        for(int c = 0; c < ALPHA; ++c) if(next[u][c] && next[u][c] != next[fail[u]][c]) st.push(next[u][c]);
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
