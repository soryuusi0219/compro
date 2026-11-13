# mo_rollback

[索引](../README.md) / [実装](../../../library/data_structure/mo_rollback.hpp)

## 機能・前提条件

削除をrollbackで置き換えるMo法。add/del/reset/snapshot/rollback/solve のcallbackを指定する。

## インターフェース

```cpp
#include "library/data_structure/mo_rollback.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Mo_Rollback;
Mo_Rollback(int n_, int q_);
void add_query(int l, int r);
template <typename ADD, typename DEL, typename RES, typename SNAP, typename ROL, typename SOL>
void run(const ADD &add, const DEL &del, const RES& reset, const SNAP &snapshot, const ROL &rollback, const SOL &solve);
```

## 計算量

Q ソート O(Q log Q)、O(N√Q+Q) 回程度の追加・巻き戻し。空間 O(Q) に利用側の状態を加える。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_count_distinct_mo_rollback`](../../../verify/library_checker/data_structure/static_range_count_distinct_mo_rollback.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
