# eertree

[索引](../README.md) / [実装](../../../library/string/eertree.hpp)

## 機能・前提条件

文字を末尾追加し異なる回文をノード化する回文木。suffix_link/ref/length/maximal_suffixを参照する。

## インターフェース

```cpp
#include "library/string/eertree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class C = char, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z', bool USE_MAP = false>
struct Eertree;
using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
using maptype = conditional_t<USE_MAP, map<C,int>, array<int,ALPHA>>;
Eertree();
Eertree(int n_);
Eertree(const VC& s);
void push_back(C c);
inline int node_size() const;
inline int suffix_link(int i) const;
inline int parent(int i) const;
inline int get_next(int i, C c) const;
inline pair<int,int> ref(int i) const;
inline int length(int i) const;
inline int maximal_suffix(int i) const;
```

## 計算量

固定文字種で合計 O(NΣ) を上界とする構築、map版は O(N log Σ) 程度。空間 O(NΣ) または O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/eertree`](../../../verify/library_checker/string/eertree.cpp)
