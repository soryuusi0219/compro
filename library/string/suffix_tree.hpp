// https://github.com/soryuusi0219/compro/blob/main/library/string/suffix_tree.hpp
#pragma once
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class C = char, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z', bool USE_MAP = false>
struct SuffixTree{
  static constexpr int ALPHA = MAX_CHARCTER - MIN_CHARCTER + 1;
  using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
  using maptype = conditional_t<USE_MAP, map<C,int>, array<int,ALPHA>>;
  
  struct StaticRMQ{
    static constexpr int b = 3;
    static constexpr int B = 1 << b;
    static constexpr int INTMX = numeric_limits<int>::max();
    int n, m, h;
    vector<int> data, left_prod, right_prod;
    vector<vector<int>> sparse_table;
    StaticRMQ() {};
    void build(const vector<int>& v){
      data = v;
      n = data.size();
      if(n <= B) return;
      m = ((n-1) >> b) + 1;
      left_prod.assign(n+1,INTMX);
      right_prod.assign(n+1,INTMX);
      h = 31-__builtin_clz(m);
      sparse_table.assign(h+1, vector(m+1,INTMX));
      int temp = INTMX;
      for(int i = 0; i < n; i++){
        left_prod[i] = temp;
        temp = min(temp, data[i]);
        if(!(~i&(B-1))) sparse_table[0][i >> b] = temp, temp = INTMX;
      }
      left_prod[n] = temp;
      if(n&(B-1)) sparse_table[0][m-1] = temp, temp = INTMX;
      for(int i = (n&~(B-1))-1; i >= 0; i--){
        temp = min(data[i], temp);
        right_prod[i] = temp;
        if(!(i&(B-1))) temp = INTMX;
      }
      sparse_table[0][m] = sparse_table[0][m-1];
      for(int h0 = 1; h0 <= h; h0++){
        int i = 1 << h0;
        for(int j = i; j <= m; j += i << 1){
          temp = INTMX;
          for(int k = j-1; k >= j - i; k--) temp = min(sparse_table[0][k], temp), sparse_table[h0][k] = temp;
          temp = INTMX;
          for(int k = j; k < min(j + i,m+1); k++) sparse_table[h0][k] = temp, temp = min(temp, sparse_table[0][k]);
        }
      }
      for(int i = 1; i <= m; i += 2) sparse_table[0][i] = INTMX;
    }
    inline int naive_prod(int l, int r) const {
      int res = INTMX;
      for(int i = l; i < r; i++) res = min(res,data[i]);
      return res;
    }
    int prod(int l, int r) const {
      if(l >= r) return INTMX;
      if(r-l <= 8) return naive_prod(l,r);
      int l1 = (l >> b) + 1;
      int r1 = r >> b;
      if(l1 > r1) return naive_prod(l,r);
      if(l1 == r1) return min(right_prod[l], left_prod[r]);
      int mid = 31-__builtin_clz(l1^r1);
      return min({right_prod[l],sparse_table[mid][l1], sparse_table[mid][r1], left_prod[r]});
    }
  };

  struct Node {
    int l, r, link, term, count, depth;
    maptype next;
    Node(int l_ = 0, int r_ = 0) : l(l_), r(r_), link(-1), term(0), count(0), depth(0) {
      if constexpr(!USE_MAP) next.fill(-1);
    }
    inline int len() const { return r - l; }
  };

  private:
    int n;
    VC s;
    vector<Node> nodes;
    vector<int> suffix_node, sa, isa, lcp;
    StaticRMQ lcpRMQ;
    bool lcp_built = false;

    int active_node, active_e, active_len, remainder;

    inline int get_next(int node, C c) const {
      if constexpr (USE_MAP) {
        auto it = nodes[node].next.find(c);
        return it != nodes[node].next.end() ? it->second : -1;
      }else return nodes[node].next[c - MIN_CHARCTER];
    }
    inline void set_next(int node, C c, int next_node) {
      if constexpr (USE_MAP) nodes[node].next[c] = next_node;
      else nodes[node].next[c - MIN_CHARCTER] = next_node;
    }
    inline int create_node(int l, int r) {
      nodes.emplace_back(l, r);
      return nodes.size() - 1;
    }

