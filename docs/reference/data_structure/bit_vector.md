# bit_vector

[索引](../README.md) / [実装](../../../library/data_structure/bit_vector.hpp)

## 機能・前提条件

XOR線形基底への挿入関数 f(base,a)。独立なら基底を更新してtrue、従属ならfalseを返します。最初はbaseを空にし、以降はこの関数で更新します。base[i]には最上位の1がi番にある基底ベクトルを格納し、欠けた位置は0です。符号付き整数は対応する符号なし整数のビット列として扱います。

## インターフェース

```cpp
#include "library/data_structure/bit_vector.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
bool f(vector<T>& base, T a);
```

## 計算量

整数型のbit幅Bに対し、最悪 O(B) 時間・空間。すべてのピボットが埋まっている場合は線形結合を計算せず、従属が確定します。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
