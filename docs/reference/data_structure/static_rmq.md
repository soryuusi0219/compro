# static_rmq

[索引](../README.md) / [実装](../../../library/data_structure/static_rmq.hpp)

## 機能・前提条件

更新のない列の区間最小値・最大値を求める。値型は `<` で順序付ける。最適値と同順位の要素が複数あるときは、そのうちのいずれかを返す。

## インターフェース

```cpp
#include "library/data_structure/static_rmq.hpp"

soryuusi::StaticRMQ<int> minimum({3,1,4});
soryuusi::StaticRMQ<int,true> maximum({3,1,4});
minimum.prod(0,2);  // 1
maximum.prod(0,2);  // 3
```

- テンプレート引数は `T`、`Maximum = false`、`BlockSize = 32`、空区間用の `identity` の順。最小値・最大値の切り替えは `Maximum` で行う。
- 既定構築、`StaticRMQ(std::vector<T> values)`、`build(std::vector<T> values)` に対応する。構築後の `data` は読み取り専用として扱う。
- `prod(l,r)` は半開区間 `[l,r)` の最適値。`naive_prod(l,r)` は同じ区間を走査する。
- 空区間の既定値は `std::numeric_limits<T>` の上限または下限。無限大がある型は正負の無限大を使う。ペアなどの型には `T` の値を返す `constexpr` 関数 `identity` を渡す。
- `BlockSize` は `1,2,4,8,16,32,64` または `0`。`1` は全要素の疎表、`0` は列長の対数に応じて幅を選ぶ。添字と列長は `int` に収める。

## 計算量

入力長を $N$、ブロック幅を $B$、ブロック数を $M=\lceil N/B\rceil$ とすると、構築時間・空間は $O(N+M\log(M+1))$。`prod` は $O(1)$、`naive_prod` は $O(r-l)$。`BlockSize=0` では $B=\Theta(\log(N+1))$ となり、ワード演算を定数時間として構築時間・空間は $O(N)$。

ブロック内は単調スタックの候補をビットマスクで持ち、完全に含まれるブロックは疎表で処理する。中央の最適値を境界ブロックの全体値と比較し、結果が確定すれば境界内のビットマスクを読み込まずに返す。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [Static RMQ のLibrary Checker verify](../../../verify/library_checker/data_structure/staticrmq.cpp)
- [最小値・最大値のユニットテスト](../../../verify/unit/static_range_queries.cpp)