    inline bool walk_down(int next_node, int pos) {
      int elen = next_node ? min(nodes[next_node].r, pos + 1) - nodes[next_node].l : 0;
      if(active_len >= elen){
        active_e += elen, active_len -= elen, active_node = next_node; 
        return true;
      }
      return false;
    }
    void build() {
      nodes.reserve(s.size()*2);
      create_node(0, 0);
      active_node = 0, active_e = -1, active_len = 0, remainder = 0;
      for(int i = 0; i < n; ++i) extend(i);

      int last_new_node = -1;
      while(remainder > 0) {
        if(!active_len){
          nodes[active_node].term++;
          if(~last_new_node) nodes[last_new_node].link = active_node, last_new_node = -1;
        }else{
          int next_node = get_next(active_node, s[active_e]);
          if(walk_down(next_node, n)) continue;
          int split_node = create_node(nodes[next_node].l, nodes[next_node].l + active_len);
          set_next(active_node, s[active_e], split_node);
          nodes[next_node].l += active_len;
          set_next(split_node, s[nodes[next_node].l], next_node);
          nodes[split_node].term++;
          if (last_new_node != -1) nodes[last_new_node].link = split_node;
          last_new_node = split_node;
        }
        remainder--;
        if(!active_node && active_len) active_len--, active_e = n - remainder;
        else if(active_node) active_node = nodes[active_node].link != -1 ? nodes[active_node].link : 0;
      }

      int id = 0, dep = 0;
      auto dfs = [&](auto self, int u, int depth) -> void {
        nodes[u].count = nodes[u].term;
        nodes[u].depth = depth;
        if(nodes[u].term) {
          suffix_node[n-depth] = u;
          sa[id] = n - depth;
          isa[n - depth] = id;
          if(id) lcp[id-1] = dep;
          ++id;
          dep = depth;
        }
        if constexpr (USE_MAP) {
          for(auto& [_, nxt] : nodes[u].next) if(~nxt) {
            self(self, nxt, depth + nodes[nxt].len());
            dep = min(dep,depth);
            nodes[u].count += nodes[nxt].count;
          }
        }
        else for(int i = 0; i < ALPHA; ++i) {
          int nxt = nodes[u].next[i];
          if(~nxt) {
            self(self, nxt, depth + nodes[nxt].len());
            dep = min(dep,depth);
            nodes[u].count += nodes[nxt].count;
          }
        }
      };
      dfs(dfs,0,0);
    }
    void extend(int pos){
      ++remainder;
      int last_new_node = -1;
      while(remainder){
        if(!active_len) active_e = pos;
        int next_node = get_next(active_node, s[active_e]);
        if(!~next_node){
          int leaf = create_node(pos,n);
          nodes[leaf].term = 1;
          set_next(active_node, s[active_e], leaf);
          if(~last_new_node) nodes[last_new_node].link = active_node, last_new_node = -1;
        }else{
          if(walk_down(next_node, pos)) continue;
          if(s[nodes[next_node].l + active_len] == s[pos]) {
            active_len++;
            if(last_new_node != -1 && active_node != 0) nodes[last_new_node].link = active_node;
            break;
          }
          int split_node = create_node(nodes[next_node].l, nodes[next_node].l + active_len);
          set_next(active_node, s[active_e], split_node);
          int leaf = create_node(pos,n);
          nodes[leaf].term = 1;
          set_next(split_node, s[pos], leaf);
          nodes[next_node].l += active_len;
          set_next(split_node, s[nodes[next_node].l], next_node);
          if(~last_new_node) nodes[last_new_node].link = split_node;
          last_new_node = split_node;
        }
        --remainder;
        if(!active_node && active_len) --active_len, active_e = pos - remainder + 1;
        else if(active_node) active_node = nodes[active_node].link != -1 ? nodes[active_node].link : 0;
      }
    }
  public:
    SuffixTree() : n(0) {build();}
    SuffixTree(const VC& s_) : n(s_.size()), s(s_), suffix_node(n), sa(n), isa(n), lcp(n-1) {build();}
    const vector<Node>& get_nodes() const { return nodes; }
    const Node& get_node(int i) const { return nodes[i]; }
    const vector<int>& suffix_array() const { return sa; }
    const vector<int>& suffix_array_inv() const { return isa; }
    const vector<int>& lcp_array() const { return lcp; }
    long long distinct_substrings() const {
      long long res = (long long) n * (n+1) / 2;
      for(int i = 0; i < n-1; ++i) res -= lcp[i];
      return res;
    }
    struct State {
      int v, nxt, l;
      State(int v_ = 0, int nxt_ = -1, int l_ = 0) : v(v_), nxt(nxt_), l(l_){}
    };

