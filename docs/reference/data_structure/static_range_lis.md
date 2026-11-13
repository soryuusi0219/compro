# static_range_lis

[索引](../README.md) / [実装](../../../library/data_structure/static_range_lis.hpp)

## 機能・前提条件

静的な数列の半開区間 `[l,r)` に含まれる最長増加部分列の長さを問い合わせます。
値が全て異なる場合は厳密増加のLISです。**重複値がある場合は従来どおり非減少部分列**を
数えます。例えば `{1,1}` の答えは2です。配列の更新・部分列の復元は提供しません。

`Compare` はデフォルト構築可能な狭義弱順序です。比較上等価な要素は元の添字順に並べます。
`T` に `operator==` は必要ありません。`greater<T>` を指定すると非増加部分列を扱います。
空配列も構築できます。配列長・添字は `int` の範囲内で使用します。

## インターフェース

```cpp
#include "library/data_structure/static_range_lis.hpp"

template<class T, class Compare = std::less<T>>
struct StaticRangeLIS {
  StaticRangeLIS(const std::vector<T>& values);
  int query(int l, int r);
};
```

- コンストラクタで全体を前処理します。入力配列は変更しません。
- `query(l,r)` は `0 <= l <= r <= N` を要求し、空区間には0を返します。
- 型名、テンプレート引数、コンストラクタ、`query` の戻り値は従来どおりです。

```cpp
std::vector<int> a = {3,1,4,2,5};
soryuusi::StaticRangeLIS<int> lis(a);
assert(lis.query(0,5) == 3);
assert(lis.query(1,4) == 2);
assert(lis.query(2,2) == 0);
```

## 計算量

Nを配列長、Bを内部wavelet matrixのbit幅（現在31）とします。

- 前処理: `O(N log² N + NB)` 時間。seaweedの分割統治を行います。
- 問い合わせ: `O(B)` 時間（添字幅に合わせる通常の評価では `O(log N)`）。空区間は `O(1)`。
- 作業領域: `O(N)` 個の整数と、`O(NB/64+B)` wordのwavelet matrix、`O(log N)` の再帰領域。

値自体は比較のみで扱い、wavelet matrixには位置から作った非負整数を格納します。

## 依存関係

- [wavelet.hpp](wavelet.md)

## Verify・利用例

- [`library_checker/static_range_lis`](../../../verify/library_checker/data_structure/static_range_lis.cpp): 順列のStatic Range LIS Query。
- [`unit/static_range_lis`](../../../verify/unit/static_range_lis.cpp): 全区間を二乗DPと比較。重複値・負数・逆順比較・比較専用型・空配列・bit境界を含みます。
