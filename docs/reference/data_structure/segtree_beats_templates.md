# segtree_beats_templates

[索引](../README.md) / [実装](../../../library/data_structure/segtree_beats_templates.hpp)

## 機能・前提条件

chmin/chmax/addと区間和・最小・最大のBeats設定。vectorからの構築を推奨する。

## インターフェース

```cpp
#include "library/data_structure/segtree_beats_templates.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Sum>
struct segtree_beats_chmin_chmax_add;
struct S;
S() = default;
S(T x, int count = 1);
struct F;
static F chmin(T x);
static F chmax(T x);
static F add(T x);
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
static constexpr S (*mapping)(F,S) = [](F f, S x)->S;
static constexpr F (*composition)(F,F) = [](F f, F g)->F;
static constexpr F (*id)() = []()->F;
using Base = segtree_beats<S,op,e,F,mapping,composition,id>;
template<class T, class Sum>
struct segtree_beats_chmin_add;
struct S;
S() = default;
S(T x, int count = 1);
struct F;
static F chmin(T x);
static F add(T x);
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
static constexpr S (*mapping)(F,S) = [](F f, S x)->S;
static constexpr F (*composition)(F,F) = [](F f, F g)->F;
static constexpr F (*id)() = []()->F;
using Base = segtree_beats<S,op,e,F,mapping,composition,id>;
template<class T, class Sum>
struct segtree_beats_chmax_add;
struct S;
S() = default;
S(T x, int count = 1);
struct F;
static F chmax(T x);
static F add(T x);
static constexpr S (*op)(S,S) = [](S x, S y)->S;
static constexpr S (*e)() = []()->S;
static constexpr S (*mapping)(F,S) = [](F f, S x)->S;
static constexpr F (*composition)(F,F) = [](F f, F g)->F;
static constexpr F (*id)() = []()->F;
using Base = segtree_beats<S,op,e,F,mapping,composition,id>;
template<class T, class Sum = T>
struct SegtreeBeatsChminChmaxAdd : internal::segtree_beats_chmin_chmax_add<T,Sum>::Base;
using Operations = internal::segtree_beats_chmin_chmax_add<T,Sum>;
using Base = typename Operations::Base;
using value_type = typename Operations::S;
using action_type = typename Operations::F;
using Base::Base;
SegtreeBeatsChminChmaxAdd() = default;
template<class U> requires std::is_same_v<T,U>
SegtreeBeatsChminChmaxAdd(const std::vector<U>& v);
void chmin(int l, int r, T x);
void chmax(int l, int r, T x);
void add(int l, int r, T x);
template<class T, class Sum = T>
struct SegtreeBeatsChminAdd : internal::segtree_beats_chmin_add<T,Sum>::Base;
using Operations = internal::segtree_beats_chmin_add<T,Sum>;
using Base = typename Operations::Base;
using value_type = typename Operations::S;
using action_type = typename Operations::F;
using Base::Base;
SegtreeBeatsChminAdd() = default;
template<class U> requires std::is_same_v<T,U>
SegtreeBeatsChminAdd(const std::vector<U>& v);
void chmin(int l, int r, T x);
void add(int l, int r, T x);
template<class T, class Sum = T>
struct SegtreeBeatsChmaxAdd : internal::segtree_beats_chmax_add<T,Sum>::Base;
using Operations = internal::segtree_beats_chmax_add<T,Sum>;
using Base = typename Operations::Base;
using value_type = typename Operations::S;
using action_type = typename Operations::F;
using Base::Base;
SegtreeBeatsChmaxAdd() = default;
template<class U> requires std::is_same_v<T,U>
SegtreeBeatsChmaxAdd(const std::vector<U>& v);
void chmax(int l, int r, T x);
void add(int l, int r, T x);
```

## 計算量

構築 O(N)、通常問い合わせ O(log N)。更新は償却解析が必要で、一回最悪 O(N)。空間 O(N)。

## 依存関係

- [segtree_beats.hpp](segtree_beats.md)

## Verify・利用例

- [`library_checker/range_chmin_chmax_add_range_sum`](../../../verify/library_checker/data_structure/range_chmin_chmax_add_range_sum.cpp)
