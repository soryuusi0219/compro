# trie

[索引](../README.md) / [実装](../../../library/string/trie.hpp)

## 機能・前提条件

文字列多重集合のtrie。個数・prefix個数・辞書順k番目・LCP。文字は指定した閉区間内とする。

## インターフェース

```cpp
#include "library/string/trie.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class C, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z'>
struct Trie;
using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
struct Node;
Node(int _p=-1, C _pch=0);
void show_node(int idx, const string& indent, bool is_last) const;
Trie();
void insert(const VC& s);
int count(const VC& s) const;
int prefix_count(const VC& s) const;
int size() const;
VC kth(int k) const;
VC min_string() const;
VC max_string() const;
int longest_prefix(const VC& s) const;
int lcp(const VC& s) const;
int node_index_of(const VC& s) const;
void show() const;
```

## 計算量

長さLの挿入・探索 O(L)、k番目 O(LΣ)、ノード数Nに O(NΣ) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`aoj/dictionary_trie`](../../../verify/aoj/string/dictionary_trie.cpp)
- [`unit/trie`](../../../verify/unit/trie.cpp)
