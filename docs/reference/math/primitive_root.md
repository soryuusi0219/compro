# primitive_root

[索引](../README.md) / [実装](../../../library/math/primitive_root.hpp)

## 機能・前提条件

素数modの原始根を探索する。m-1を因数分解し候補を検査する。

## インターフェース

```cpp
#include "library/math/primitive_root.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
long long primitive_root(long long m);
```

## 計算量

因数分解に加え、候補数C・異なる素因数数Kに O(C K log m) 回程度のmod演算。

## 依存関係

- [prime_factorization.hpp](prime_factorization.md)

## Verify・利用例

- [`library_checker/primitive_root`](../../../verify/library_checker/number_theory/primitive_root.cpp)
