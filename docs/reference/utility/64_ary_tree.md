# 64_ary_tree

[索引](../README.md) / [実装](../../../library/utility/64_ary_tree.hpp)

## 機能・前提条件

整数キーの集合を64分木bit列で保持する。上限を `n` とし、`insert`、`erase`、`contains`、`operator[]` のキーは `[0,n)`。既にあるキーの追加や、存在しないキーの削除もできる。既定構築は `n == 0` の空集合。

`next(x)` は `x` 以上の最小要素、`prev(x)` は `x` 未満の最大要素。見つからなければ、それぞれ `n`、`T(-1)` を返す。符号なし型の `T(-1)` は型の最大値。

各段のビット列を一つの配列に格納する。子のワードが空から非空に変わったときだけ、挿入を上の段に伝播する。削除は子のワードが空になったときに伝播する。

## インターフェース

```cpp
#include "library/utility/64_ary_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct BitSetSet;
using u64 = uint64_t;
BitSetSet() = default;
BitSetSet(T n_);
void insert(T x);
void erase(T x);
bool operator[](T x) const;
bool contains(T x);
T next(T x);
T prev(T x);
```

## 計算量

段数を $H=O(1+\log_{64}(n+1))$ とすると、更新・前後探索は最悪 $O(H)$、所属判定は $O(1)$。空間は $O(1+n/64)$ ワード。構築は同じワード数の初期化が必要。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`library_checker/predecessor_problem`](../../../verify/library_checker/data_structure/predecessor_problem.cpp)
- [`unit/utility_misc`](../../../verify/unit/utility_misc.cpp)
