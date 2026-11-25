# aho_corasick

[索引](../README.md) / [実装](../../../library/string/aho_corasick.hpp)

## 機能・前提条件

固定文字集合の複数パターン照合automaton。`insert` で登録し、`build` で失敗リンクと遷移を確定してから `node`・`query` で走査する。登録は `build` より前に行う。構築済みの状態に対する `build` は何も行わない。`lex_order` は根から先に訪れ、各頂点の子を文字の大きい順にたどる。

## インターフェース

```cpp
#include "library/string/aho_corasick.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class C = char, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z'>
struct AhoCorasick;
AhoCorasick();
AhoCorasick(int N);
void insert(const VC& s);
void build();
inline int size() const;
vector<int> lex_order();
int node(const VC& s, int p = 0);
int query(const VC& s, int p = 0);
```

## 計算量

総ノードN・文字種Σに build O(NΣ)、文字列長Lの走査 O(L)、空間 O(NΣ)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/aho_corasick`](../../../verify/library_checker/string/aho_corasick.cpp)
- [`unit/string_algorithms`](../../../verify/unit/string_algorithms.cpp)