    inline int state_left(const State& st) const {return (~st.nxt ? nodes[st.nxt].l : nodes[st.v].r) - nodes[st.v].depth;}
    inline int state_right(const State& st) const {return (~st.nxt ? nodes[st.nxt].l : nodes[st.v].r) + st.l;}
    inline int state_len(const State& st) const {return nodes[st.v].depth + st.l;}
    inline int state_count(const State& st) const {return nodes[~st.nxt ? st.nxt : st.v].count;}
    
    void next_state(State& st, C c) const {
      if(!n) return;
      while(1){
        if(!st.l){
          int child = get_next(st.v, c);
          if(~child) {
            if(++st.l == nodes[st.nxt=child].len() && nodes[st.nxt].r != n) st.v = st.nxt, st.nxt = -1, st.l = 0;
            return;
          }else{
            if(!st.v) return;
            st.v = nodes[st.v].link;
            if(!~st.v) st.v = 0;
          }
        }else{
          if(st.l < nodes[st.nxt].len() && s[nodes[st.nxt].l + st.l] == c) {
            if(++st.l == nodes[st.nxt].len() && nodes[st.nxt].r != n) st.v = st.nxt, st.nxt = -1, st.l = 0;
            return;
          }else{
            int start_idx, remain;
            if(!st.v) start_idx = nodes[st.nxt].l + 1, remain = st.l - 1, st.v = 0;
            else{
              st.v = nodes[st.v].link; 
              if(!~st.v) st.v = 0;
              start_idx = nodes[st.nxt].l, remain = st.l;
            }
            bool on_leaf_edge = false;
            while(remain) {
              int child = get_next(st.v, s[start_idx]);
              int elen = nodes[child].len();
              if(remain < elen || (remain == elen && nodes[child].r == n)){
                st.nxt = child, st.l = remain;
                on_leaf_edge = true;
                break;
              }
              remain -= elen, start_idx += elen, st.v = child;
            }
            if(!on_leaf_edge) st.nxt = -1, st.l = 0;
          }
        }
      }
    }
    template<class U> vector<State> matching_states(const U& T) const {
      vector<State> res(T.size(), 0);
      State st;
      for(int i = 0; i < (int)T.size(); ++i){
        next_state(st, T[i]);
        res[i] = st;
      }
      return res;
    }
    // S[a,a+len) = T[b,b+len)
    template<class U> tuple<int,int,int> longest_common_substring(const U& T){
      int a = 0, b = 0, len = 0;
      State st;
      for(int i = 0; i < (int)T.size(); ++i){
        next_state(st, T[i]);
        if(len < state_len(st)){
          len = state_len(st);
          a = state_left(st);
          b = i + 1 - len;
        }
      }
      return {a,b,len};
    }
    template<class U> int count_occurrences(const U& T){
      State st;
      for(auto c : T) next_state(st, c);
      if(state_len(st) < static_cast<int>(T.size())) return 0;
      return state_count(st);
    }
    // LCP of S[i,n) and S[j,n);
    int suffix_LCP(int i, int j){
      if(i == j) return n-i;
      if(isa[i] > isa[j]) swap(i,j);
      if(!lcp_built) lcpRMQ.build(lcp), lcp_built = true;
      return lcpRMQ.prod(isa[i],isa[j]);
    }
};

}  // namespace soryuusi
