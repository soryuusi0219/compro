# chromatic_polynomial

[索引](../README.md) / [実装](../../../library/graph/chromatic_polynomial.hpp)

## 機能・前提条件

小さいグラフの彩色多項式をFormalPowerSeriesで返す。指数時間のため頂点数を小さくする。

## インターフェース

```cpp
#include "library/graph/chromatic_polynomial.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T, typename INT>
FormalPowerSeries<T> chromatic_polynomial(const vector<vector<INT>>& G);
```

## 計算量

部分集合を使う指数時間。安全な上界は O(poly(V) 3^V)、空間 O(poly(V) 2^V)。係数演算コストを別途加える。

## 依存関係

- [formal_power_series.hpp](../math/formal_power_series.md)
- [set_power_series.hpp](../math/set_power_series.md)

## Verify・利用例

- [`library_checker/chromatic_polynomial`](../../../verify/library_checker/graph/chromatic_polynomial.cpp)
