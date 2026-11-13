# stern_brocot

[索引](../README.md) / [実装](../../../library/math/stern_brocot.hpp)

## 機能・前提条件

正の有理数をStern–Brocot木の圧縮経路で表す。LCA、祖先、区間、二分探索を提供する。分子分母の積のオーバーフローに注意。

## インターフェース

```cpp
#include "library/math/stern_brocot.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct SternBrocot;
using Fraction = pair<T,T>;
using Path = vector<T>;
static Path encode_path(Fraction x);
static Fraction decode_path(const Path &p);
static Fraction lca(Fraction x, Fraction y);
static T depth(Fraction x);
static Fraction ancestor(T d, Fraction x);
static pair<Fraction,Fraction> range(Fraction x);
template<class F> static pair<Fraction,Fraction> bisect(F f, T MAX);
using SB = SternBrocot<long long>;
```

## 計算量

経路の連数をKとしてencodeはEuclid型 O(log max(p,q))、decode/LCA等 O(K)、空間 O(K)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/rational_approximation`](../../../verify/library_checker/number_theory/rational_approximation.cpp)
- [`library_checker/stern_brocot_tree`](../../../verify/library_checker/number_theory/stern_brocot_tree.cpp)
