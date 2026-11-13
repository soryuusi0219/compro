# mo

[索引](../README.md) / [実装](../../../library/data_structure/mo.hpp)

## 機能・前提条件

オフライン区間クエリを並べ替え、左右の追加・削除と回答callbackを呼ぶ。登録順のIDで結果を保存する。

## インターフェース

```cpp
#include "library/data_structure/mo.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Mo;
Mo(int n_, int q_);
void add_query(int l, int r);
template <typename AL, typename AR, typename DL, typename DR, typename SOL>
void run(const AL &add_left, const AR &add_right, const DL &delete_left, const DR &delete_right, const SOL &solve);
```

## 計算量

Q ソート O(Q log Q)、区間移動 O(N√Q+Q) 回程度。各callbackのコストを掛ける。空間 O(Q)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_inversions_mo`](../../../verify/library_checker/data_structure/static_range_inversions_mo.cpp)
- [`library_checker/static_range_mode_query_increment_decrement_ordered_multiset`](../../../verify/library_checker/data_structure/static_range_mode_query_increment_decrement_ordered_multiset.cpp)
