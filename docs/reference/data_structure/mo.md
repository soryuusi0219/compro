# mo

[索引](../README.md) / [実装](../../../library/data_structure/mo.hpp)

## 機能・前提条件

オフラインの半開区間 `[l,r)` を並べ替え、左右の追加・削除と回答callbackを呼びます。runは空区間 `[0,0)` から始めるため、利用側でも初期状態を空にします。solveに渡すIDは0始まりの登録順です。コンストラクタのqは想定件数で、runは登録済みの区間を処理します。

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

Q件のソートは O(Q log(Q+1))。ブロック幅bに対し端点の移動回数は O(Qb+N²/b+N) が上界です。左端のブロック境界を半ブロックずらした候補と比較し、移動回数を64bitで数えて選びます。空間は O(Q)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_inversions_mo`](../../../verify/library_checker/data_structure/static_range_inversions_mo.cpp)
- [`library_checker/static_range_mode_query_increment_decrement_ordered_multiset`](../../../verify/library_checker/data_structure/static_range_mode_query_increment_decrement_ordered_multiset.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
