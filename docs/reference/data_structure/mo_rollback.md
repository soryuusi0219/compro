# mo_rollback

[索引](../README.md) / [実装](../../../library/data_structure/mo_rollback.hpp)

## 機能・前提条件

半開区間 `[l,r)` を扱うrollback Mo法。左端のブロックが変わるとresetを呼びます。右端を伸ばす追加はそのブロック内に残し、各回答の前にsnapshotと一時追加、回答の後に一時部分のdelとrollbackをこの順で呼びます。solveに渡すIDは0始まりの登録順です。コンストラクタのqは想定件数で、runは登録済みの区間を処理します。

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

Q件のソートは O(Q log(Q+1))。ブロック幅bではadd/delの合計が O(Qb+N²/b+N) 回です。これにresetと回答ごとのsnapshot/rollback/solveの処理費用が加わります。空間は O(Q) と利用側の状態です。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_count_distinct_mo_rollback`](../../../verify/library_checker/data_structure/static_range_count_distinct_mo_rollback.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
