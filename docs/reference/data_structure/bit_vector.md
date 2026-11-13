# bit_vector

[索引](../README.md) / [実装](../../../library/data_structure/bit_vector.hpp)

## 機能・前提条件

XOR線形基底への挿入関数 f(base,a)。独立なら基底を更新して true、従属なら false。整数のビット演算と大小比較を使う。

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

B bit、基底長 B に対し O(B) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
