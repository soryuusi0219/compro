# enumerate_quotients

[索引](../README.md) / [実装](../../../library/math/enumerate_quotients.hpp)

## 機能・前提条件

`EnumerateQuotients<T>(N)` は正整数iに対する `floor(N/i)` の異なる値を昇順に保持する。`operator[]` で値、`id(x)` で商集合内の値xの位置を得る。区間やtupleは返さない。

## インターフェース

```cpp
#include "library/math/enumerate_quotients.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct EnumerateQuotients;
EnumerateQuotients(T n);
inline int size() const;
inline T operator[](int i) const;
inline T id(int n);
```

## 計算量

O(√N) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/enumerate_quotients`](../../../verify/library_checker/number_theory/enumerate_quotients.cpp)
