# lyndon_factorization

[索引](../README.md) / [実装](../../../library/string/lyndon_factorization.hpp)

## 機能・前提条件

文字列を辞書順非増加のLyndon語列へ分解し境界位置列を返す。

## インターフェース

```cpp
#include "library/string/lyndon_factorization.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
vector<int> LyndonFactorization(const string& s);
```

## 計算量

O(N) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/lyndon_factorization`](../../../verify/library_checker/string/lyndon_factorization.cpp)
