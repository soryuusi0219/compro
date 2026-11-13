# suffix_tree

[索引](../README.md) / [実装](../../../library/string/suffix_tree.hpp)

## 機能・前提条件

固定文字集合またはmapを使うsuffix tree。suffix array/LCP、異なる部分文字列数、文字列の探索状態を提供する。

## インターフェース

```cpp
#include "library/string/suffix_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class C = char, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z', bool USE_MAP = false>
struct SuffixTree;
using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
using maptype = conditional_t<USE_MAP, map<C,int>, array<int,ALPHA>>;
struct StaticRMQ;
StaticRMQ();
void build(const vector<int>& v);
inline int naive_prod(int l, int r) const;
int prod(int l, int r) const;
struct Node;
Node(int l_ = 0, int r_ = 0);
inline int len() const;
SuffixTree();
SuffixTree(const VC& s_);
const vector<Node>& get_nodes() const;
const Node& get_node(int i) const;
const vector<int>& suffix_array() const;
const vector<int>& suffix_array_inv() const;
const vector<int>& lcp_array() const;
long long distinct_substrings() const;
struct State;
State(int v_ = 0, int nxt_ = -1, int l_ = 0);
inline int state_left(const State& st) const;
inline int state_right(const State& st) const;
inline int state_len(const State& st) const;
inline int state_count(const State& st) const;
void next_state(State& st, C c) const;
template<class U> vector<State> matching_states(const U& T) const;
template<class U> tuple<int,int,int> longest_common_substring(const U& T);
template<class U> int count_occurrences(const U& T);
int suffix_LCP(int i, int j);
```

## 計算量

固定文字種Σで構築 O(NΣ) を上界とする。map版は遷移ごと O(log Σ)。空間 O(NΣ) または O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/longest_common_substring_suffix_tree`](../../../verify/library_checker/string/longest_common_substring_suffix_tree.cpp)
- [`library_checker/number_of_substrings_suffix_tree`](../../../verify/library_checker/string/number_of_substrings_suffix_tree.cpp)
- [`library_checker/suffix_array_suffix_tree`](../../../verify/library_checker/string/suffix_array_suffix_tree.cpp)
- [`unit/suffix_tree`](../../../verify/unit/suffix_tree.cpp)
