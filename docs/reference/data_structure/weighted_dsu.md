# weighted_dsu

[索引](../README.md) / [実装](../../../library/data_structure/weighted_dsu.hpp)

## 機能・前提条件

群上の相対ポテンシャルを保持するUnion-Find。op/e/invを指定し、mergeで差の制約を追加する。weightは同一成分で使用する。

`weight(i,j)` は `op(w[j],inv(w[i]))`。加法版ではjのポテンシャル−iのポテンシャルです。`merge(i,j,a)` はこの値がaとなる制約を追加し、新しい代表元を返します。既存制約と矛盾する場合は `-1` を返します。

## インターフェース

```cpp
#include "library/data_structure/weighted_dsu.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T, auto op, auto e, auto inv>
struct Weighted_dsu;
Weighted_dsu(int m);
int leader(int i);
bool same(int i, int j);
T weight(int i, int j);
int merge(int i, int j, T a);
int size(int i);
template<class T>
struct weighted_dsu_sum;
static constexpr T (*op)(T,T) = [](T x, T y)->T;
static constexpr T (*e)() = []()->T;
static constexpr T (*inv)(T) = [](T x)->T;
template<class T>
struct weighted_dsu_xor;
static constexpr T (*op)(T,T) = [](T x, T y)->T;
static constexpr T (*e)() = []()->T;
static constexpr T (*inv)(T) = [](T x)->T;
template<typename T>
using Weighted_dsu_sum = Weighted_dsu<T,internal::weighted_dsu_sum<T>::op,internal::weighted_dsu_sum<T>::e,internal::weighted_dsu_sum<T>::inv>;
template<typename T>
using Weighted_dsu_xor = Weighted_dsu<T,internal::weighted_dsu_xor<T>::op,internal::weighted_dsu_xor<T>::e,internal::weighted_dsu_xor<T>::inv>;
using Weighted_dsu_bool = Weighted_dsu_xor<bool>;
```

## 計算量

構築 O(N)、通常操作 償却 O(α(N))、空間 O(N)。群演算を O(1) とする。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/unionfind_with_potential`](../../../verify/library_checker/data_structure/unionfind_with_potential.cpp)
- [`library_checker/unionfind_with_potential_non_commutative_group`](../../../verify/library_checker/data_structure/unionfind_with_potential_non_commutative_group.cpp)
