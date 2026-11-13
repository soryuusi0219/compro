# aho_corasick

[索引](../README.md) / [実装](../../../library/string/aho_corasick.hpp)

## 機能・前提条件

固定文字集合の複数パターン照合automaton。insertで登録しbuild後にnode/queryで走査する。

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
