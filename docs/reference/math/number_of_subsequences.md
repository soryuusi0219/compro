# number_of_subsequences

[索引](../README.md) / [実装](../../../library/math/number_of_subsequences.hpp)

## 機能・前提条件

空でない相異なる部分列の個数を数える。順序を保って要素を選び、同じ値の列が複数の添字集合から作れる場合は1つとして数える。空入力の答えは0。

要素型 `Value` は `std::hash<Value>` と等値比較を使えること。`Count` は整数またはmodintなどの計数型。整数型では空列を含む途中結果まで表せる型を使う。

## インターフェース

```cpp
#include "library/math/number_of_subsequences.hpp"

template<class Count, class Value>
Count number_of_subsequences(const std::vector<Value>& values);
```

## 計算量

入力長を $N$、相異なる値の個数を $D$ とする。ハッシュ表で値ごとに最後の出現の直前までの部分列数を保持する。時間は期待 $O(N)$、領域は $O(D)$。要素のハッシュや比較にかかる時間を含める場合は、要素型に依存する。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/number_of_subsequences`](../../../verify/library_checker/enumerative_combinatorics/number_of_subsequences.cpp)
- [`unit/combinatorics`](../../../verify/unit/combinatorics.cpp)
