# smawk_dp

[索引](../README.md) / [実装](../../../library/utility/smawk_dp.hpp)

## 機能・前提条件

全単調なDP遷移のSMAWK高速化。未確定状態の参照を避けるよう遷移callbackを設計する。

## インターフェース

```cpp
#include "library/utility/smawk_dp.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class F, class Compare = less<T>>
vector<T> SMAWK_dp(int n, const F& f, const Compare &comp = Compare());
```

## 計算量

N状態に O(N log N) 回の評価を上界とする。空間 O(N)。

## 依存関係

- [smawk.hpp](smawk.md)

## Verify・利用例

- [`unit/monge`](../../../verify/unit/monge.cpp)
