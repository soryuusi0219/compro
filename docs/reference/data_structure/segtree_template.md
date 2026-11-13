# segtree_template

[索引](../README.md) / [実装](../../../library/data_structure/segtree_template.hpp)

## 機能・前提条件

和・最小最大・出現回数・ペア和・アフィン合成の定番Segtree設定。基底Segtreeの操作を継承する。

## インターフェース

```cpp
#include "library/data_structure/segtree_template.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct segtree_sum;
static constexpr T (*op)(T,T) = [](T x, T y)->T;
static constexpr T (*e)() = []()->T;
template<class T, T INF>
struct segtree_min;
static constexpr T (*op)(T,T) = [](T x, T y)->T;
static constexpr T (*e)() = []()->T;
template<class T, class U, T INF>
struct segtree_min_count;
using S = std::pair<T,U>;
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
using Base = Segtree<S,op,e>;
template<class T, T LOWEST>
struct segtree_max;
static constexpr T (*op)(T,T) = [](T x, T y)->T;
static constexpr T (*e)() = []()->T;
template<class T, class U, T LOWEST>
struct segtree_max_count;
using S = std::pair<T,U>;
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
using Base = Segtree<S,op,e>;
template<class T, class U>
std::vector<std::pair<T,U>> segtree_count_values(const std::vector<T>& v);
template<class T, class U>
struct segtree_pair_sum;
using S = std::pair<T,U>;
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
template<class T>
struct segtree_composite;
using S = std::pair<T,T>;
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
template<class T>
using SegtreeSum = Segtree<T,internal::segtree_sum<T>::op,internal::segtree_sum<T>::e>;
template<class T, T INF = std::numeric_limits<T>::max()/2>
using SegtreeMin = Segtree<T,internal::segtree_min<T,INF>::op,internal::segtree_min<T,INF>::e>;
template<class T, class U = int, T INF = std::numeric_limits<T>::max()/2>
struct SegtreeMinCount : internal::segtree_min_count<T,U,INF>::Base;
using Operations = internal::segtree_min_count<T,U,INF>;
using Base = typename Operations::Base;
using value_type = typename Operations::S;
using Base::Base;
SegtreeMinCount() = default;
template<class V> requires std::is_same_v<T,V>
SegtreeMinCount(const std::vector<V>& v);
template<class T, T LOWEST = std::numeric_limits<T>::lowest()/2>
using SegtreeMax = Segtree<T,internal::segtree_max<T,LOWEST>::op,internal::segtree_max<T,LOWEST>::e>;
template<class T, class U = int, T LOWEST = std::numeric_limits<T>::lowest()/2>
struct SegtreeMaxCount : internal::segtree_max_count<T,U,LOWEST>::Base;
using Operations = internal::segtree_max_count<T,U,LOWEST>;
using Base = typename Operations::Base;
using value_type = typename Operations::S;
using Base::Base;
SegtreeMaxCount() = default;
template<class V> requires std::is_same_v<T,V>
SegtreeMaxCount(const std::vector<V>& v);
template<class T, class U = T>
using SegtreePairSum = Segtree<std::pair<T,U>,internal::segtree_pair_sum<T,U>::op,internal::segtree_pair_sum<T,U>::e>;
template<class T>
using SegtreeComposite = Segtree<std::pair<T,T>,internal::segtree_composite<T>::op,internal::segtree_composite<T>::e>;
```

## 計算量

構築 O(N)、更新・問い合わせ O(log N)、空間 O(N)。

## 依存関係

- [segtree.hpp](segtree.md)

## Verify・利用例

- [`library_checker/point_add_range_sum_segtree`](../../../verify/library_checker/data_structure/point_add_range_sum_segtree.cpp)
- [`aoj/range_min_segtree_template`](../../../verify/aoj/data_structure/range_min_segtree_template.cpp)
